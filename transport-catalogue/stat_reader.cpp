#include "stat_reader.h"

void ParseAndPrintStat(const transport::TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {
    // Реализуйте самостоятельно
    std::string name,command;

    command = request.substr(request.find_first_not_of(' '));
    command = command.substr(0,command.find(' '));

    name = request.substr(request.find_first_not_of(' '));
    name = name.substr(name.find(' ')+1);

    if (command == "Bus") {
        transport::InfoAboutRoute inf_route = tansport_catalogue.GetInfoAboutRoute(name);

        if (inf_route.name_route == "") {
            output << "Bus "<<  name << ": "<< "not found" << std::endl;
            return;
        }

        output << "Bus "<< inf_route.name_route << ": " << inf_route.counter_stops << " stops on route, " << inf_route.unique_stops << " unique stops, " << inf_route.fact_lenght << " route length, " << inf_route.fact_lenght/inf_route.geo_lenght << " curvature" << std::endl;

        return;
    }

    if (command == "Stop") {
        std::vector<std::string_view> inf_stop = tansport_catalogue.GetInfoAboutStop(name);

        if (inf_stop.empty()) {
            output << "Stop " << name << ": " << "not found" << std::endl;
            return;
        }

        if (inf_stop[0] == "") {
            output << "Stop " << name << ": " << "no buses" << std::endl;
            return;
        }

        bool is_first = true;

        for (auto& bus : inf_stop) {
            if (is_first) {
                output << "Stop " << name << ": " << "buses";
                is_first = false;
            }
        
            output << " " << bus;
        }

        output << std::endl;

        return;
    }    
}