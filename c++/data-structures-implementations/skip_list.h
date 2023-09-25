#ifndef _SKIP_H
#define _SKIP_H
#include <iostream>
#include <list>
#include <vector>
#include <limits>
#include <random>
#include <bits/stdc++.h>
#include <memory>

#define max_level 20

template <typename T>
class Node
{
private:
    T data;
    int level;
public:
    Node<T>** next_all_levels;
    Node(T d, int l);
    Node();
    T get_data();
    int get_level();
    void set_level(int l);
    ~Node();
};

template <typename T>
class skip_list
{
private:
    int num_nodes;
public:
    Node<T>* header;
    skip_list();
    int get_num_nodes();
    float get_random_level();
    void insert(T element);
    void print_list();
    bool find(T element, int& lvl);
    void remove(T element);
    ~skip_list();
};

#endif