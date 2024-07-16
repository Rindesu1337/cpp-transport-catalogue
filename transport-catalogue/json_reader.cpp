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

    json::Node JsonReader::GetJSONByRequests(transport::TransportCatalogue& catalogue, request::RequestHander& req_hend) {
        json::Array arr_node;

    for (const auto& req : com_request_) {
        if (req.type == "Stop"s) {
            std::vector<std::string_view> inf_stop = catalogue.GetInfoAboutStop(req.name);

            sort(inf_stop.begin(), inf_stop.end());

            if (inf_stop.empty() ) {
                json::Node dict_node{json::Dict{{"request_id"s, req.id},
                                                {"error_message"s , std::string("not found"s)}}};
                arr_node.push_back(dict_node);
            } else if (inf_stop.at(0) == "") {
                json::Array arr;
                json::Node dict_node{json::Dict{{"request_id", req.id}, {"buses"s, arr}}};
                arr_node.push_back(dict_node);
            } else {
                json::Array arr;

                for (auto& bus : inf_stop) {
                    std::string s;
                    s = bus;
                    json::Node elem(s);
                    arr.push_back(elem);
                }


                json::Node dict_node{json::Dict{{"request_id"s, req.id},
                                                {"buses"s, arr}}};
                arr_node.push_back(dict_node);
            }
        } else if (req.type == "Bus"s) {
            InfoAboutRoute inf_rout = catalogue.GetInfoAboutRoute(req.name);

            if (inf_rout.name_route == "") {
                json::Node dict_node {json::Dict{{"request_id"s, req.id},
                                                 {"error_message"s, std::string("not found"s)}}};
                arr_node.push_back(dict_node);
            } else {
                double curvature = inf_rout.fact_lenght/inf_rout.geo_lenght;
                json::Node dict_node{json::Dict{{"request_id"s, req.id},
                                            {"curvature"s, curvature},
                                            {"route_length"s, inf_rout.fact_lenght},
                                            {"stop_count"s, static_cast<int>(inf_rout.counter_stops)},
                                            {"unique_stop_count"s, static_cast<int>(inf_rout.unique_stops)}}};

                arr_node.push_back(dict_node);
            }
        } else if (req.type == "Map"s) {
            std::ostringstream strm;
            req_hend.RenderMap().Render(strm);

            json::Node dict_node{json::Dict{{"request_id"s, req.id},
                                            {"map"s, strm.str()}}};
            arr_node.push_back(dict_node);
        }
    }

    return arr_node;
    }

    render::RenderSettings JsonReader::GetRenderSettings() const {
        return render_s_;
    }

    std::vector<CommandForBus> JsonReader::GetCommandForBus() const {
        return com_for_bus_;
    }

}