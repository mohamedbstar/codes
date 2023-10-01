#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <queue>
#include <limits>
using namespace std;
class Edge;
enum direc {DIRECTED_GRAPH, UNDIRECTED_GRAPH};

class Comparator{
public:
    bool operator()(pair<int,int>& p1, pair<int,int>& p2){
        return p1.second < p2.second;
    }
};

class GraphNode
{
public:
    int data, idx;
    bool visited;
    vector<Edge*> out_edges;//assuming undirected-unwheighted graphs
    GraphNode(int d){
        data = d;
        visited = false;
    }
    /*bool operator<(GraphNode* another){
        return data < another->data;
    }*/
};

class Edge{
public:
    int weight;
    GraphNode *src, *dst;
    Edge(int w, GraphNode* s, GraphNode* d){
        weight = w;
        src = s;
        dst = d;
    }
};
template <enum direc di>
class Graph
{
public:

    list<GraphNode*> storage;
    vector<GraphNode*> nodes;
    vector<Edge*> edges;
    vector<bool> visited;
    list<GraphNode*> top_order;
    ~Graph(){
        int s = nodes.size();
        for(int i =0; i< s; i++){
            delete nodes[i];
        }
        s = edges.size();
        for (size_t i = 0; i < s; i++)
        {
            delete edges[i];
        }
    }
    void insert_node(int value){
        GraphNode* new_node = new GraphNode(value);
        int idx = nodes.size();
        nodes.push_back(new_node);
        visited.resize(nodes.size());
        new_node->idx = idx;
    }
    void insert_edge(int s, int d, int weight=0){
        auto src = find_if(nodes.begin(), nodes.end(), [s](GraphNode* n){
            return n->data == s;
        });
        if (src == nodes.end())
        {
            cout<<"No src node"<<endl;
            return;
        }
        
        auto dst = find_if(nodes.begin(), nodes.end(), [d](GraphNode* n){
            return n->data == d;
        });
        if (dst == nodes.end())
        {
            cout<<"No dst node"<<endl;
            return;
        }
        
        Edge* e = new Edge(weight, *src, *dst);
        edges.push_back(e);
        (*src)->out_edges.push_back(e);
        if(di == UNDIRECTED_GRAPH){
            Edge* ae = new Edge(weight, *dst, *src);
            (*dst)->out_edges.push_back(ae);
        }
    }
    void unvisit_all_nodes(){
        for_each(nodes.begin(), nodes.end(), [](GraphNode* n){
            n->visited = false;
        });
    }
    void bfs(){
        //set all nodes to be not visited at first
        unvisit_all_nodes();

        storage.push_back(nodes[0]);
        GraphNode* cur;
        while (storage.size() > 0)
        {
            //pop the current node
            cur = storage.front();
            storage.pop_front();
            if (cur->visited == true)
            {
                continue;
            }
            cur->visited = true;
            //cout<<cur->data<<" ";
            for(auto edge : cur->out_edges){
                if(edge->dst->visited)
                    continue;
                storage.push_back(edge->dst);
            }
        }
        cout<<endl;
        storage.clear();//reset storage list for other functions
    }

    void dfs(){
        
        auto cur = storage.back();
        storage.pop_back();
        cur->visited = true;
        //cout<<cur->data;
        for (auto e : cur->out_edges)
        {
            if (e->dst->visited)
            {
                continue;
            }
            storage.push_back(e->dst);
            dfs();
        }
        top_order.push_front(cur);
    }

    void label_components(){
        int label = 0;
        for(auto node : nodes){
            if (node->visited == false)
            {
                storage.push_back(node);
                dfs();
            }
            label++;//it indicates component number abd could indicate that a particular node is in a particular component
                    // by giviing label as an argument for dfs function
            
        }
    }
    void top_sort(){
        for (size_t i = 0; i < nodes.size(); i++)
        {
            if (!nodes[i]->visited)
            {
                storage.push_back(nodes[i]);
                dfs();
            }
        }
        top_order.reverse();
    }

    void dkijestra(){
        unvisit_all_nodes();
        priority_queue< pair<int,int>, vector<pair<int,int>>, Comparator > pq;
        
        vector<int> dist(nodes.size());
        transform(dist.begin(), dist.end(), dist.begin(), [](int& i){
            i = numeric_limits<int>::max();
            return i;
        });

        pq.push({0,0});
        dist[0] = 0;

        while (pq.size() > 0)
        {
            pair<int,int> p = pq.top();
            pq.pop();
            int idx = p.first;
            cout<<"cur_idx: "<<idx<<endl;
            cout<<"cur_dist: "<<p.second<<endl;
            nodes[idx]->visited = true;

            for(auto edge : nodes[idx]->out_edges){
                auto node = edge->dst;
                
                /*if(node->visited == true){
                    continue;
                }*/

                int new_dist = dist[idx] + edge->weight;
                if (new_dist < dist[node->idx])
                {
                    dist[node->idx] = new_dist;
                    pq.push({node->idx , new_dist});
                }
            }
        }
        //printing out the distance
        for(int i = 0; i < dist.size(); i++){
            cout<<"distance of "<<i+1<<" = "<<dist[i]<<endl;
        }
    }

    void dfs(int root ,int at, int parent, vector<Edge*>& bridges, int& id, vector<int>& ids, vector<int>& low_link, int& numOutEdges){
        nodes[at]->visited = true;
        low_link[at] = id;
        ids[at] = id;
        id++;
        if(root == parent)
            numOutEdges++;
        for(auto edge : nodes[at]->out_edges){
            int to = edge->dst->idx;
            if(to == parent)
                continue;
            if(!nodes[to]->visited){
                dfs(root ,to, at, bridges, id, ids, low_link, numOutEdges);
                //check if low[at] changes by this dfs or not
                low_link[at] = min(low_link[at], low_link[to]);
                if(ids[at] < low_link[to]){
                    //found a bridge
                    Edge* e = new Edge(0,nodes[at], nodes[to]);
                    bridges.push_back(e);
                    //add this node to articulation points
                }
                if(ids[at] == low_link[to]){
                    //add at to articulatin points
                }
            }else{
                low_link[at] = min(low_link[at], low_link[to]);
            }
        }
    }
    void find_bridges_and_art_points(){
        unvisit_all_nodes();
        vector<Edge*> bridges;
        int id = 0;
        vector<int> low_link(nodes.size());
        vector<int> ids(nodes.size());

        for (size_t i = 0; i < nodes.size(); i++)
        {
            if(!nodes[i]->visited){
                int numOutEdges = 0;
                dfs(i,i,-1,bridges, id, ids, low_link, numOutEdges);
                //if numOutEdges > 1-> add i to articulation points
            }
        }
        cout<<"Bridges are: "<<endl;
        for(int i = 0; i < bridges.size(); i++){
            cout<<"from "<<bridges[i]->src->data<<" to "<<bridges[i]->dst->data<<endl;
        }
    }
};


int main(int argc, char const *argv[])
{
    Graph<DIRECTED_GRAPH> g;
    g.insert_node(1);
    g.insert_node(2);
    g.insert_node(3);
    g.insert_node(4);
    g.insert_node(5);

    g.insert_edge(1,2, 2);
    g.insert_edge(2,3, 3);
    g.insert_edge(3,4, 3);
    g.insert_edge(4,1, 2);
    g.insert_edge(3,5, 2);

    g.find_bridges_and_art_points();
    return 0;
}
