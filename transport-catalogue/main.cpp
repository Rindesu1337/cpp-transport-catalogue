#include "json_reader.h"
#include "request_handler.h"
#include "transport_router.h"
#include <fstream>

using namespace std::literals;

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */



    json_reader::JsonReader Reader;
    transport::TransportCatalogue catalogue;

    // Opening the input file stream and associate it with 
    // "input.txt" 
     /* std::ifstream fileIn("input.txt"); 
  
    // Redirecting cin to read from "input.txt" 
    std::cin.rdbuf(fileIn.rdbuf()); 
  
    // Opening the output file stream and associate it with 
    // "output.txt" 
    std::ofstream fileOut("output.txt"); 
  
    // Redirecting cout to write to "output.txt" 
    std::cout.rdbuf(fileOut.rdbuf());   */

    json::Document doc(json::Load(std::cin));


    //std::cout << doc.GetRoot().AsMap().at("base_requests"s).AsArray().at(1).AsMap().at("type"s).AsString() << std::endl;

    Reader.ParseCommand(doc);

    Reader.ApplyCommands(catalogue);

    render::MapRender Render(Reader.GetRenderSettings());

    graph::DirectedWeightedGraph<double> router_graph(catalogue.GetAllStops().size() * 2); // вершин 2 * кол-во остановок

    tr::TransportRouter transport_router(router_graph,catalogue, Reader.GetRoutingSettings());

    request::RequestHander Stat(catalogue, Render);

    Stat.GetCoordinatesForRoute(Reader.GetCommandForBus());


    
    json::Document doc_out(Reader.GetJSONByRequests(catalogue, Stat, transport_router));
    json::Print(doc_out, std::cout);
}