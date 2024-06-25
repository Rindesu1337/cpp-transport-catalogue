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
        tansport_catalogue.InfoAboutRoute(name, output);
    }
    
    if (command == "Stop") {
        tansport_catalogue.InfoAboutStop(name, output);
    }    
}