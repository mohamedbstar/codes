#include <iostream>
#include <memory>
#include <algorithm>
using namespace std;

template <typename T>
class Node{
private:
    T data;
    Node* next;
    Node* prev;
public:
    Node(T d){
        data = d;
        next = prev = nullptr;
    }
    Node* getNext(){
        return next;
    }
    Node* getPrev(){
        return prev;
    }
    void setNext(Node* n){
        next = n;
    }
    void setPrev(Node* p){
        prev = p;
    }
    T getData(){
        return data;
    }
    void setData(T d){
        data = d;
    }
};

template <typename T>
class List
{
private:
    int size;
    Node<T>* head;
public:
    List(){
        size = 0;
        head = nullptr;
    }
    class Iterator
    {
    private:
        Node<T>* curNode;
        Iterator(Node<T>* n){
            curNode = n;
        }
    public:
        friend class List;//to enable List to access private data of Iterator and prevent anything else
        Node<T>* operator*(){
            return curNode;
        }
        bool operator==(const Iterator& another){
            return curNode == another.curNode;
        }
        bool operator!=(const Iterator& another){
            return curNode != another.curNode;
        }
        void operator++(){
            curNode = curNode->getNext();
        }
        void operator--(){
            curNode = curNode->getPrev();
        }
        Node<T>* operator->(){
            return curNode;
        }
    };
    Iterator begin(){
        Iterator it(head);
        return it;
    }
    Iterator end(){
        Iterator it(nullptr);
        return it;
    }
    void insert(T d){
        Node<T>* newNode = new Node<T>(d);
        if(head == nullptr){
            head = newNode;
            size++;
            return;
        }
        Iterator it = begin();
        for (it; it->getNext() != nullptr; ++it);
        it->setNext(newNode);
        size++;
        return;
    }
    int getSize(){
        return size;
    }
    ~List(){
        cout<<"At list destructor"<<endl;
        if (head)
        {
            delete head;
        }
    }
    
    
};

template <typename T>
ostream& operator<<(ostream& os, List<T>* list){
    for(auto it = list->begin(); it != list->end(); ++it){
        os<<it->getData()<<" ";
    }
    cout<<endl;
    return os;
}

int main(int argc, char const *argv[])
{
    List<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);
    list.insert(4);
    cout<<(&list);

    return 0;
}
