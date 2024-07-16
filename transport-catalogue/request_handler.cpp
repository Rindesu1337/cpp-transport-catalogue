#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

using namespace std::literals;

namespace request {
/* void RequestHander::SetRequest(const std::vector<domain::CommandRequest>& request) { 
    com_and_request_ = request;
} */

RequestHander::RequestHander(const transport::TransportCatalogue& db, const render::MapRender& renderer) 
    : db_(db)
    , renderer_(renderer) {}

void RequestHander::PrintStat(const std::vector<CommandRequest>& request, std::ostream& out) {
    json::Array arr_node;

    for (const auto& req : request) {
        if (req.type == "Stop"s) {
            std::vector<std::string_view> inf_stop = db_.GetInfoAboutStop(req.name);

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
            InfoAboutRoute inf_rout = db_.GetInfoAboutRoute(req.name);

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
            RenderMap().Render(strm);

            json::Node dict_node{json::Dict{{"request_id"s, req.id},
                                            {"map"s, strm.str()}}};
            arr_node.push_back(dict_node);
        }
    }

    json::Document doc(arr_node);
    json::Print(doc, out);
} 


void RequestHander::GetCoordinatesForRoute(const std::vector<CommandForBus>& route) { 

    std::vector<CommandForBus> sort_route(route);

    sort(sort_route.begin(), sort_route.end(), [] (CommandForBus& lhs, CommandForBus& rhs) {
        return lhs.name < rhs.name;           
    });

    std::vector<geo::Coordinates> nash;

    for (const auto& bus : sort_route) {
        for (const auto& stop : bus.routes) {
            nash.push_back(db_.FindStop(stop).coordinates);
        }
    }

    render::SphereProjector to_point(nash.begin(),nash.end(), renderer_.GetWidth(), renderer_.GetHeight(), renderer_.GetPadding());

    for (const auto& bus : sort_route) {
        for (const auto& stop : bus.routes) {
            std::string s;
            s = stop;
            sort_vec_coor_[{bus.name, bus.is_roundtrip}].push_back(to_point(db_.FindStop(stop).coordinates));
            stop_name_to_point[s] = to_point(db_.FindStop(stop).coordinates);
        }
    }
}

svg::Document RequestHander::RenderMap() const {
    svg::Document doc;

    size_t color_counter =0;
    for (const auto& [name, value] : sort_vec_coor_) {
        if (color_counter >= renderer_.GetColorPaletteSize()) {
            color_counter = 0;
        }

        renderer_.RenderBusLine(doc, value, color_counter);
        ++color_counter;
    }

    color_counter = 0;
    for (const auto& [name, value] : sort_vec_coor_) {
        if (color_counter >= renderer_.GetColorPaletteSize()) {
            color_counter = 0;
        }

        std::string name_bus;
        name_bus = name.first;

        renderer_.RenderBusName(doc, value[0], name_bus, color_counter);

        if (!name.second) {
            size_t middle_index = value.size()/2;
            if (value[middle_index].x != value[0].x and value[middle_index].y != value[0].y){
                renderer_.RenderBusName(doc, value[middle_index], name_bus, color_counter);
            }    
        }

        ++color_counter;
    }

    for (const auto& [name, value] : stop_name_to_point) {
        renderer_.RenderStopCircle(doc, value);
    }

    for (const auto& [name, value] : stop_name_to_point) {
        renderer_.RenderStopName(doc, value, name);
    }


    return doc;
}

}