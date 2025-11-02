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
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <graph.json> <queries.json>" << std::endl;
        return 1;
    }

    // Read graph from first file
    /*
        Add your graph reading and processing code here
        Initialize any classes and data structures needed for query processing
    */
    std::ifstream graph_file(argv[1]);
    if(!graph_file.is_open())
    {
        std::cerr << "Failed to open " << argv[1] << std::endl;
        return 1;
    }
    //Graph Processing
    json graph_json;graph_file>>graph_json;
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

    G.printedges();


    // Read queries from second file
    std::ifstream queries_file(argv[2]);
    if (!queries_file.is_open()) {
        std::cerr << "Failed to open " << argv[2] << std::endl;
        return 1;
    }
    json queries_json;
    queries_file >> queries_json;

    std::ofstream output_file("output.json");
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output.json for writing" << std::endl;
        return 1;
    }
    for (const auto& query : queries_json["events"]) {
        auto start_time = std::chrono::high_resolution_clock::now();

        /*
            Add your query processing code here
            Each query should return a json object which should be printed to sample.json
        */
       json result;
       if(query["type"]=="remove_edge")
       {
        bool status= G.removeedge(query["edge_id"]);
        result["done"]=status;
        G.printedges();
       }
       else if(query["type"]=="modify_edge")
       {
        bool status= G.modify_edge(query["edge_id"],query["patch"]["length"]);
        result["done"]=status;
        G.printedges();
       }
       else if(query["type"]=="shortest_path")
       {
        double output=0;
        std::vector<int>path;
        bool status=G.shortest_path(query["mode"],query["source"],query["target"],output,path,query["constraints"]["forbidden_nodes"],query["constraints"]["forbidden_road_types"]);
        result["id"]=query["id"];
        result["possible"]=status;
        if(status)
        {result["minimum_time/minimum_distance"]=output;
        result["path"]=path;}
        else{
            result["minimum_time/minimum_distance"]="NaN";
        result["path"]="NaN";
        }

       }
    //    if(query["type"]=="add_edge")
    //    {   
    //     auto& edge=query["edge_information"];
    //     bool status= G.addedge(edge["id"],edge["u"],edge["v"],edge["length"],edge["average_time"],edge["speed_profile"],edge["oneway"],edge["road_type"]);;
    //     result["done"]=status;
    //     G.printedges();
    //    }


        // Answer each query replacing the function process_query using 
        // whatever function or class methods that you have implemented
        // json result = process_query(query);
        

        auto end_time = std::chrono::high_resolution_clock::now();
        // result["processing_time"] = std::chrono::duration<double, std::milli>(end_time - start_time).count();

        output_file << result.dump(4) << '\n';
    }

    output_file.close();
    return 0;
}