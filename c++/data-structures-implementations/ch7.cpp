#include <iostream>
#include <memory>

using namespace std;

template <typename T>
class GenericTree
{
private:
    shared_ptr<T> root;
public:
    GenericTree();
    ~GenericTree();
    void insert(shared_ptr<T> newNode);
    void remove(shared_ptr<T> newNode);
    void inorderTraversal(shared_ptr<T> cur);
    shared_ptr<T> getRoot();
    shared_ptr<T> find(shared_ptr<T> toFind);
    void doSwapping(shared_ptr<T> tmp, shared_ptr<T> ret);
    void removeRoot();
};
template <typename T>
GenericTree<T>::GenericTree()
{
    cout<<"Consstructing Tree"<<endl;
}
template <typename T>
GenericTree<T>::~GenericTree()
{
    cout<<"Destructing Tree"<<endl;
}
template <typename T>
void GenericTree<T>::insert(shared_ptr<T> newNode){
    if (root == nullptr)
    {
        root = newNode;
    }else{
        shared_ptr<T> cur = root;
        while (true)
        {
            if (cur->getData() == newNode->getData())
            {
                return;
            }else if (cur->getData() > newNode->getData())
            {
                //test if there is a left child or not
                if(cur->getLeft().use_count() == 0)
                    break;
                cur = cur->getLeft();
            }else{
                //test if there is a right child or not
                if(cur->getRight().use_count() == 0)
                    break;
                cur = cur->getRight();
            }
        }
        if (cur->getData() > newNode->getData())
        {
            cur->setLeft(newNode);
            weak_ptr<T> ptr = cur;
            newNode->setParent(ptr);
        }else if(cur->getData() < newNode->getData()){
            cur->setRight(newNode);
            weak_ptr<T> ptr = cur;
            newNode->setParent(ptr);
        }
    }
}
template <typename T>
shared_ptr<T> GenericTree<T>::find(shared_ptr<T> toFind){
    bool found = false;
    shared_ptr<T> cur = root;
    while (true)
    {
        if (cur.use_count() == 0)
        {
            break;
        }
        if (cur->getData() == toFind->getData())
        {
            found = true;
            break;
        }else{
            if (cur->getData() > toFind->getData())
            {
                cur = cur->getLeft();
            }else{
                cur = cur->getRight();
            }
        }
    }
    shared_ptr<T> toRet;
    if (found)
    {
        toRet = cur;
    }    
    return toRet;
}


template <typename T>
void GenericTree<T>::remove(shared_ptr<T> newNode){
    shared_ptr<T> ret = find(newNode);
    if (ret.use_count() > 0)
    {
        //if it's a leaf node, just remove it
        if (ret->getLeft().use_count() == 0 && ret->getRight().use_count() == 0)
        {
            cout<<"A leaf Node"<<endl;
            if (ret->getParent().lock().use_count() > 0)
            {
                if(ret->getParent().lock()->getLeft()->getData() == ret->getData())
                    //it's left 
                    ret->getParent().lock()->getLeft().reset();
                else
                    //it's right
                    ret->getParent().lock()->getRight().reset();
            }
            ret.reset();
        }else{
            cout<<"Not leaf Node"<<endl;
            //it's not a leaf node
            shared_ptr<T> tmp;
            //we have two options; [1] replace it with predecessor [2] replace it with successor
            if(ret->getLeft().use_count() > 0){
                //get predecessor
                tmp = ret->getLeft();
                while (tmp->getRight().use_count() > 0)
                {
                    tmp = tmp->getRight();
                }
                
                //now tmp is the predecessor
                cout<<"Predecessor is "<<tmp->getData()<<endl;
                /*tmp.swap(ret);
                cout<<"After Swapping: ret is "<<ret->getData()<<" and tmp is "<<tmp->getData()<<endl;
                
                if (ret->getParent().lock()->getLeft()->getData() == tmp->getData())
                {
                    ret->getParent().lock()->setLeft(ret);
                }else{
                    ret->getParent().lock()->setRight(ret);
                }
                
                tmp.reset();
                
                cout<<"After reset(): tmp use_count() is "<<tmp.use_count()<<endl;*/
                doSwapping(tmp,ret);

            }else{
                //get successor
                tmp = ret->getRight();
                while (tmp->getLeft().use_count() > 00)
                {
                    tmp = tmp->getLeft();
                }
                cout<<"Successor is "<<tmp->getData()<<endl;
                //ret->getParent().lock()
                
                //cout<<"After Swapping: ret is "<<ret->getData()<<" and tmp is "<<tmp->getData()<<endl;
                
                /*if (ret->getParent().lock()->getLeft()->getData() == tmp->getData())
                {
                    ret->getParent().lock()->setLeft(ret);
                }else{
                    ret->getParent().lock()->setRight(ret);
                }*/
                doSwapping(tmp,ret);
                
                //cout<<"After reset(): tmp use_count() is "<<tmp.use_count()<<endl;
            }
        }
    }else{
        cout<<"Not Existing Node in the Tree"<<endl;
    }
}
template <typename T>
void GenericTree<T>::doSwapping(shared_ptr<T> tmp, shared_ptr<T> ret){
    tmp.swap(ret);
    shared_ptr<T> tmpLeft = tmp->getLeft();
    shared_ptr<T> tmpRight = tmp->getRight();
    weak_ptr<T> tmpParent = tmp->getParent();
    //weak_ptr<T> retParent = ret->getParent();
    if(!(tmpRight.use_count() > 0 && tmpRight->getData() == ret->getData()))
        ret->setRight(tmpRight);
    if(!(tmpLeft.use_count() > 0 && tmpLeft->getData() == ret->getData()))
        ret->setLeft(tmpLeft);
    ret->setParent(tmpParent);
    if(!((tmpRight.use_count() > 0 && tmpRight->getData() == ret->getData()) &&(tmpLeft.use_count() > 0 && tmpLeft->getData() == ret->getData())))
        if (tmpParent.lock().use_count() > 0)
        {
            if (ret->getParent().lock()->getLeft()->getData() == tmp->getData())
            {
                ret->getParent().lock()->setLeft(ret);
            }else{
                ret->getParent().lock()->setRight(ret);
            }
        }
    tmp.reset();
}
/*==============TO IMPLEMENT=================*/
template <typename T>
void GenericTree<T>::removeRoot(){   //======
                                    //======
    //get predecessor               //======
}                                   //======
/*===========================================*/
template <typename T>
void GenericTree<T>::inorderTraversal(shared_ptr<T> cur){
    if (cur->getLeft().use_count() > 0)
    {
        inorderTraversal(cur->getLeft());
    }
    cout<<cur->getData()<<" ";
    if (cur->getRight().use_count() > 0)
    {
        inorderTraversal(cur->getRight());
    }
    if (cur->getData() == root->getData())
    {
        cout<<endl;
    }
    
}
template <typename T>
shared_ptr<T> GenericTree<T>::getRoot(){
    return root;
}
template <typename T>
class TreeNode
{
private:
    T data;
    shared_ptr<TreeNode<T>> left;
    shared_ptr<TreeNode<T>> right;
    weak_ptr<TreeNode<T>> parent;
public:
    TreeNode(T d = 0){
        data = d;
    }
    ~TreeNode(){
        cout<<"Destructing Node: "<<data<<endl;
    }
    T getData(){
        return data;
    }
    void setData(T d){
        data = d;
    }
    shared_ptr<TreeNode<T>> getLeft(){
        return left;
    }
    shared_ptr<TreeNode<T>> getRight(){
        return right;
    }
    weak_ptr<TreeNode<T>> getParent(){
        return parent;
    }
    void setLeft(shared_ptr<TreeNode<T>> l){
        left = l;
    }
    void setRight(shared_ptr<TreeNode<T>> r){
        right = r;
    }
    void setParent(weak_ptr<TreeNode<T>> p){
        parent = p;
    }
};

void testTree(){
    shared_ptr<TreeNode<int>> t1 = make_shared<TreeNode<int>>(10);
    shared_ptr<TreeNode<int>> t2 = make_shared<TreeNode<int>>(8);
    shared_ptr<TreeNode<int>> t3 = make_shared<TreeNode<int>>(11);
    shared_ptr<TreeNode<int>> t4 = make_shared<TreeNode<int>>(12);
    shared_ptr<TreeNode<int>> t5 = make_shared<TreeNode<int>>(9);
    shared_ptr<TreeNode<int>> t6 = make_shared<TreeNode<int>>(1000);
    shared_ptr<TreeNode<int>> t7 = make_shared<TreeNode<int>>(-55);
    shared_ptr<TreeNode<int>> t8 = make_shared<TreeNode<int>>(60);
    GenericTree<TreeNode<int>> tree;
    tree.insert(t1);
    tree.insert(t2);
    tree.insert(t3);
    tree.insert(t4);
    tree.insert(t5);
    tree.insert(t6);
    tree.insert(t7);
    tree.insert(t8);
    tree.inorderTraversal(tree.getRoot());
    tree.remove(t2);
    tree.inorderTraversal(tree.getRoot());
    cout<<tree.getRoot()->getRight()->getData()<<endl;
    cout<<tree.getRoot()->getRight()->getRight()->getData()<<endl;
}

int main(int argc, char const *argv[])
{
    testTree();
    return 0;
}
