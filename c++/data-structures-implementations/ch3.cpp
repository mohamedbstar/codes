#include <iostream>
#include <string>
#include <stdexcept>
#include <ostream>

using namespace std;


class GameEntry
{
private:
    int score;
    string name;
public:
    GameEntry(int s, string n);
    string getName();
    int getScore();
};

GameEntry::GameEntry(int s = 0, string n = "mohamed"){
    name = n;
    score = s;
}

string GameEntry::getName(){
    return name;
}

int GameEntry::getScore(){
    return score;
}

class Scores { // stores game high scores
public:
    Scores(int maxEnt = 10); // constructor
    ~Scores(); // destructor
    int getNumEntries();
    GameEntry* getEntries();
    void add(GameEntry& e); // add a game entry
    void add(GameEntry&& e); // add a game entry
    GameEntry remove(int i); // 
    void print_entries();
private:
    int maxEntries; // maximum number of entries
    int numEntries; // actual number of entries
    GameEntry* entries; // array of game entries
};

Scores::Scores(int maxEnt) { // constructor
    maxEntries = maxEnt; // save the max size
    entries = new GameEntry[maxEntries]; // allocate array storage
    numEntries = 0; // initially no elements
}
Scores::~Scores() { // destructor
    delete[ ] entries;
}

GameEntry* Scores::getEntries(){
    return entries;
}

void Scores::add(GameEntry&& e){
    //first get index of first element that is less than e
    int i = numEntries - 1;
    while (i >= 0 && entries[i].getScore() >= e.getScore())
    {
        i--;
    }    
    //see if entries is full or not
    if (numEntries == maxEntries)
    {
        cout<<"numEntries = maxEntries"<<endl;
        if (e.getScore() <= entries[0].getScore())
        {
            /* do nothing */
            cout<<"Less than all elements"<<endl;
        }else{
            /*always shift for right [towards 0] starting from i*/
            cout<<"e.getScore() > entries[0].getScore()"<<endl;
            int j = i;
            for (j; j > 0; j--)
            {
                entries[j - 1] = entries[j];
            }
            entries[j] = e;
        }
        
    }else{
        //always shift to the left [towards end of the array] because there is a place for a new element
        cout<<"numEntries < maxEntries"<<endl;
        int j = numEntries;
        for ( j; j > i+1; j--)
        {
            entries[j] = entries[j-1];
        }
        entries[i + 1] = e;
        numEntries++;
        print_entries();
    }
}

void Scores::add(GameEntry& e){
    //first get index of first element that is less than e
    int i = numEntries - 1;
    while (i >= 0 && entries[i].getScore() >= e.getScore())
    {
        i--;
    }    
    //see if entries is full or not
    if (numEntries == maxEntries)
    {
        cout<<"numEntries = maxEntries"<<endl;
        if (e.getScore() <= entries[0].getScore())
        {
            /* do nothing */
            cout<<"Less than all elements"<<endl;
        }else{
            /*always shift for right [towards 0] starting from i*/
            cout<<"e.getScore() > entries[0].getScore()"<<endl;
            int j = i;
            for (j; j > 0; j--)
            {
                entries[j - 1] = entries[j];
            }
            entries[j] = e;
        }
        
    }else{
        //always shift to the left [towards end of the array] because there is a place for a new element
        cout<<"numEntries < maxEntries"<<endl;
        int j = numEntries;
        for ( j; j > i+1; j--)
        {
            entries[j] = entries[j-1];
        }
        entries[i + 1] = e;
        numEntries++;
        print_entries();
    }
}

GameEntry Scores::remove(int i){
    if (i >= maxEntries || i < 0)
    {
        return static_cast<GameEntry>(0);
    }
    GameEntry to_ret = entries[i];
    int j = i;
    while (j + 1 < numEntries)
    {
        entries[j] = entries[j+1];
        j++;
    }
    numEntries--;
    print_entries();
    return to_ret;
    
}

void Scores::print_entries(){
    cout<<"entries: ";
    for (size_t i = 0; i < numEntries; i++)
    {
        cout<<entries[i].getScore()<<" ";
    }
    cout<<endl;
    
}

int Scores::getNumEntries(){
    return numEntries;
}

void test_scores(){
    Scores scores(5);
    scores.add(GameEntry(5));
    cout<<"==============="<<endl;
    scores.add(GameEntry(3));
    cout<<"==============="<<endl;
    scores.add(GameEntry(7));
    cout<<"==============="<<endl;
    scores.add(GameEntry(1));
    cout<<"==============="<<endl;
    scores.remove(1);
}

void insertion_sort(int* arr, int size){
    for (size_t i = 1; i < size; i++)
    {
        int cur = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > cur)
        {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = cur;
    }

    cout<<"Array: ";
    for (size_t i = 0; i < size; i++)
    {
        cout<<arr[i]<<" ";
    }
    cout<<endl;
}

void selection_sort(int* arr, int size){
    for (size_t i = 0; i < size; i++)
    {
        int min_idx = i;
        for (size_t j = i+1; j < size; j++)
        {
            if (arr[j] < arr[min_idx])
            {
                min_idx = j;
            }
            
        }
        //swap i and min_idx
        int tmp = arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = tmp;
    }
    cout<<"Array: ";
    for (size_t i = 0; i < size; i++)
    {
        cout<<arr[i]<<" ";
    }
    cout<<endl;
}

class SLLNode
{
private:
    int data;
    SLLNode* next;
public:
    SLLNode(int d , SLLNode* n = nullptr){
        data = d;
        next = n;
    }

    int getData(){
        return data;
    }

    SLLNode* getNext(){
        return next;
    }

    void setNext(SLLNode* n){
        next = n;
    }
};

class SingleLinkedList
{
private:
    SLLNode* head;
    int numNodes;
public:
    SingleLinkedList(SLLNode* h = nullptr){
        head = h;
        numNodes = 0;
    }

    ~SingleLinkedList(){
        remove_list();
    }

    SLLNode* getHead(){
        return head;
    }

    int getNumNodes(){
        return numNodes;
    }

    void insert(int d){
        SLLNode* node = new SLLNode(d);
        if(head == nullptr){
            head = node;
        }else{
            SLLNode* cur = head;
            while (cur->getNext() != nullptr)
            {
                cur = cur->getNext();
            }
            cur->setNext(node);
        }
        numNodes++;
    }
    void insert_first(int d){
        SLLNode* newNode = new SLLNode(d);
        if (head == nullptr)
        {
            head = newNode;
        }else{
            newNode->setNext(head);
            head = newNode;
            numNodes--;
        }
    }

    void remove_first(){
        remove(0);
    }

    void remove_last(){
        remove(numNodes - 1);
    }

    /*void remove(int d){
        int idx = find(d);
        if (idx != -1)
        {
            if (idx == 0)
            {
                //remove head
                SLLNode* tmp = head;
                head = head->getNext();
                delete tmp;
            }else{
                //tmp is prev for node to be deleted
                SLLNode* tmp = head;
                SLLNode* tmp2;
                
                for (size_t i = 0; i < idx - 1; i++)
                {
                    tmp = tmp->getNext();
                }
                tmp2 = tmp->getNext();
                tmp->setNext(tmp2->getNext());
                delete tmp2;
            }
            numNodes--;
        }
    }*/
    void remove(int idx){
        if (idx < 0 || idx >= numNodes)
        {
            cout<<"Invalid Index: "<<idx<<endl;
            return;
        }

        if (idx == 0)
            {
                //remove head
                SLLNode* tmp = head;
                head = head->getNext();
                delete tmp;
            }else{
                //tmp is prev for node to be deleted
                SLLNode* tmp = head;
                SLLNode* tmp2;
                
                for (size_t i = 0; i < idx - 1; i++)
                {
                    tmp = tmp->getNext();
                }
                tmp2 = tmp->getNext();
                tmp->setNext(tmp2->getNext());
                delete tmp2;
            }
            numNodes--;
    }
    /*void remove_list(SLLNode* cur = head){
        if (cur != nullptr)
        {
            remove_list(cur->getNext());
            delete cur;
            numNodes--;
        }
    }*/

    void remove_list(int idx = 0){
        if (idx >= numNodes)
        {
            return;
        }

        remove_list(idx + 1);
        remove(idx);
    }

    bool empty(){
        return numNodes == 0;
    }

    int getHeadData(){
        return head->getData();
    }
    
    int find(int d){
        int counter = -1;
        SLLNode* cur = head;
        while (cur)
        {
            counter++;
            if (cur->getData() == d)
            {
                return counter;
            }
            cur = cur->getNext();
        }
        cout<<"Can't find "<<d<<endl;
        return -1;
    }
    void print_list(){
        SLLNode* cur = head;
        cout<<"List: ";
        while (cur)
        {
            cout<<cur->getData()<<" ";
            cur = cur->getNext();
        }
        cout<<endl;
    }
    void reverseIteration(){

    }
    SLLNode* reverseRecursion(SLLNode* cur){
        if (cur->getNext() != nullptr)
        {
            SLLNode* tmp = reverseRecursion(cur->getNext());
            tmp->setNext(cur);
        }else{
            head = cur;
        }
        return cur;
    }
    void reverse(){
        SLLNode* oldHead = getHead();
        reverseRecursion(getHead());
        oldHead->setNext(nullptr);
    }

    SLLNode* getSecondLast(){
        //get numNodes and do getNext() 
        return nullptr;
    }

    SLLNode* getSecondLastRecursion(SLLNode* cur){
        if (cur)
        {
            if (cur->getNext())
            {
                if (!cur->getNext()->getNext())
                {
                    return cur;
                }
            }else{
                return getSecondLastRecursion(cur->getNext());
            }
        }
        return nullptr;
    }
    SLLNode* middleElement(){
        SLLNode* singleWalker = head;
        SLLNode* doubleWalker = head;
        while (doubleWalker)
        {
            if (doubleWalker->getNext() && doubleWalker->getNext()->getNext())
            {
                doubleWalker = doubleWalker->getNext()->getNext();
                singleWalker = singleWalker->getNext();
            }else{
                return singleWalker;
            }
        }
        return nullptr;
    }
};

//can't define functions outside class as i neeed to pass the type like (int) to it
//Note: T type must have the following methods:
//  1:getData() of type U
//  2:getNext() of type T*
//  3:setNext(T*)
//  4:getPrev() of type T*
//  5:setPrev(T*)
template <typename T, typename U>
class DoubleLinkedList
{
private:
    T* head;
    int numNodes;
public:
    DoubleLinkedList(T* h = nullptr){
        head = h;
        numNodes  =0;
    }

    void insertLast(U value){
        T* newNode = new T(value);
        if (head == nullptr)
        {
            head = newNode;
        }else{
            T* cur = head;
            while (cur->getNext())
            {
                cur = cur->getNext();
            }
            cur->setNext(newNode);
            newNode->setPrev(cur);
        }
        numNodes++;
    }
    void removeLast(){
        if (numNodes > 0)
        {
            T* cur = head;
            for (size_t i = 1; i < numNodes - 1; i++)
            {
                cur = cur->getNext();
            }
            
            delete cur->getNext();
            cur->setNext(nullptr);
            numNodes--;
        }else{
            cout<<"Empty List!"<<endl;
        }
        
    }
    void insertFirst(U value){
        T* newNode = new T(value);
        if (head)
        {
            head->setPrev(newNode);
        }
        newNode->setNext(head);
        head = newNode;
        numNodes++;
    }
    void removeFirst(){
        if (head)
        {
            T* tmp = head;
            head = head->getNext();
            delete tmp;
            numNodes--;
        }
    }
    void print_list(){
        T* cur = head;
        cout<<"List: ";
        while (cur)
        {
            cout<<*cur<<" ";
            cur = cur->getNext();
        }
        cout<<endl;
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

template <typename T, typename U>
class CircularLinkedList
{
private:
    T* cursor;
    int numNodes;
public:
    CircularLinkedList(){
        cursor = nullptr;
        numNodes = 0;
    }
    void insert(U data){            
        T* newNode = new T(data);
        if (numNodes == 0)
        {
            /* Make new node and set cursor to it */
            newNode->setNext(newNode);
            newNode->setPrev(newNode);
            cursor = newNode;
        }else{
            /* make newNode after cursor immediately */
            newNode->setNext(cursor->getNext());
            cursor->getNext()->setPrev(newNode);
            cursor->setNext(newNode);
            newNode->setPrev(cursor);
        }
        numNodes++;
    }
    void remove(){
        /* remove the element immediately following the cursor or the cursor itself */
        if (numNodes > 0)
        {
            if (cursor->getNext() != cursor)
            {
                /* delete the item following cursor */
                T* tmp = cursor->getNext();
                tmp->getNext()->setPrev(cursor);
                cursor->setNext(tmp->getNext());
                delete tmp;
            }else{
                /* delete cursor itself */
                delete cursor;
                cursor = nullptr;
            }
            numNodes--;
        }else{
                cout<<"Empty List!"<<endl;
        }
    }
    void printList(){
        if (!cursor)
        {
            return;
        }
        cout<<*cursor<<" ";
        T* cur = cursor->getNext();
        while (cur != cursor)
        {
            cout<<*cur<<" ";
            cur = cur->getNext();
        }
        cout<<endl;
    }
    void removeList(){
        while (cursor != nullptr)
        {
            remove();
        }
    }
    ~CircularLinkedList(){
        removeList();
    }
};


void test_dll(){
    DoubleLinkedList<DLLNode<int>, int> dll;
    dll.insertLast(5);
    dll.insertLast(101);
    dll.insertLast(1);
    dll.insertFirst(20);
    dll.print_list();
    cout<<"numNodes: "<<dll.getNumNodes()<<endl;
    dll.removeLast();
    cout<<"numNodes: "<<dll.getNumNodes()<<endl;
    dll.print_list();
}
void test_cll(){
    CircularLinkedList<DLLNode<int>,int> cll;
    cll.insert(5);
    cll.insert(3);
    cll.insert(100);
    cll.insert(505);
    cll.insert(70);
    cll.printList();
    cll.remove();
    cll.printList();
}

void test_sll(){
    SingleLinkedList sll;
    sll.insert(1);
    sll.insert(2);
    sll.insert(3);
    sll.insert(4);
    sll.insert(6);
    sll.insert(7);
    sll.insert(9);
    sll.insert(89);
    sll.print_list();
    cout<<"=============================="<<endl;
    cout<<(sll.middleElement())->getData()<<endl;
}
int main(int argc, char const *argv[])
{
    test_sll();
    return 0;
}
