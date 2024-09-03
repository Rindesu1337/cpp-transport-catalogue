#include "transport_catalogue.h"
#include <algorithm>

using namespace transport;
using namespace geo;


void TransportCatalogue::AddStop(std::string_view name, Coordinates shirina_visota){
    Stop elem;

    elem.stop_name = name;
    elem.coordinates = shirina_visota;

    //std::cout << shirina_visota.lat << " " << shirina_visota.lng << std::endl;

    stops_.push_back(std::move(elem));

    stopname_to_stops_.insert({stops_.back().stop_name, &stops_.back()});

    stop_get_buses_[stops_.back().stop_name];

    stop_to_index_[&stops_.back()].first = counter_; 
    stop_to_index_[&stops_.back()].second = ++counter_;
    ++counter_;
}

size_t TransportCatalogue::GetStopCounter() const {
    return counter_;
}

std::pair<size_t, size_t> TransportCatalogue::GetStopIndex(const Stop* stop) const {
    return stop_to_index_.at(stop);
}

const std::deque<Bus>& TransportCatalogue::GetAllBuses() const {
    return route_;
}

const std::deque<Stop>& TransportCatalogue::GetAllStops() const {
    return stops_;
}

std::optional<double> TransportCatalogue::GetDistanseForPair(const Stop* stop_1, const Stop* stop_2) const {
    auto it = distanse_to_stops_.find(std::make_pair(stop_1, stop_2));

        if (it == distanse_to_stops_.end()) {
            auto it_reverse = distanse_to_stops_.find(std::make_pair(stop_2, stop_1));

            if (it_reverse == distanse_to_stops_.end()) {
                return std::nullopt;
            }

            return it_reverse->second;
        }

    return it->second;
}

void TransportCatalogue::AddBus(std::string_view name, std::vector<std::string_view> route, bool is_roundt){
    Bus elem;

    elem.is_roundtrip = is_roundt;

    elem.bus_name = name;
    for (auto& stop : route) {
        elem.buses.push_back(stopname_to_stops_.at(stop));
    }

    route_.push_back(std::move(elem));

    for (auto& str : route_.back().buses) {
        stop_get_buses_[str->stop_name].insert(route_.back().bus_name);
    }

    busname_to_route_.insert({route_.back().bus_name, &route_.back()});
}

void TransportCatalogue::AddDistanse(std::string_view name, std::vector<std::pair<std::string_view, double>> distanse_to_stop){
    if (distanse_to_stop.empty()) {
        return;
    }

    std::pair<Stop*, Stop*> name_to_name2, name2_to_name;

    name_to_name2.first = stopname_to_stops_.at(name);
    name2_to_name.second = stopname_to_stops_.at(name);

    for (auto elem : distanse_to_stop) {
        name_to_name2.second = stopname_to_stops_.at(elem.first);
        name2_to_name.first = stopname_to_stops_.at(elem.first);

        distanse_to_stops_[name_to_name2] = elem.second;

        if (distanse_to_stops_.count(name2_to_name) == 0) {
            distanse_to_stops_[name2_to_name] = elem.second;
        }
    }
}

Stop* TransportCatalogue::FindStop(std::string_view name) const {
    if (stopname_to_stops_.count(name) > 0) {
        return stopname_to_stops_.at(name);
    }
    
    return nullptr;
}

Bus TransportCatalogue::FindRoute(std::string_view name) const {
    if (busname_to_route_.count(name) > 0) {
        return *(busname_to_route_.at(name));
    }

    return Bus{};
}

InfoAboutRoute TransportCatalogue::GetInfoAboutRoute(std::string_view name) const {

    Bus nash = TransportCatalogue::FindRoute(name);
    std::unordered_set<std::string_view> unic_stops;
    Stop* proshlay;
    bool is_first = true;
    InfoAboutRoute result;

    if (nash.bus_name == "") {
        return result;
    }

    for (auto& elem : nash.buses) {
        if (is_first) {
            proshlay = elem;
            is_first = false;
        } else {
            result.geo_lenght += ComputeDistance(proshlay->coordinates, elem->coordinates);

            std::pair<Stop*, Stop*> pair_stops;
            pair_stops.first = proshlay;
            pair_stops.second = elem;

            result.fact_lenght += distanse_to_stops_.at(pair_stops); 

            proshlay = elem;
        }
        unic_stops.insert(elem->stop_name);
    }

    result.name_route = nash.bus_name;
    result.counter_stops = static_cast<int>(nash.buses.size());
    result.unique_stops = static_cast<int>(unic_stops.size());

    return result;
}

std::vector<std::string_view> TransportCatalogue::GetInfoAboutStop(std::string_view name) const {  
    if (stop_get_buses_.count(name) == 0) {
        return std::vector<std::string_view>{};
    }

    std::vector<std::string_view> result(1);

    if (stop_get_buses_.at(name).empty()) {     
        return result;
    }

    result.clear();

    for (auto& elem : stop_get_buses_.at(name)){
        result.push_back(elem);
    }

    sort(result.begin(),result.end());

    return result;
} 