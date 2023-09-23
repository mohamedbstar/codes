#include <iostream>

using namespace std;

int compSum(int arr[3][3], int r, int c, int size = 3){
    if (c >= size)
    {
        return compSum(arr, r+1,0,size);
    }
    if (r >= size)
    {
        return 0;
    }
    int s = arr[r][c] + compSum(arr, r,c+1,size);
    return s;
}

int maxArrRecur(int arr[], int size, int pos){
    if (pos >= size)
    {
        return -1;
    }
    
    return max(arr[pos], maxArrRecur(arr, size, pos+1));
}

int main(int argc, char const *argv[])
{
    int arr[] = {100,54,1,0,2,744,10,7,64};
    cout<<maxArrRecur(arr, 9, 0)<<endl;
    return 0;
}
