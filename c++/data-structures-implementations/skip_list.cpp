#include "skip_list.h"

using namespace std;

template <typename T>
Node<T>::Node(T d, int l)
{
    data = d;
    level = l;
    next_all_levels = new Node<T>*[level];
    //prevent next_all_levels to be dangling pointers
    for (size_t i = 0; i < level; i++)
    {
        next_all_levels[i] = nullptr;
    }
}
template <typename T>
Node<T>::Node(){
    data = numeric_limits<int>::min();
    level = max_level;
    next_all_levels = new Node<T>*[level];
    //prevent next_all_levels to be dangling pointers
    for (size_t i = 0; i < level; i++)
    {
        next_all_levels[i] = nullptr;
    }
}
template <typename T>
T Node<T>::get_data(){
    return data;
}
template <typename T>
int Node<T>::get_level(){
    return level;
}
template <typename T>
void Node<T>::set_level(int l){
    level = l;
}
template <typename T>
Node<T>::~Node()
{
    cout<<"deleting node "<<data<<endl;
    delete[] next_all_levels;
}
/*
skip_list();
    void get_num_nodes();
    void insert(T element);
    bool find(T element);
    ~skip_list();

*/
template <typename T>//T is numeric type
skip_list<T>::skip_list(){
    num_nodes = 0;
    //makes an array of pointers to nodes each with data = 0
    header = new Node<T>();
}
template <typename T>
float skip_list<T>::get_random_level(){
        default_random_engine gen;
        uniform_real_distribution<double> distribution(0.0,1.0);
        return distribution(gen);
    }
template <typename T>
void skip_list<T>::insert(T element){
    cout<<"iserting "<<element<<endl;
    int r_level = 1;
    float f;
    while ((f = get_random_level()) < 0.5 && r_level < max_level)
    {
        r_level++;
    }
    //now the new node will be initialized with this r_level
    Node<T>* new_node = new Node<T>(element, r_level);

    //fill the next_all_levels array
    Node<T>** tmp = new Node<T>*[r_level];
    Node<T>* cur = header;
    for (int i = (r_level-1); i >= 0; i--)
    {
        while((cur->next_all_levels[i] != nullptr) && cur->next_all_levels[i]->get_data() < element){
            cur = cur->next_all_levels[i];
        }
        tmp[i] = cur;
        if(i == 0)
            break;
    }
    //update next_all_levels for the new_node
    for (int i = 0; i < r_level; i++)
    {
        new_node->next_all_levels[i] = tmp[i]->next_all_levels[i];
        tmp[i]->next_all_levels[i] = new_node;
    }
    num_nodes++;
}
template <typename T>
int skip_list<T>::get_num_nodes(){
    return num_nodes;
}
template <typename T>
void skip_list<T>::print_list(){
    Node<T>* cur = header;
    if(cur->next_all_levels[0] == nullptr){
        cout<<"empty list"<<endl;
        return;
    }
    cur = cur->next_all_levels[0];
    while (cur != nullptr)
    {
        cout<<cur->get_data()<<" ";
        cur = cur->next_all_levels[0];
    }
    cout<<endl;
}
template <typename T>
bool skip_list<T>::find(T element, int& lvl){
    Node<T>* cur = header;
    while (cur != nullptr && cur->get_data() != element)
    {
        cur = cur->next_all_levels[0];
    }
    if(cur != nullptr){
        lvl = cur->get_level();
        return true;
    }
    lvl = -1;
    return false;
}
template <typename T>
void skip_list<T>::remove(T element){
    int lvl;
    bool found;
    if(!(found = find(element, lvl))){
        cout<<"Not existing Element"<<endl;
        return;
    }
    //first get the nodes existing right before element node in every level
    Node<T>** tmp = new Node<T>*[lvl];
    for(int i = 0; i < lvl; i++)
        tmp[i] = nullptr; //to not be a dangling pointer
    Node<T>* cur = header;
    Node<T>* to_remove;//The desired node to be removed

    for (int i = (lvl-1); i >= 0; i--)
    {
        while((cur->next_all_levels[i] != nullptr) && cur->next_all_levels[i]->get_data() < element){
            cur = cur->next_all_levels[i];
        }
        tmp[i] = cur;
        if(i == 0)
            break;
    }

    //then remove connections between tmp nodes and the removed node
    for(int i = 0; i < lvl; i++){
        tmp[i]->next_all_levels[i] = tmp[i]->next_all_levels[i]->next_all_levels[i];
    }
    num_nodes--;
}
template <typename T>
skip_list<T>::~skip_list(){
    //delete all lists for all nodes
    for(int i = 0;i < num_nodes; i++){
        Node<T>* tmp = header;
        
        for(int j = 0; j < i; j++){
            tmp = tmp->next_all_levels[0];
        }
        delete[] &tmp;
    }    
}
int main(int argc, char const *argv[])
{
    skip_list<int> sl;
    sl.print_list();
    sl.insert(5);
    sl.insert(2);
    sl.insert(7);
    sl.insert(11);
    sl.insert(-1);
    sl.insert(-8);
    sl.print_list();
    sl.remove(2);
    sl.remove(5);
    sl.remove(11);
    sl.remove(7);
    sl.remove(-1);
    sl.remove(-8);
    sl.remove(-9);
    sl.print_list();
    return 0;
}
