#include <iostream>
#include <vector>

using namespace std;


template <typename N,  typename C>//N for Node and C for comparator
class PQ_Heap
{
private:
    
public:
    vector<N> tree;//(10, nullptr); //a backing store for nodes
    C isLess;
    int size() const; // number of elements
    bool empty() const; // is the queue empty?
    void insert( N& node); // insert element
    const N& min(); // minimum element
    void removeMin();
    void upHeap(int idx);
    void downHeap(int idx);
    bool hasChild(int idx);
    ~PQ_Heap();
};


template <typename N, typename C>
int PQ_Heap<N,C>::size() const{
    return tree.size();
}

template <typename N, typename C>
bool PQ_Heap<N,C>::empty() const{
    return tree.empty();
}

template <typename N, typename C>
void PQ_Heap<N,C>::upHeap(int idx){
    if (idx == 0)
    {
        //base case when at the root
        return;
    }
    
    int parent = idx / 2;
    if (isLess(tree[idx], tree[parent]))
    {
        swap(tree[idx], tree[parent]); //swap their contents
        upHeap(parent);//then upheap for the parent to preserve the heap structural property
    }
}

template <typename N, typename C>
void PQ_Heap<N,C>::insert(N& node){// insert Node e
    tree.push_back(node);
    if (size() != 1)
    {
        upHeap(tree.size() - 1);//the index of the last element
    }
} 


template <typename N, typename C>
const N& PQ_Heap<N,C>::min(){
    if (empty())
    {
        cout<<"Empty Heap!"<<endl;
        N n;
        return n; //return a dummy node
    }
    return tree[0];
}

template <typename N, typename C>
void PQ_Heap<N,C>::removeMin(){
    if (empty())
    {
        cout<<"Empty Heap!"<<endl;
        return;
    }
    //first swap minimum element with the last element
    swap(tree[0], tree[tree.size() - 1]);
    tree.erase(tree.end()-1); //delete the swapped element a.k.a the min element
    //then downHeap from the root element
    downHeap(0);
}

template <typename N, typename C>
bool PQ_Heap<N,C>::hasChild(int idx){
    if (idx < tree.size())
    {
        return true;
    }
    return false;
}

template <typename N, typename C>
void PQ_Heap<N,C>::downHeap(int idx){
    if ( idx >= tree.size() )
    {
        /* base case when not at a node */
        return;
    }
    int leftChild = (2*idx) + 1;
    int rightChild = (2*idx) + 2;
    int smallestChild = idx;//in case it has no children
    if (hasChild(leftChild) && tree[leftChild] < tree[idx])
    {
        smallestChild = leftChild;
    }
    if (hasChild(rightChild) && tree[smallestChild] > tree[rightChild])
    {
        smallestChild = rightChild;
    }
    if (smallestChild != idx)
    {
        swap(tree[idx], tree[smallestChild]);
        downHeap(smallestChild);
    }
    
    
}
template <typename N, typename C>
PQ_Heap<N,C>::~PQ_Heap()
{
    /*if (!empty())
    {
        for (size_t i = 0; i < size(); i++)
        {
            delete tree[i];
        }
        cout<<"deleted all nodes"<<endl;
    }*/
}

template <typename N>
void heapify(int idx, N* arr, int size){
    int largestChild = idx;
    int leftChild = idx*2 + 1;
    int rightChild = idx*2 + 2;

    if(leftChild < size && arr[leftChild] > arr[largestChild]){
        largestChild = leftChild;
    }
    if (rightChild < size && arr[rightChild] > arr[largestChild])
    {
        largestChild = rightChild;
    }
    if (largestChild != idx)
    {
        swap(arr[largestChild], arr[idx]);
        heapify(largestChild, arr, size);
    }
    
    
}

template <typename N>
void heapSort(N *arr, int size){
    cout<<"Initial Array ";
    for(int i = 0; i < size; i++){
        cout<<arr[i]<<" ";
    }
    cout<<endl;
    for(int i = (size/2); i >= 0 ;i--){
        heapify(i, arr, size);
    }
    for (int i = size - 1; i  > 0; i--)
    {
        swap(arr[0], arr[i]);
        heapify<N>(0, arr, i);//meaning that doen't consider i to be inclusive in the array positions
    }
    cout<<"Sorted Array ";
    for(int i = 0; i < size; i++){
        cout<<arr[i]<<" ";
    }
    cout<<endl;
}

int main(int argc, char const *argv[])
{
    return 0;
}
