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
    , renderer_(renderer) {

}

void RequestHander::GetCoordinatesForRoute(const std::vector<CommandForBus>& route) { 

    std::vector<CommandForBus> sort_route(route);

    sort(sort_route.begin(), sort_route.end(), [] (CommandForBus& lhs, CommandForBus& rhs) {
        return lhs.name < rhs.name;           
    });

    std::vector<geo::Coordinates> nash;

    for (const auto& bus : sort_route) {
        for (const auto& stop : bus.routes) {
            nash.push_back(db_.FindStop(stop)->coordinates);
        }
    }

    render::SphereProjector to_point(nash.begin(),nash.end(), renderer_.GetWidth(), renderer_.GetHeight(), renderer_.GetPadding());

    for (const auto& bus : sort_route) {
        for (const auto& stop : bus.routes) {
            std::string s;
            s = stop;
            sort_vec_coor_[{bus.name, bus.is_roundtrip}].push_back(to_point(db_.FindStop(stop)->coordinates));
            stop_name_to_point[s] = to_point((db_.FindStop(stop)->coordinates));
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