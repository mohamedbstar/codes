#include <iostream>
#include <cstdio>
#include <ostream>
#include <cstring>
#include <vector>

using namespace std;

template <typename U>
class StackNode
{
private:
    int data;
public:
    StackNode(int d=0){
        data = d;
    }
    U getData(){
        return data;
    }
    void setData(int d){
        data = d;
    }
    bool operator==(const StackNode& node){
        return data == node.getData();
    }
    bool operator!=(const StackNode& node){
        return data != node.getData();
    }
    bool operator>(const StackNode& node){
        return data > node.getData();
    }
    bool operator<(const StackNode& node){
        return data < node.getData();
    }
};

//provide a printing mechanism for StackNode
template <typename U>
ostream& operator<<(ostream& os, StackNode<U>& node){
    os<<node.getData();
    return os;
}

template <typename T, typename U>
class Stack
{
private:
    T* entries;
    int size;
    int max_size;
public:
    Stack(int init_size = 3){
        entries = new T[init_size];
        size = 0;
        max_size = init_size;
    }
    void push(U value){
        if (size == max_size)
        {
            /* resize the entries array */
            resize(max_size * 2);
        }
        T new_entry(value);
        entries[size++] = new_entry;
    }
    void resize(int new_size){
        T* new_entries = new T[new_size];
        
        //copy elements
        for (size_t i = 0; i < new_size / 2; i++)
        {
            new_entries[i] = entries[i];
        }
        
        delete[] entries;

        entries = new_entries;
        max_size = new_size;
    }
    T pop(){
        if (empty())
        {
            cout<<"Empty Stack, Can't pop"<<endl;
            return 0;
        }
        T to_ret = entries[--size];
        return to_ret;
    }
    T* top(){
        if (empty())
        {
            cout<<"Empty Stack, Can't top"<<endl;
            return 0;
        }
        return entries[size-1];
    }
    int getSize(){
        return size;
    }
    int getMaxSize(){
        return max_size;
    }
    bool empty(){
        return size == 0;
    }
    void printStack(){
        for (size_t i = 0; i < size; i++)
        {
            cout<<entries[i]<<" ";
        }
        cout<<endl;
    }
    ~Stack(){
        delete[] entries;
    }
};

template <typename E>
class StackError
{
private:
    E message;
public:
    StackError(E m);
    void what();
};

template <typename E>
StackError<E>::StackError(E m)
{
    message = m;
}
template <typename E>
void StackError<E>::what(){
    cout<<message<<endl;
}

void reverse_vector(vector<int> v){
    Stack< StackNode<int> , int > stack(v.size());
    for (size_t i = 0; i < v.size(); i++)
    {
        stack.push(v[i]);
    }
    for (size_t i = 0; i < v.size(); i++)
    {
        v[i] = stack.pop().getData();
    }
    for (size_t i = 0; i < v.size(); i++)
    {
        cout<<v[i]<<" ";
    }
    cout<<endl;
}
/*
void match_parnethesis(string txt){
    string left = "{[(";
    string right = "}])";
    int txt_size = txt.size();
    Stack< StackNode<char>,char > stack;
    bool matching = true;
    char c;
    if (txt_size == 0)
    {
        return;
    }
    int counter = 0;
    for (size_t i = 0; i < txt_size; i++)
    {
        c = txt[i];
        //if character in left=> add it to the stack
        if(left.find(c) != -1){
            counter++;
            stack.push(c);
        //if character in right=> pop stack and see if matching positions or not
        }else if(right.find(c) != -1){
            if(stack.empty())
                matching = false;
                cout<<"fasle because stack empty!"<<endl;
                break;
            int c_idx = right.find(c);
            char c_left = stack.pop().getData();
            int c_left_idx = left.find(c_left);
            if (c_left_idx != right.find(c))
            {
                cout<<"false as "<<c<<" (idx in txt: "<<c_idx<<") in right != "<<c_left<<" in left"<<" (idx in txt: "<<c_left_idx<<endl;
                matching = false;
                break;
            }else{
                cout<<c_idx<<" is the same as "<<c_left_idx<<endl;
            }
        }
    }
    if (matching)
    {
        cout<<"Matched txt"<<endl;
    }else{
        cout<<"Not Well Formatted!"<<endl;
    }
    cout<<"counter: "<<counter<<endl;
}
*/

//T is the type of Node and U is the data value of the node
template <typename T, typename U>
class ArrayQueue
{
private:
    T* entries;
    int size;
    int max_size;
public:
    ArrayQueue(int init_size = 3);
    ~ArrayQueue();
    void enqueue(U value);
    void dequeue();
    void resize(int new_size);
    T next_element();
    bool empty();
    int getSize();
    int getMaxSize();
    void printQueue();
};

template <typename T, typename U>
ArrayQueue<T,U>::ArrayQueue(int init_size)
{
    max_size = init_size;
    size = 0;
    entries = new T[init_size];
}
template <typename T , typename U>
void ArrayQueue<T,U>::resize(int new_size){
    T* new_entries = new T[new_size];
    for (size_t i = 0; i < new_size/2; i++)
    {
        new_entries[i] = entries[i];
    }
    delete[] entries;
    entries = new_entries;
    max_size = new_size;
}
template <typename T , typename U>
void ArrayQueue<T,U>::enqueue(U value){
    if (size == max_size)
    {
        resize(max_size * 2);
    }
    T new_node(value);
    entries[size++] = new_node;
}
template <typename T , typename U>
void ArrayQueue<T,U>::dequeue(){
    if(empty()){
        cout<<"Empty Queue!, Can't dequeue()"<<endl;
        return;
    }
    //remove element from the position 0
    for (size_t i = 0; i < size - 1; i++)
    {
        entries[i] = entries[i+1];
    }
    size--; //implenment a facitity to destruct the abandoned object
}

template <typename T , typename U>
T ArrayQueue<T,U>::next_element(){
    if (empty())
    {
        cout<<"Empty Queue! no Next Element!!"<<endl;
        return 0;
    }
    return entries[0];
}
template <typename T , typename U>
void ArrayQueue<T,U>::printQueue(){
    for (size_t i = 0; i < size; i++)
    {
        cout<<entries[i]<<" ";
    }
    cout<<endl;
}

template <typename T , typename U>
ArrayQueue<T,U>::~ArrayQueue()
{
    delete[] entries;
}

template <typename T , typename U>
int ArrayQueue<T,U>::getSize(){
    return size;
}

template <typename T , typename U>
int ArrayQueue<T,U>::getMaxSize(){
    return max_size;
}

template <typename T , typename U>
bool ArrayQueue<T,U>::empty(){
    return size == 0;
}

template <typename U>
class QueueNode
{
private:
    U data;
    QueueNode* next;
public:
    QueueNode(U d = 0){
        data = d;
        next = nullptr;
    }
    U getData(){
        return data;
    }
    void setData(U d){
        data = d;
    }
    QueueNode* getNext(){
        return next;
    }
    void setNext(QueueNode* n){
        next = n;
    }
    ~QueueNode(){
        cout<<"Deleting Node: "<<data<<endl;
    }
};

template <typename U>
ostream& operator<<(ostream& os, QueueNode<U>& node){
    os<<node.getData();
    return os;
}

//T type must have: setNext(), getNext() methods as well as a U type for its own data
template <typename T, typename U>
class CircularListQueue
{
private:
    int numNodes;
    T* cursor;
public:
    CircularListQueue();
    ~CircularListQueue();
    void enqueue(U value); //added node is the cursor
    T* dequeue(); //delete the node following the cursor
    T* getCursor();
    T* getNext();
    void removeNodes();
    void remove(T* node);
    void printQueue();
};

template <typename T, typename U>
CircularListQueue<T,U>::CircularListQueue()
{
    numNodes = 0;
    cursor = nullptr;
}
template <typename T, typename U>
void CircularListQueue<T,U>::enqueue(U value){
    T* new_node = new T(value);
    if (cursor == nullptr)
    {
        cursor = new_node;
        cursor->setNext(new_node);
    }else{
        new_node->setNext(cursor->getNext());
        cursor->setNext(new_node);
        cursor = new_node;
    }
    numNodes++;
}
template <typename T, typename U>
T* CircularListQueue<T,U>::dequeue(){
    T* to_rem;
    if (cursor == nullptr)
    {
        cout<<"Empty Queue"<<endl;
        return nullptr;
    }else if (numNodes == 1)
    {
        to_rem = cursor;
        cursor = nullptr;
    }else{
        to_rem = cursor->getNext();
        cursor->setNext(cursor->getNext()->getNext());    
    }
    numNodes--;
    return to_rem;
}
/*
  T* getCursor();
    T* getNext();
    void removeNodes();
*/
template <typename T, typename U>
T* CircularListQueue<T,U>::getCursor(){
    return cursor;
}
template <typename T, typename U>
T* CircularListQueue<T,U>::getNext(){
    return cursor->getNext();
}
template <typename T, typename U>
void CircularListQueue<T,U>::remove(T* node){
    if (node == nullptr)
    {
        return;
    }
    T* nex_node = node->getNext();
    cout<<"deleting node: "<<node->getData()<<endl;
    delete node;
    node = nullptr;
    remove(nex_node);
}
template <typename T, typename U>
void CircularListQueue<T,U>::removeNodes(){
    remove(cursor);
}
template <typename T, typename U>
void CircularListQueue<T,U>::printQueue(){
    cout<<*cursor<<" ";
    T* cur = cursor->getNext();
    while (cur != cursor)
    {
        cout<<*cur<<" ";
        cur = cur->getNext();
    }
    cout<<endl;
}
template <typename T, typename U>
CircularListQueue<T,U>::~CircularListQueue()
{
    /*if (cursor != nullptr)
    {
        T* cur = cursor;
        T* tmp;
        while (cur != nullptr &&cur->getNext() != nullptr)
        {
            tmp = cur->getNext();
            cur->setNext(cur->getNext());
            delete tmp;
        }
    }*/
    if (cursor != nullptr)
    {
        T* cur = cursor;
        T* tmp;
        for (size_t i = 0; i < numNodes; i++)
        {
            tmp = cur->getNext();
            delete cur;
            cur = tmp;
        }
    }
    
    
    //removeNodes();
}

int main(int argc, char const *argv[])
{
    
    return 0;
}
