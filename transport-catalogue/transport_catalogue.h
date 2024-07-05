#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <utility>
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

struct InfoAboutRoute {
	std::string name_route;
	size_t counter_stops;
	size_t unique_stops;
	double geo_lenght = 0.0;
	int fact_lenght = 0;
};

// Определение кастомного хешера для пары указателей
struct PairHasher {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1*, T2*>& pair) const {
        std::hash<T1*> hash1;
        std::hash<T2*> hash2;
        std::size_t h1 = hash1(pair.first);
        std::size_t h2 = hash2(pair.second);

        return h1 ^ (h2 << 1);
    }
};

class TransportCatalogue {
	// Реализуйте класс самостоятельно
public:

	void AddStop(std::string_view name, Coordinates shirina_visota);
	void AddBus(std::string_view name, std::vector<std::string_view> route);
	void AddDistanse(std::string_view name, std::vector<std::pair<std::string_view, int>> distanse_to_stop);
	Bus FindRoute(std::string_view name) const;//поиск маршрута по имени
	Stop FindStop(std::string_view name) const ;//поиск остановки по иени
	InfoAboutRoute GetInfoAboutRoute(std::string_view name) const ;
	std::vector<std::string_view> GetInfoAboutStop(std::string_view name) const;


private:
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> stopname_to_stops_;
	std::deque<Bus> route_;
	std::unordered_map<std::string_view, Bus*> busname_to_route_;
	std::unordered_map<std::string_view, std::unordered_set<std::string_view>> stop_get_buses_;
	std::unordered_map<std::pair<Stop*, Stop*>, int, PairHasher> distanse_to_stops_;
};

}