#include "json_reader.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

using namespace std::literals;

namespace json_reader::detail{
    
 CommandForStop FillComForStops(const json::Dict& elem) {
        CommandForStop result;

        for (const auto& [key, value] : elem){
            if (key == "name"s) {
                result.name = value.AsString();
            } else if (key == "latitude"s) {
                result.shirina_visota.lat = value.AsDouble();
            } else if (key == "longitude") {
                result.shirina_visota.lng = value.AsDouble();
            } else if (key == "road_distances") {
                for (const auto& [name, dist] : value.AsMap()) {
                    result.distanse.push_back({name, dist.AsDouble()});
                }
            }
        }
        
        return result;  
    } 

    CommandForBus FillComForBuses(const json::Dict& elem) {
        CommandForBus result;

        result.is_roundtrip = elem.at("is_roundtrip"s).AsBool();
        for (const auto& [key, value] : elem) {
            if (key == "name"s) {
                result.name = value.AsString();
            } else if (key == "stops") {

                for (const auto& stop : value.AsArray()) {
                    if (stop.IsString()){
                        result.routes.push_back(stop.AsString());  
                    }
                }

                if (!result.is_roundtrip) {
                    for (auto it = value.AsArray().rbegin()+1; it != value.AsArray().rend(); ++it ) {
                        result.routes.push_back((*it).AsString());
                    }
                }
            } 
        
        }

        return result;
    }

    CommandRequest FillRequest(const json::Dict& elem) {
        CommandRequest result;

        for (const auto& [key,value] : elem) {
            if (key == "id"s) {
                result.id = value.AsInt();
            } else if (key == "type"s) {
                result.type = value.AsString();
            } else if (key == "name"s) {
                result.name = value.AsString();
            }
        }

        return result;
    }

    svg::Color GetColor (const json::Node node) {
        if (node.IsString()) {
            return node.AsString();
        }

        auto& arr = node.AsArray();

        if (arr.size() == 3) {
            return svg::Rgb{static_cast<uint8_t>(arr[0].AsInt()), static_cast<uint8_t>(arr[1].AsInt()), static_cast<uint8_t>(arr[2].AsInt())};
        }

        return svg::Rgba{static_cast<uint8_t>(arr[0].AsInt()), static_cast<uint8_t>(arr[1].AsInt()), static_cast<uint8_t>(arr[2].AsInt()), arr[3].AsDouble()};

    }

}

namespace json_reader {
    void JsonReader::ParseCommand(json::Document& doc){

        for (const auto& [key,value] : doc.GetRoot().AsMap()){
            if (key == "base_requests"s) {
                for (const auto& elem : value.AsArray()) {
                    if (elem.AsMap().at("type"s).AsString() == "Stop"s){
                        com_for_stops_.push_back(detail::FillComForStops(elem.AsMap()));
                    }
                }
            }
        }

        for (const auto& [key, value] : doc.GetRoot().AsMap()) {
            if (key == "base_requests"s) {
                for (const auto& elem : value.AsArray()) {
                    if (elem.AsMap().at("type"s).AsString() == "Bus") {
                        com_for_bus_.push_back(detail::FillComForBuses(elem.AsMap()));
                    }
                }
            }
        }

        for (const auto& [key, value] : doc.GetRoot().AsMap()) {
            if (key == "stat_requests"s) {
                for (const auto& elem : value.AsArray()){
                    com_request_.push_back(detail::FillRequest(elem.AsMap()));
                }
            } else if (key == "render_settings"s) {
                if (value.IsMap()) {
                    json::Node elem(value.AsMap()); 
                    render_s_.width = elem.AsMap().at("width"s).AsDouble();
                    render_s_.height = elem.AsMap().at("height"s).AsDouble();
                    render_s_.padding = elem.AsMap().at("padding"s).AsDouble();
                    render_s_.line_width = elem.AsMap().at("line_width"s).AsDouble();
                    render_s_.stop_radius = elem.AsMap().at("stop_radius"s).AsDouble();
                    render_s_.bus_label_font_size = elem.AsMap().at("bus_label_font_size"s).AsInt();

                    render_s_.bus_label_offset[0] = elem.AsMap().at("bus_label_offset"s).AsArray().at(0).AsDouble();
                    render_s_.bus_label_offset[1] = elem.AsMap().at("bus_label_offset"s).AsArray().at(1).AsDouble();

                    render_s_.stop_label_font_size = elem.AsMap().at("stop_label_font_size"s).AsInt();

                    render_s_.stop_label_offset[0] = elem.AsMap().at("stop_label_offset"s).AsArray().at(0).AsDouble();
                    render_s_.stop_label_offset[1] = elem.AsMap().at("stop_label_offset"s).AsArray().at(1).AsDouble();

                    render_s_.underlayer_color = detail::GetColor(elem.AsMap().at("underlayer_color"s));

                    render_s_.underlayer_width = elem.AsMap().at("underlayer_width"s).AsDouble();

                    auto arr = elem.AsMap().at("color_palette"s).AsArray();
                    for (const auto& it : arr){
                        render_s_.color_palette.push_back(detail::GetColor(it));
                    }

                                      
                } else {
                    std::cout << "not a map"s << std::endl;
                }
            }
        }     
    }

        

    void JsonReader::ApplyCommands(transport::TransportCatalogue& catalogue) const {
        
        for (const auto& elem : com_for_stops_) {
            catalogue.AddStop(elem.name, elem.shirina_visota);
        }

        for (const auto& elem : com_for_stops_) {
            catalogue.AddDistanse(elem.name, elem.distanse);
        }

        for (const auto& elem : com_for_bus_) {
            catalogue.AddBus(elem.name, elem.routes);
        }    
    }

    std::vector<CommandRequest> JsonReader::GetRequest() const {
        return com_request_;
    }

    render::RenderSettings JsonReader::GetRenderSettings() const {
        return render_s_;
    }

    std::vector<CommandForBus> JsonReader::GetCommandForBus() const {
        return com_for_bus_;
    }

}