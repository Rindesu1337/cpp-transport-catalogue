#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
#include "geo.h"
#include<string_view>
#include <vector>
#include <string>

struct EdgeSettings {
        std::string type_;
        double time_;
        std::string stop_name_;
        std::string bus_name_;
        int span_count_;
    };

struct Stop {
	std::string stop_name;
	geo::Coordinates coordinates;
};

struct Bus {
	std::string bus_name;
	std::vector<Stop*> buses;
	bool is_roundtrip;
};

struct InfoAboutRoute {
	std::string name_route;
	int counter_stops;
	int unique_stops;
	double geo_lenght = 0.0;
	double fact_lenght = 0;
};

struct CommandForStop {
    std::string_view name;
    geo::Coordinates shirina_visota;
    std::vector<std::pair<std::string_view, double>> distanse;
};

struct CommandForBus {
    std::string_view name;
    std::vector<std::string_view> routes;
	bool is_roundtrip;
};

struct CommandRequest {
    std::string_view type;
    std::string_view name;
    int id;
	std::string_view from;
	std::string_view to;
};

struct RoutingSettings {
	int bus_wait_time;
	double bus_velocity;
};


