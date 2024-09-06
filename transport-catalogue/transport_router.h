#pragma once

#include "router.h"
#include "transport_catalogue.h"
#include "domain.h"

namespace tr {

class TransportRouter {
public:
    TransportRouter(graph::DirectedWeightedGraph<double>& routes_graph, const transport::TransportCatalogue& transport_catalogue, const RoutingSettings& routing_settings)
        : routes_graph_(routes_graph)
        , transport_catalogue_(transport_catalogue)
        , routing_settings_(routing_settings)
        , router_(CreateGraph()) { //Router<double> теперь скрыт в TransportRouter
    }

    std::optional<RouteInfo> FindRoute(std::string_view from, std::string_view to) const;

private:
    graph::DirectedWeightedGraph<double>& routes_graph_; // изменим
    const transport::TransportCatalogue& transport_catalogue_;
    const RoutingSettings& routing_settings_;
    std::unordered_map<graph::EdgeId, EdgeSettings> edge_settings_; 
    std::unordered_map<const Stop*, std::pair<size_t, size_t>> stop_to_index_;
    graph::Router<double> router_;

    std::pair<size_t, size_t> GetStopIndex(const Stop* stop) const;

    const EdgeSettings& GetEdgeSettings(graph::EdgeId id) const;

    void CreateVertexId();// нумерация остановок и их зеркал

    void CreateEdgesForDoubleVertex();
    void CreateEdgesByRoute();

    graph::DirectedWeightedGraph<double>& CreateGraph() ;
};

}