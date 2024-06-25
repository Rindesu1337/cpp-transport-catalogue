#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include "geo.h"

namespace transport{

struct Stop {
	std::string stop_name;
	Coordinates coordinates;
};

struct Bus {
	std::string bus_name;
	std::vector<Stop*> buses;
};

class TransportCatalogue {
	// Реализуйте класс самостоятельно
public:

	void AddStop(std::string_view name, Coordinates shirina_visota);
	void AddBus(std::string_view name, std::vector<std::string_view> route);
	Bus FindRoute(std::string_view name) const;//поиск маршрута по имени
	Stop FindStop(std::string_view name) const ;//поиск остановки по иени
	void InfoAboutRoute(std::string_view name, std::ostream& out) const ;
	void InfoAboutStop(std::string_view name, std::ostream& out) const;


private:
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> stopname_to_stops_;
	std::deque<Bus> route_;
	std::unordered_map<std::string_view, Bus*> busname_to_route_;
	std::unordered_map<std::string_view, std::unordered_set<std::string_view>> stop_get_buses_;
};

}