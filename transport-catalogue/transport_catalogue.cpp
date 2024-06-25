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

void TransportCatalogue::InfoAboutRoute(std::string_view name, std::ostream& out) const {
    Bus nash = FindRoute(name);

    if (nash.bus_name == "") {
        out << "Bus "<<  name << ": "<< "not found" << std::endl;
        return ;
    }

    out << "Bus "<<  nash.bus_name << ": " << nash.buses.size() << " stops on route, ";

    std::unordered_set<std::string_view> unic_stops;
    double lenght = 0.0;
    Coordinates proshlay;
    bool is_first = true;

    for (auto& elem : nash.buses) {
        if (is_first) {
            proshlay = elem->coordinates;
            is_first = false;
        } else {
            lenght += ComputeDistance(proshlay, elem->coordinates);
            proshlay = elem->coordinates;
        }
        unic_stops.insert(elem->stop_name);
    }

    out << unic_stops.size() << " unique stops, " << lenght << " route length" << std::endl;
   
    return;
}

void TransportCatalogue::InfoAboutStop(std::string_view name, std::ostream& out) const {
    if (stop_get_buses_.count(name) == 0) {
        out << "Stop " << name << ": " << "not found" << std::endl;
        return;
    }

    if (stop_get_buses_.at(name).empty()) {
        out << "Stop " << name << ": " << "no buses" << std::endl;
        return;
    }

    std::vector<std::string_view> nash;
    for (auto& elem : stop_get_buses_.at(name)){
        nash.push_back(elem);
    }

    sort(nash.begin(),nash.end());
    bool is_first = true;

    for (auto& bus : nash) {
        if (is_first) {
            out << "Stop " << name << ": " << "buses";
            is_first = false;
        }
        
        out << " " << bus;
    }

    out << std::endl;
}