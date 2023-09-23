#include <iostream>
#include <memory>
#include <stdexcept>

using namespace std;

//Generic vector which uses a backing array for storing element
template <typename T>
class GenericVector
{
private:
    int numEntries;
    int curSize;
    T* backingArray;
public:
    GenericVector();//create a vector with 0 space
    GenericVector(initializer_list<T> init);//creates a vector with elements in the init
    GenericVector(int initial_num);//creates a vector with a space for holding initial_num elements
    void insert_first(T elem);
    void insert_last(T elem);
    T pop_first();
    T pop_last();
    bool empty();
    int capacity();
    int size();
    void resizeUp();
    void resizeDown();
    T getE(int idx);
    ~GenericVector();
};

template <typename T>
ostream& operator<<(ostream& os, GenericVector<T>& v){
    for (size_t i = 0; i < v.size(); i++)
    {
        os<<v.getE(i)<<" ";
    }
    os<<endl;
    return os;
}

template <typename T>
GenericVector<T>::GenericVector()
{
    curSize = 10;
    numEntries = 0;
    backingArray = new T[curSize];
}

//creates a vector with elements in the init
template <typename T>
GenericVector<T>::GenericVector(initializer_list<T> init){
    cout<<"[CONSTRUCTOR] initializer_list"<<endl;
    curSize = init.size();
    numEntries = 0;
    backingArray = new T[curSize];
    for(auto& i : init){
        backingArray[numEntries++] = i;
    }
}

//creates a vector with a space for holding initial_num elements
template <typename T>
GenericVector<T>::GenericVector(int initial_num){
    cout<<"[CONSTRUCTOR] initial_num"<<endl;
    curSize = initial_num;
    numEntries = 0;
    backingArray = new T[initial_num];
}
template <typename T>
void GenericVector<T>::insert_first(T elem){
    if (numEntries == curSize)
    {
        resizeUp();
    }
    for (size_t i = numEntries; i > 0; i--)
    {
        backingArray[i] = backingArray[i-1];
    }
    backingArray[0] = elem;
    numEntries++;
}
template <typename T>
void GenericVector<T>::insert_last(T elem){
    if (numEntries == curSize)
    {
        resizeUp();
    }
    backingArray[numEntries++] = elem;
}

template <typename T>
T GenericVector<T>::pop_first(){
    if (empty())
    {
        cout<<"Empty Vector"<<endl;
    }
    T tmp = backingArray[0];
    for (size_t i = 1; i < numEntries; i++)
    {
        backingArray[i-1] = backingArray[i];
    }
    numEntries--;
    if (numEntries < (curSize/4))
    {
        resizeDown();
    }
    return tmp;
}
template <typename T>
T GenericVector<T>::pop_last(){
    if (empty())
    {
        cout<<"Empty Vector"<<endl;
    }
    T tmp = backingArray[--numEntries];
    if (numEntries < (curSize/4))
    {
        resizeDown();
    }
    return tmp;
}
template <typename T>
bool GenericVector<T>::empty(){
    return numEntries == 0;
}
template <typename T>
int GenericVector<T>::capacity(){
    return curSize;
}
template <typename T>
int GenericVector<T>::size(){
    return numEntries;
}
template <typename T>
void GenericVector<T>::resizeUp(){
    int newSize = curSize * 2;
    T* newArr = new T[newSize];
    for (size_t i = 0; i < numEntries; i++)
    {
        newArr[i] = backingArray[i];
    }
    /*//when backingArr = newArr: the old array will be deleted
    backingArray = newArr;
    //when newArr.reset() the newArr automatic variable will now point to nothjing
    newArr.reset();*/
    delete[] backingArray;
    backingArray = newArr;
    newArr = nullptr;
}
template <typename T>
void GenericVector<T>::resizeDown(){
    int newSize = curSize/2;
    T* newArr = new T[newSize];
    for (size_t i = 0; i < numEntries; i++)
    {
        newArr[i] = backingArray[i];
    }
    /*//when backingArr = newArr: the old array will be deleted
    backingArray = newArr;
    //when newArr.reset() the newArr automatic variable will now point to nothjing
    newArr.reset();*/
    
    delete[] backingArray;
    backingArray = newArr;
    newArr = nullptr;
}
template <typename T>
T GenericVector<T>::getE(int idx){
    if (idx < 0 || idx >= numEntries)
    {
        throw out_of_range("Accessing out of range element");
    }
    return backingArray[idx];
}

/*=========================================================================================*/
/*=========================================================================================*/
/*=========================================================================================*/

template <typename T>
GenericVector<T>::~GenericVector()
{
    delete[] backingArray;
    cout<<"Deleted Backing Array"<<endl;
}

/*
class Test
{
public:
    int data;
    Test(initializer_list<int> init){
        cout<<"Initializer list Constructor"<<endl;
        for(auto& i : init){
            cout<<i<<" ";
        }
        cout<<endl;
        data = 8;
    }
    Test(int a1, int a2){
        cout<<"2-element Constructor"<<endl;
        data = 8;
    }
    ~Test(){
        cout<<"Test Destructor"<<endl;
    }
};

template <typename T>
void test_Test(T e, T e2, T e3){
    shared_ptr<T[]> sp = make_shared<T[]>(3);
    sp[0] = e;
    sp[1] = e2;
    sp[2] = e3;

    for (size_t i = 0; i < 3; i++)
    {
        cout<<sp[i]<<" ";
    }
    cout<<endl;
}*/
/*
void test_shared(){
    shared_ptr<int[]> sp(new int[3]);
    sp[0] = 1;
    sp[1] = 2;
    sp[2] = 3;

    cout<<sp[0]<<" ";
    cout<<sp[1]<<" ";
    cout<<sp[2]<<" ";
    cout<<endl;
}*/

/*=========================================================================================*/
/*=========================================================================================*/
/*                                      LIST ITERATOR                                       */
/*=========================================================================================*/
/*=========================================================================================*/
/*
class Iterator { // an iterator for the list
public:
Elem& operator*(); // reference to the element
bool operator==(const Iterator& p) const; // compare positions
bool operator!=(const Iterator& p) const;
Iterator& operator++(); // move to next position
Iterator& operator−−(); // move to previous position
friend class NodeList; // give NodeList access
private:
Node* v; // pointer to the node
Iterator(Node* u); // create from node
};
*/

template <typename T, typename U>
class DoubleLinkedList
{
private:
    T* head;
    T* tail;
    int numNodes;
public:
    class Iterator { // an iterator for the list
    public:
        // reference to the element
        /*T* operator*(){
            return v;
        }*/
         // compare positions
        bool operator==(Iterator& p) const{
            return v == p.getV();
        }
        T* getV(){
            return v;
        }
        bool operator!=(Iterator& p) const{
            return v != p.getV();
        }
        // move to next position
        void operator++(){
            v = v->getNext();
            //return *this;
        }
         // move to previous position
        void operator--(){
            v = v->getPrev();
            //return *this;
        }
        friend class DoubleLinkedList;
    private:
        T* v; // pointer to the node
         // create from node
        Iterator(T* u){
            v = u;
        }
    };

    DoubleLinkedList(){
        head = new T(-1);
        tail = new T(-1);
        head->setNext(tail);
        head->setPrev(nullptr);
        tail->setPrev(tail);
        tail->setNext(nullptr);
        numNodes  =0;
    }
    Iterator& begin(){
        Iterator it(head);
        return it;
    }
    Iterator& end(){
        Iterator it(tail);
        return it;
    }
    void insertLast(U value){
        T* newNode = new T(value);
        newNode->setPrev(tail->getPrev());
        tail->getPrev()->setNext(newNode);
        newNode->setNext(tail);
        tail->setPrev(newNode);
        numNodes++;
    }
    void removeLast(){
        if (numNodes > 0)
        {
            T* tmp = tail->getPrev();
            tmp->getPrev()->setNext(tail);
            tail->setPrev(tmp->getPrev());
            delete tmp;
            numNodes--;
        }else{
            cout<<"Empty List!"<<endl;
        }
    }
    void insertFirst(U value){
        T* newNode = new T(value);
        newNode->setNext(head->getNext());
        newNode->setPrev(head);
        head->setNext(newNode);
        newNode->getNext()->setPrev(newNode);
        numNodes++;
    }
    void removeFirst(){
        if (numNodes > 0)
        {
            T* tmp = head->getNext();
            head->setNext(tmp->getNext());
            tmp->getNext()->setPrev(head);
            delete tmp;
            numNodes--;
        }else{
            cout<<"Empty List!"<<endl;
        }
    }
    void print_list(){
        Iterator& it = begin();
        for (it; it != end(); ++it)
        {
            cout<<*(it.getV())<<endl;
        }
    }
    int getNumNodes(){
        return numNodes;
    }
};

template <typename U>
class DLLNode
{
private:
    U data;
    DLLNode* next;
    DLLNode* prev;
public:
    DLLNode(U d, DLLNode* n = nullptr, DLLNode* p = nullptr ){
        data = d;
        next = n;
        prev = p;
    }
    U getData(){
        return data;
    }
    DLLNode* getNext(){
        return next;
    }
    void setNext(DLLNode* n){
        next = n;
    }
    DLLNode* getPrev(){
        return prev;
    }
    void setPrev(DLLNode* p){
        prev = p;
    }
    bool operator==(const DLLNode& node){
        return (data == node->getData());
    }
    bool operator!=(const DLLNode& node){
        return (data != node->getData());
    }
};

template <typename U>
ostream& operator<<(ostream& os, DLLNode<U>& node){
    os<<node.getData();
    return os;
}

void test_iters(){
    cout<<"Starting..."<<endl;
    DoubleLinkedList< DLLNode<int>, int> dll;
    dll.insertLast(3);
    dll.insertLast(2);
    dll.insertLast(1);
    dll.print_list();
}

int main(int argc, char const *argv[])
{
    test_iters();
    return 0;
}
