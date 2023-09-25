#include "avl_tree.h"

using namespace std;

template <typename T>
avl_node<T>::avl_node(T d){
    data = d;
    height = 1;//as it has itself accounted for
    left = right = nullptr;
}
template <typename T>
avl_node<T>* avl_node<T>::left_rotate(){
    avl_node<T>* old_cur_right = this->right;
    this->right = old_cur_right->left;

    old_cur_right->left = this;
    this->height = 1 + max(this->left->get_height(), this->right->get_height());
    old_cur_right->height = 1 + max(old_cur_right->left->get_height(), old_cur_right->right->get_height());
    return old_cur_right;
}
template <typename T>
avl_node<T>* avl_node<T>::right_rotate(){
    avl_node<T>* old_cur_left = this->left;
    this->left = old_cur_left->right;

    old_cur_left->right = this;
    this->height = 1 + max(this->left->get_height(), this->right->get_height());
    old_cur_left->height = 1 + max(old_cur_left->left->get_height(), old_cur_left->right->get_height());
    return old_cur_left;
}
template <typename T>
avl_node<T>* avl_node<T>::find_predecessor(){
    avl_node<T>* cur = this;
    while (cur->right != nullptr)
    {
        cur = cur->right;
    }
    return cur;
}
template <typename T>
avl_node<T>* avl_node<T>::find_successor(){
    avl_node<T>* cur = this;
    while (cur->left != nullptr)
    {
        cur = cur->left;
    }
    return cur;
}
template <typename T>
avl_tree<T>::avl_tree(){
    num_nodes = 0;
    root = nullptr;
}
template <typename T>
bool avl_tree<T>::find(avl_node<T>* cur, T element){
    if (cur == nullptr)
    {
        return false;
    }
    return (cur->data == element ||find(cur->left, element) || find(cur->right, element)); 
}
template <typename T>
int avl_node<T>::get_height(){
    if(this == nullptr)
        return 0;
    return this->height;
}
template <typename T>
avl_node<T>* avl_tree<T>::insert_util(avl_node<T>* cur, T element){
    if (cur == nullptr)
    {
        avl_node<T>* new_node = new avl_node<T>(element);
        num_nodes++;
        return new_node;
    }
    if (element < cur->data)
    {
        cur->left = insert_util(cur->left, element);
    }else if (element > cur->data)
    {
        cur->right = insert_util(cur->right, element);
    }
    cur->height = 1 + max(cur->left->get_height(), cur->right->get_height());
    //here we have returned from adding the new_node
    //handle the rotation
    int right_height = 0;
    int left_height = 0;
    if(cur->right != nullptr)
        right_height = cur->right->height;
    if(cur->left != nullptr)
        left_height = cur->left->height;
    int dif = left_height - right_height;
    
    //4 case: left-left, left-right, right-left, right-right
    if (dif > 1 && left != nullptr && element < cur->left->data)
    {
        //left-left case
        cur = cur->right_rotate();
    }else if(dif > 1 && left != nullptr && element > cur->left->data){
        //left-right
        cur->left = cur->left->left_rotate();
        cur = cur->right_rotate();
    }else if (dif < -1 && right != nullptr && element < cur->right->data)
    {
        /* right-left case */
        cur->right = cur->right->right_rotate();
        cur = cur->left_rotate();
    }else if (dif < -1 && right != nullptr && element > cur->right->data)
    {
        //right-right case
        cur = cur->left_rotate();
    }
    return cur;
}
template <typename T>
void avl_tree<T>::insert(avl_node<T>* cur, T element){
    if (find(cur, element))
    {
        cout<<"Existing Node"<<endl;
        return;
    }
    root = insert_util(cur, element);
}
template <typename T>
void avl_tree<T>::inorder(avl_node<T>* cur){
    if (cur->left != nullptr)
    {
        inorder(cur->left);
    }
    cout<<cur->data<<" ";
    if (cur->right)
    {
        inorder(cur->right);
    }
}
template <typename T>
avl_node<T>* avl_tree<T>::remove_util(avl_node<T>* cur, T element){
    if (element < cur->data)
    {
        cur->left = remove_util(cur->left, element);
    }else if (element > cur->data)
    {
        cur->right = remove_util(cur->right, element);
    }else{
        avl_node<T>* tmp;
        //it's the node to be removed
        if (cur->left == nullptr && cur->right == nullptr)//if it's leaf node, just remove it
        {
            delete cur;
            num_nodes--;
            return nullptr;
        }else if(cur->left == nullptr){
            tmp = cur->right->find_successor();
            cur->data = tmp->data;
            cur->right = remove_util(cur->right, tmp->data);
        }else{
            tmp = cur->left->find_predecessor();
            cur->data = tmp->data;
            cur->left = remove_util(cur->left, tmp->data);
        }   
    }

    cur->height = 1 + max(cur->left->get_height(), cur->right->get_height());

    //handle the rotation as in insert
    int right_height = 0;
    int left_height = 0;
    if(cur->right != nullptr)
        right_height = cur->right->height;
    if(cur->left != nullptr)
        left_height = cur->left->height;
    int dif = left_height - right_height;
    
    //4 case: left-left, left-right, right-left, right-right
    if (dif > 1 && left != nullptr && element < cur->left->data)
    {
        //left-left case
        cur = cur->right_rotate();
    }else if(dif > 1 && left != nullptr && element > cur->left->data){
        //left-right
        cur->left = cur->left->left_rotate();
        cur = cur->right_rotate();
    }else if (dif < -1 && right != nullptr && element < cur->right->data)
    {
        /* right-left case */
        cur->right = cur->right->right_rotate();
        cur = cur->left_rotate();
    }else if (dif < -1 && right != nullptr && element > cur->right->data)
    {
        //right-right case
        cur = cur->left_rotate();
    }
    return cur;
    
}
template <typename T>
void avl_tree<T>::remove(avl_node<T>* cur, T element){
    if (!find(cur, element))
    {
        cout<<"Not Existing Node"<<endl;
        return;
    }
    root = remove_util(cur,element );
}
int main(int argc, char const *argv[])
{
    avl_tree<int> avl;
    avl.insert(avl.root,10);
    avl.insert(avl.root, 9);
    avl.insert(avl.root, 8);
    avl.insert(avl.root, 7);
    avl.insert(avl.root, 6);
    avl.insert(avl.root, 5);
    avl.inorder(avl.root);
    cout<<endl;
    cout<<"root is "<<avl.root->data<<endl;
    avl.remove(avl.root, 6);
    avl.inorder(avl.root);
    cout<<endl;
    return 0;
}