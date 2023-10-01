#include <iostream>

using namespace std;
#define DISG_MAX 10

class DisjNode{
public:
    int data, idx, parent_idx, num_child;
    DisjNode(){
        data = -1;
        idx = -1;
        parent_idx = -1;
        num_child = 0;
    }
    DisjNode(int d,int i = -1,int p = -1){
        data = d;
        idx = i;
        parent_idx = p;
        num_child = 0;
    }
};

class DisjSet{
public:
    int num_nodes;
    DisjNode all_nodes[DISG_MAX];
    DisjSet(){
        num_nodes = 0;
    }
    void insert(int d){
        if (num_nodes == DISG_MAX)
        {
            cout<<"full set"<<endl;
            return;
        }
        all_nodes[num_nodes] = DisjNode(d,num_nodes,num_nodes );
        num_nodes++;
    }
    int find(DisjNode child){
        //int par_idx = parent[child.idx];
        int par_idx = child.parent_idx;
        if (par_idx != child.idx)
        {
            return find(all_nodes[par_idx]);
        }else{
            return par_idx;
        }
    }
    void union_sets(int i, int j){
        int i_rep = find(all_nodes[i]);
        int j_rep = find(all_nodes[j]);
        if (i_rep == j_rep)
        {
            cout<<"Both already in the same set"<<endl;
            return;
        }
        DisjNode& irep_node = all_nodes[i_rep];
        DisjNode& jrep_node = all_nodes[j_rep];
        if(irep_node.num_child > jrep_node.num_child){
            jrep_node.parent_idx = irep_node.idx;
            irep_node.num_child += jrep_node.num_child;
        }else{
            irep_node.parent_idx = jrep_node.idx;
            jrep_node.num_child += irep_node.num_child;
        }
    }
    
};

int main(int argc, char const *argv[])
{
    DisjSet ds;
    ds.insert(0);
    ds.insert(1);
    ds.insert(2);
    ds.insert(3);
    ds.insert(4);
    ds.insert(5);
    cout<<"=============================="<<endl;
    cout<<"=============================="<<endl;
    ds.union_sets(0,1);
    ds.union_sets(2,3);
    ds.union_sets(4,5);
    cout<<"=============================="<<endl;
    cout<<"=============================="<<endl;
    ds.union_sets(2,3);
    ds.union_sets(0,1);
    return 0;
}
