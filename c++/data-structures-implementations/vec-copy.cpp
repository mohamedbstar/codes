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

void test_copy(){
    GenericVector<int> v{1,2,3};
    cout<<v;
}

int main(int argc, char const *argv[])
{
    test_copy();
    return 0;
}
