#include "transport_catalogue.h"
#include <algorithm>

using namespace transport;

void TransportCatalogue::AddStop(std::string_view name, Coordinates shirina_visota){
    Stop elem;

    elem.stop_name = name;
    elem.coordinates = shirina_visota;

    stops_.push_back(std::move(elem));

    stopname_to_stops_.insert({stops_.back().stop_name, &stops_.back()});

    stop_get_buses_[stops_.back().stop_name];
}

void TransportCatalogue::AddBus(std::string_view name, std::vector<std::string_view> route){
    Bus elem;

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

Stop TransportCatalogue::FindStop(std::string_view name) const {
    if (stopname_to_stops_.count(name) > 0) {
        return *(stopname_to_stops_.at(name));
    }
    
    return Stop{};
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
    Coordinates proshlay;
    bool is_first = true;
    InfoAboutRoute result;

    if (nash.bus_name == "") {
        return result;
    }

    for (auto& elem : nash.buses) {
        if (is_first) {
            proshlay = elem->coordinates;
            is_first = false;
        } else {
            result.lenght += ComputeDistance(proshlay, elem->coordinates);
            proshlay = elem->coordinates;
        }
        unic_stops.insert(elem->stop_name);
    }

    result.name_route = nash.bus_name;
    result.counter_stops = nash.buses.size();
    result.unique_stops = unic_stops.size();

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