#include "transport_router.h"

using namespace tr;
using namespace std::literals;

void TransportRouter::CreateGraph() {
    std::unordered_map<std::pair<graph::VertexId, graph::VertexId>, double, transport::PairHasher> tmp_pair_idx_to_distance;

    graph::VertexId idx_stop_from = 0;
    graph::VertexId idx_current_stop = 0;

    

    for (auto&& stop : transport_catalogue_.GetAllStops()) {
        //Edge между собой, ожидание
        size_t index_1_stop = transport_catalogue_.GetStopIndex(&stop).first;
        size_t index_2_stop = transport_catalogue_.GetStopIndex(&stop).second;
        double time = routing_settings_.bus_wait_time;
        
        graph::EdgeId edge_id = routes_graph_.AddEdge({index_1_stop, index_2_stop, time});

        EdgeSettings edge_s;
        edge_s.stop_name_ = stop.stop_name;
        edge_s.type_ = "Wait"s;
        edge_s.time_ = time;

        edge_settings_[edge_id] = edge_s;
    }

    //ребра по маршрутам

    for (auto&& bus : transport_catalogue_.GetAllBuses()) {
        
        for (auto&& it = bus.buses.begin(); (it + 1) != bus.buses.end(); ++it) {
            int span_count = 0;
            const Stop* from = *it;
            double time = 0.0;
            idx_stop_from = transport_catalogue_.GetStopIndex(from).second;
            for (auto&& next_it = it + 1; next_it != bus.buses.end(); ++next_it) {
                ++span_count;
                idx_current_stop = transport_catalogue_.GetStopIndex(*next_it).first;
                
                std::optional<double> distanse = transport_catalogue_.GetDistanseForPair(*(next_it - 1), *next_it);
                if (distanse == std::nullopt) {
                    throw std::logic_error("Can't find distanse"s);
                }

                time += (*distanse/routing_settings_.bus_velocity);

                graph::EdgeId edge_id = routes_graph_.AddEdge({idx_stop_from, idx_current_stop, time});

                EdgeSettings edge_s;
                edge_s.stop_name_ = from->stop_name;
                edge_s.type_ = "Bus"s;
                edge_s.time_ = time;
                edge_s.span_count_ = span_count;
                edge_s.bus_name_ = bus.bus_name;

                edge_settings_[edge_id] = edge_s;                
            }
            
        }
       
    }
}

const EdgeSettings& TransportRouter::GetEdgeSettings(graph::EdgeId id) const {
    return edge_settings_.at(id);
}