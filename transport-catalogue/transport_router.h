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
        , routing_settings_(routing_settings) {
    }

    void CreateGraph();

    const EdgeSettings& GetEdgeSettings(graph::EdgeId id) const;

private:
    graph::DirectedWeightedGraph<double>& routes_graph_; // изменим
    const transport::TransportCatalogue& transport_catalogue_;
    const RoutingSettings& routing_settings_;
    std::unordered_map<graph::EdgeId, EdgeSettings> edge_settings_; 
};

}