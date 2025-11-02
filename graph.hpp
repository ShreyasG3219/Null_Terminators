#include<bits/stdc++.h>
// enum ROADTUY
struct Node{
    int id;
    double lat,lon;
    std::vector<std::string>pois;
    Node(int id,double lat,double lon,std::vector<std::string> pois)
    {
        this->id=id;
        this->lat=lat;
        this->lon=lon;
        this->pois=pois;
    }
};
struct Edge{
    int id;
    int u,v;
    double length;
    double average_time;
    std::vector<double>speed_profile;
    bool oneway;
    std::string road_type;
    Edge(int id,int u,int v,double length,double average_time,std::vector<double>speed_profile,bool oneway,std::string road_type)
    {
        this->id=id;
        this->u=u;
        this->v=v;
        this->length=length;
        this->average_time=average_time;
        this->speed_profile=speed_profile;
        this->oneway=oneway;
        this->road_type=road_type;
    }
    void print()
    {
        std::cout<<"Edge Id "<<id<<" u "<<u<<" v "<<v<<" length "<<length<<std::endl;
    }
};
class Graph{
    private:
    int nodes;
    std::map<int,Node*>All_Nodes;
    std::map<int,std::map<int,std::vector<Edge*>>>Network;
    std::map<int,Edge*>Roads;
    public:
    Graph(int nodes)
    {
        this->nodes=nodes;
    }
    void addNode(int id,double lat,double lon,std::vector<std::string> pois)
    {
        All_Nodes[id]=new Node(id,lat,lon,pois);
    }
    bool addedge(int id,int u,int v,double length,double average_time,std::vector<double>speed_profile,bool oneway,std::string road_type)
    {
        if(All_Nodes.find(u)==All_Nodes.end() || All_Nodes.find(v)==All_Nodes.end())
        {return false;
        }
        Edge* newedge=new Edge(id,u,v,length,average_time,speed_profile,oneway,road_type);
        Network[u][v].push_back(newedge);
        if(oneway==false)
        {
            Network[v][u].push_back(newedge);
        }

        Roads[id]=newedge;
        return true;
    }
    bool removeedge(int id)
    {   
        if(Roads.find(id) == Roads.end())
        {
            return false;
        }
        auto& listedges=Network[Roads[id]->u][Roads[id]->v];
        listedges.erase(find(listedges.begin(),listedges.end(),Roads[id]));
        if(Roads[id]->oneway==false)
        {
            auto& listedges=Network[Roads[id]->v][Roads[id]->u];
        listedges.erase(find(listedges.begin(),listedges.end(),Roads[id]));

        }
        delete Roads[id];
        Roads.erase(id);
        return true;
        
    }
    void printedges()
    {
        for(auto& id_edge:Roads)
        {
            (id_edge.second)->print();
        }
    }
    bool modify_edge(int id,double length)
        {
            if(Roads.find(id) == Roads.end())
        {
            return false;
        }
            Roads[id]->length=length;
            return true;
        }
    double measure(std::string mode,int id)
    {
        if(mode=="distance")
        {
            return Roads[id]->length;
        }
        if(mode=="time")
        {
            return Roads[id]->average_time;
        }
        
    }
    bool shortest_path(std::string mode,int src,int target,double& output,std::vector<int>&path,std::vector<int> forbidden_nodes={},std::vector<std::string> forbidden_road_types={})
    {
        std::priority_queue<std::pair<double,std::pair<int,int>>>pq;
        std::vector<bool>presence(nodes);
        std::vector<bool>isforbidden(nodes);
        std::vector<double>measure(nodes);
        for(int id:forbidden_nodes)
        {isforbidden[id]=true;
        }
        pq.push({0,{src,-1}});std::map<int,int>travelid;
        while(!pq.empty())
        {
            std::pair<double,std::pair<int,int>> curr=pq.top();pq.pop();
            if(presence[curr.second.first]){continue;}
            presence[curr.second.first]=true;
            measure[curr.second.first]=-curr.first;
            travelid[curr.second.first]=curr.second.second;
            
            if(curr.second.first==target){break;}
            for(auto nodeedges:Network[curr.second.first])
            {   if(presence[nodeedges.first] || isforbidden[nodeedges.first]) continue;
                auto road_list=nodeedges.second;
                for(auto edge: road_list)
                {
                    if( find(forbidden_road_types.begin(),forbidden_road_types.end(),edge->road_type)==forbidden_road_types.end() )
                    {
                        pq.push({-edge->length-measure[curr.second.first],{nodeedges.first,edge->id}});
                        
                    }
                }
            }
        }
        if(presence[target]==false || isforbidden[target]==true){return false;}
        output=measure[target];
        path.push_back(target);
        while(target!=src)
        {
            target=Roads[travelid[target]]->u+Roads[travelid[target]]->v-target;
            path.push_back(target);

        }
        reverse(path.begin(),path.end());

        return true;
    }
    
};