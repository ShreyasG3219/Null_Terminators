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
    bool isblocked=false;
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
    std::vector<Node*>All_Nodes;
    std::map<int,std::map<int,std::vector<Edge*>>>Network;
    std::map<int,Edge*>Roads;

    public:
    Graph(int nodes)
    {
        this->nodes=nodes;
        this->All_Nodes=std::vector<Node*>(this->nodes);
    }
    void addNode(int id,double lat,double lon,std::vector<std::string> pois)
    {
        All_Nodes[id]=new Node(id,lat,lon,pois);
    }
    bool addedge(int id,int u,int v,double length,double average_time,std::vector<double>speed_profile,bool oneway,std::string road_type)
    {
        if(u>=nodes || v>=nodes || u<0 || v<0)
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
        listedges[find(listedges.begin(),listedges.end(),Roads[id])-listedges.begin()]->isblocked=true;

        return true;
    }
    void printedges()
    {
        for(auto& id_edge:Roads)
        {
            (id_edge.second)->print();
        }
    }
    bool modify_edge(int id,double length=-1,double average_time=-1,std::vector<double>speed_profile={-1})
        {
            if(Roads.find(id) == Roads.end())
        {
            return false;
        }
            Roads[id]->isblocked=false;
            if(length>=0) Roads[id]->length=length;
            if(average_time>=0) Roads[id]->average_time=average_time;
            if(speed_profile.size()==0 || speed_profile[0]!=-1) Roads[id]->speed_profile=speed_profile;
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
                {   if(edge->isblocked==true){continue;}
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