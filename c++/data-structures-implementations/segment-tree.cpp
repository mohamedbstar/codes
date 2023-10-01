#include <iostream>
#include <limits>
using namespace std;

//a SUM example of segment tree
typedef int (*less_type)(int,int);

template <typename T,less_type op, int size>
class GenericSegTree{
public:
    static const int tree_size = 4*size;
    T tree[tree_size];
    T* arr;
    GenericSegTree(T* a){
        arr = a;
        for (size_t i = 0; i < tree_size; i++)
        {
            tree[i] = 0;
        }
        build_tree(0, 0, size-1);
    }
    //gets the parent of the index i
    int parent(int i){
        if (i > 0)
        {
            return ((i-1)/2);
        }
        return i;
    }

    void build_tree(int root, int left, int right){
        if (left == right)
        {
            tree[root] = arr[left];
            return;
        }
        int mid = (left+right)/2;
        //build left tree
        build_tree(2*root + 1, left, mid);
        //build right tree
        build_tree(2*root + 2, mid + 1, right);
        tree[root] = op(tree[2*root+1], tree[2*root+2]);
    }

    T query(int root,int left, int right,int query_left, int query_right){
        if(query_left <= left && right <= query_right)
            return tree[root];
        
        int mid = (left + right) / 2;
        T op_value = numeric_limits<int>::max();//op_value corresponds to an element of type T which has least value compared to others
        if(query_left <= mid)
            op_value = op(op_value, query(2*root+1, left, mid, query_left, query_right));
        if(query_right > mid)
            op_value = op(op_value, query(2*root+2, mid+1, right, query_left, query_right));
        return op_value;
    }
    void update(int root,int idx,int left, int right, T new_val){
        //root has the op_value of the range [left:right]
        if (left == right)
        {
            //found the index
            tree[root] = new_val;
            return;
        }
        
        int mid = (left + right) / 2;
        if (idx <= mid)
        {
            update(2*root + 1, idx, left, mid, new_val);
        }else if (idx > mid)
        {
            update(2*root + 2, idx, mid+1, right, new_val);
        }
        //update the root
        T left_val = tree[2*root + 1];
        T right_val = tree[2*root + 2];
        tree[root] = op(left_val, right_val);
    }
};
int less_than(int a, int b){
    int m = a < b? a: b;
    return m;
}

int main(int argc, char const *argv[])
{
    int arr[] = {4,7,1,-5,6,-8,20,-9,2};
    GenericSegTree<int, less_than, 9> st(arr);
    cout<<st.tree[0]<<endl;
    st.update(0,7,0,8,-80);
    cout<<st.tree[0]<<endl;
    return 0;
}