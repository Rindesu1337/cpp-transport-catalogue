#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <utility>
#include <optional>
#include "geo.h"
#include "domain.h"

namespace transport{

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

	void AddStop(std::string_view name, geo::Coordinates shirina_visota);
	void AddBus(std::string_view name, std::vector<std::string_view> route, bool is_roundt);
	void AddDistanse(std::string_view name, std::vector<std::pair<std::string_view, double>> distanse_to_stop);
	Bus FindRoute(std::string_view name) const;//поиск маршрута по имени
	Stop* FindStop(std::string_view name) const ;//поиск остановки по иени
	InfoAboutRoute GetInfoAboutRoute(std::string_view name) const ;
	std::vector<std::string_view> GetInfoAboutStop(std::string_view name) const;
	size_t GetStopCounter() const;
	std::pair<size_t, size_t> GetStopIndex(const Stop* stop) const;
	const std::deque<Bus>& GetAllBuses() const;
	std::optional<double> GetDistanseForPair(const Stop* stop_1, const Stop* stop_2) const;
	const std::deque<Stop>& GetAllStops() const;


private:
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> stopname_to_stops_;
	std::deque<Bus> route_;
	std::unordered_map<std::string_view, Bus*> busname_to_route_;
	std::unordered_map<std::string_view, std::unordered_set<std::string_view>> stop_get_buses_;
	std::unordered_map<std::pair<const Stop*, const Stop*>, double, PairHasher> distanse_to_stops_;
	std::unordered_map<const Stop*, std::pair<size_t, size_t>> stop_to_index_;
	size_t counter_ = 0;
};

}