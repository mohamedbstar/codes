#include <iostream>
#include <math.h>

using namespace std;
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
int main(int argc, char const *argv[])
{
    cout<<pow(2,0)<<endl;
    cout<<pow(2,1)<<endl;
    cout<<pow(2,2)<<endl;
    return 0;
}
