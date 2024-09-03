#pragma once

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

#include "domain.h"
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "json_builder.h"

namespace json_reader {

class JsonReader {
    public:

    void ParseCommand(json::Document& doc);

    void ApplyCommands(transport::TransportCatalogue& catalogue ) const;

    json::Node GetJSONByRequests(const transport::TransportCatalogue& catalogue, const request::RequestHander& req_hend,const tr::TransportRouter& tr_router);

    render::RenderSettings GetRenderSettings() const;

    std::vector<CommandForBus> GetCommandForBus() const;

    const RoutingSettings& GetRoutingSettings() const;

private:
    std::vector<CommandForStop> com_for_stops_;
    std::vector<CommandForBus> com_for_bus_;
    std::vector<CommandRequest> com_request_;
    render::RenderSettings render_s_;
    RoutingSettings routing_setings_;
};

}