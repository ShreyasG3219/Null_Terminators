#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
/*
    Add other includes that you require, only write code wherever indicated
*/
#include "graph.hpp"

using json = nlohmann::ordered_json;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <graph.json> <queries.json> <output.json>" << std::endl;
        return 1;
    }

    std::ifstream graph_file(argv[1]);
    if(!graph_file.is_open())
    {
        std::cerr << "Failed to open " << argv[1] << std::endl;
        return 1;
    }

    json graph_json;
    graph_file>>graph_json;

    int nodes=graph_json["meta"]["nodes"];

    Graph G(nodes);

    for(auto& node : graph_json["nodes"])
    {
        G.addNode(node["id"],node["lat"],node["lon"],node["pois"]);
    }
    for(auto& edge : graph_json["edges"])
    {
        G.addedge(edge["id"],edge["u"],edge["v"],edge["length"],edge["average_time"],edge["speed_profile"],edge["oneway"],edge["road_type"]);
    }

    // Read Queries from Second file

    std::ifstream queries_file(argv[2]);
    if (!queries_file.is_open()) {
        std::cerr << "Failed to open " << argv[2] << std::endl;
        return 1;
    }
    json queries_json;
    queries_file >> queries_json;

    std::vector<json> results;

    for (const auto& query : queries_json["events"]) {
        auto start_time = std::chrono::high_resolution_clock::now();

       json result;
        try{
       if(query["type"]=="remove_edge") {
        G.removeedge(query["edge_id"]);
        result["id"]=query["id"];
        result["done"]=true;
       }
       else if(query["type"]=="modify_edge"){
        auto& patch=query["patch"];
        G.modify_edge(query["edge_id"],
                        patch.value("length", -1.0),
                        patch.value("average_time", -1.0),
                        patch.value("speed_profile", std::vector<double>{}),
                        patch.value("road_type", std::string{}));
        result["id"]=query["id"];
        result["done"]=true;
       }
       
       else if(query["type"]=="shortest_path"){
        double output=0;std::vector<int>path;bool status=false;

        if(query["mode"]=="distance")
            {status=G.shortest_distance_path(query["source"],query["target"],output,path,
                (query.contains("constraints") && query["constraints"].contains("forbidden_nodes"))? 
                query["constraints"]["forbidden_nodes"].get<std::vector<int>>() : std::vector<int>(0),
                (query.contains("constraints") && query["constraints"].contains("forbidden_road_types"))? 
                query["constraints"]["forbidden_road_types"].get<std::vector<std::string>>() : std::vector<std::string>(0));}
        else if(query["mode"]=="time")
            {status=G.shortest_time_path(query["source"],query["target"],output,path,
                            (query.contains("constraints") && query["constraints"].contains("forbidden_nodes"))? 
                            query["constraints"]["forbidden_nodes"].get<std::vector<int>>() : std::vector<int>(0),
                            (query.contains("constraints") && query["constraints"].contains("forbidden_road_types"))? 
                            query["constraints"]["forbidden_road_types"].get<std::vector<std::string>>() : std::vector<std::string>(0));}           
        result["id"]=query["id"];
        result["possible"]=status;

        if(status){
            result["minimum_time/minimum_distance"]=output;
            result["path"]=path;
        }

       }
    }
    catch(const std::exception& e){}

        auto end_time = std::chrono::high_resolution_clock::now();
        result["processing_time"] = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        results.push_back(result);

    }

    std::ofstream output_file(argv[3]);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output.json for writing" << std::endl;
        return 1;
    }
    json output;
    output["meta"]=queries_json["meta"];
    output["results"]=results;
    output_file << output.dump(4) << std::endl;
    return 0;
}