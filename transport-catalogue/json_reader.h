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

namespace json_reader {

class JsonReader {
    public:

    void ParseCommand(json::Document& doc);

    void ApplyCommands(transport::TransportCatalogue& catalogue ) const;

    json::Node GetJSONByRequests(transport::TransportCatalogue& catalogue, request::RequestHander& req_hend);

    render::RenderSettings GetRenderSettings() const;

    std::vector<CommandForBus> GetCommandForBus() const;

private:
    std::vector<CommandForStop> com_for_stops_;
    std::vector<CommandForBus> com_for_bus_;
    std::vector<CommandRequest> com_request_;
    render::RenderSettings render_s_;
};

}