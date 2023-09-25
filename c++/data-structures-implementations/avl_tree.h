#include <iostream>

template <typename T>
class avl_node
{
public:
    T data;
    int height;
    avl_node<T>* left;
    avl_node<T>* right;
    avl_node(T d);
    avl_node<T>* find_predecessor();
    avl_node<T>* find_successor();
    avl_node<T>* right_rotate();
    avl_node<T>* left_rotate();
    int get_height();

};

template <typename T>
class avl_tree{
public:
    avl_tree();
    int num_nodes;
    avl_node<T>* root;
    bool find(avl_node<T>* cur, T element);
    void insert(avl_node<T>* cur ,T element);
    void remove(avl_node<T>* cur ,T element);
    void inorder(avl_node<T>* cur);
private:
    avl_node<T>* insert_util(avl_node<T>* cur, T element);
    avl_node<T>* remove_util(avl_node<T>* cur, T element);
};