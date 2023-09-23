#include <iostream>
#include <list>
#include <algorithm>
using namespace std;
template <typename T>
bool (*lessThan)(T t1, T t2); //this is a template function serving as a comparator for th epriority queue

//we can also define function object via a class
//NOTE that in real life situations, Comparator will not be templated as it needs to meet the actual types of compared elements
template <typename T>
class Comparator
{
public:
    bool operator()(T t1,T t2){
        return t1 < t2;
    }
};

template <typename T, typename C>
class PriorityQueue
{
private:
    int ssize;
    list<T> l;
    C isLess;
public:
    PriorityQueue(){
        ssize = 0;
    }
    void insert(T t){
        auto it = l.begin();
        while (it != l.end() && isLess(t, *it))
        {
            ++it;
        }
        l.insert(it, t);
    }
    T top(){
        return l.front();
    }
};

int main(int argc, char const *argv[])
{
    PriorityQueue< int, Comparator<int> > pq;
    pq.insert(5);
    pq.insert(70);
    pq.insert(1);
    pq.insert(100);
    pq.insert(-1);
    pq.insert(900);
    pq.insert(3);
    cout<<pq.top()<<endl;
    return 0;
}
