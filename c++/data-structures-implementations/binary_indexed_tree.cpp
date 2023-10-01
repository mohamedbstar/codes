#include <iostream>
#include <math.h>
using namespace std;
#define BIT_MAX 8

class BinaryIndexedTree{
public:
    int* arr = new int[BIT_MAX + 1];
    BinaryIndexedTree(int* a){
        arr[0] = 0;
        for(int i = 1; i <= BIT_MAX; i++){
            arr[i] = a[i-1];
        }
        construct_tree();
    }
    int get_sum(int idx){
        int sum = 0;
        int i = idx;
        while(i > 0){
            int lsb = get_lsb(i);
            int num_below = pow(2,lsb);
            sum += arr[i];
            i -= num_below;
        }
        return sum;
    }
    int get_sum_range(int left_idx, int right_idx){
        return get_sum(right_idx) - get_sum(left_idx - 1);
    }
    void construct_tree(){
        //construct the tree
        for (size_t i = 1; i <= BIT_MAX; i++)
        {
            int lsb = get_lsb(i);
            int num_below = pow(2,lsb);
            while (num_below > 1)
            {
                arr[i] += arr[i-num_below];
                num_below -= pow(2,get_lsb());
            }
        }
    }
    int get_lsb(int idx){
        if (idx == 0)
        {
            cout<<"idx = 0"<<endl;
            return -1;
        }
    
        int pos = 0;
        while ((idx % 2) == 0)
        {
            idx>>=1;
            pos++;
        }
        return pos;
    }
};

int main(int argc, char const *argv[])
{
    int arr[] = {1,2,3,4,5,6,7,8};
    BinaryIndexedTree bit(arr);
    cout<<bit.get_sum()<<endl;
    return 0;
}
