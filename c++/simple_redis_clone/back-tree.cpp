#include "back-tree.h"
using namespace std;

node::node(string k,string v ,int l, int r, int p){
    key = k;
    value = v;
    left_pos = l;
    right_pos = r;
    pos = p;
    left = nullptr;
    right = nullptr;
}

file_opener::file_opener(){
    stream.open(file_name, fstream::in | fstream::ate);
    int root_pos = get_root();

    if (root_pos == 0)
    {
        root = nullptr;
    }else{
        populate_node(root_pos, &root);
    }
    
}

file_opener::~file_opener(){
    stream.close();
}
void file_opener::change_value(string new_value, node* cur){
    stream.seekp(cur->pos+4, ios::beg);//like seek_set for this->value offset
    stream.write(new_value.c_str(), value_max);
    stream.flush();
}
//populates a node object from file position
void file_opener::populate_node(int pos, node** n){
    cout<<"in populate data"<<endl;
    stream.seekg(pos);
    char key_buf[5];
    string key, value;
    char val_buf[value_max];
    char left_buf[5];
    char right_buf[5];
    char heigh_buf[5];
    int val_len;
    
    stream.read(key_buf, 4);
    key_buf[4] = '\0';
    stream.read(val_buf, value_max);
    val_buf[value_max - 1] = '\0';
    stream.read(heigh_buf, 4);
    heigh_buf[4] = '\0';

    if(stream.tellp() != -1){
        stream.read(left_buf, 4);
    }else{
        strcpy(left_buf, "-1");
    }
    left_buf[4] = '\0';
    if(stream.tellp() != -1){
        stream.read(right_buf, 4);
    }else{
        strcpy(right_buf, "-1");
    }
    right_buf[4] = '\0';
    key = key_buf;
    value = val_buf;
    (*n) = new node(key,value ,stoi(left_buf), stoi(right_buf), pos);
    cout<<"out populate data"<<endl;
}
//the file position of the root node
int file_opener::get_root(){
    cout<<"in get_root"<<endl;
    //the root position is in the first 4 bytes
    char root_pos[4];
    int i =0;
    for(i; i < 4; i++){
        stream.read((root_pos+i), 1);
        if(root_pos[i] == '\n'){
            root_pos[i] = '\0';
            break;
        }
    }
    cout<<"root_pos "<<root_pos<<endl;
    cout<<"out get_root"<<endl;
    return stoi(root_pos);
}
bool file_opener::exists(string key, node* cur){
    if (cur == nullptr)
    {
        return false;
    }
    if(key < cur->key){
        if (cur->left_pos == -1)
        {
            return false;
        }
        if (cur->left == nullptr)
        {
            populate_node(cur->left_pos, &(cur->left));
        }
        return exists(key, cur->left);
    }else if (key > cur->key)
    {
        if (cur->right_pos == -1)
        {
            return false;
        }
        if(cur->right == nullptr){
            populate_node(cur->right_pos, &(cur->right));
        }
        return exists(key, cur->right);
    }else{
        //here the key = cur->key
        return true;
    }
    
}
int file_opener::write_node(string key, string value, int left_pos, int right_pos){
    cout<<"in write_node"<<endl;
    stream.seekp(0, ios::end);
    int cur_pos = stream.tellp();
    cout<<"at pos "<<cur_pos<<endl;

    stream.write(key.c_str(), 4);
    stream.write(value.c_str(), value_max);

    stream.write(to_string(left_pos).c_str(), 4);
    stream.write(to_string(right_pos).c_str(), 4);
    stream.flush();
    return cur_pos;
}
void file_opener::insert(string key, string value, node* cur){
    if(key < cur->key){
        if (cur->left_pos == -1)
        {
            stream.seekp(0, ios::end);//seek to the end of a file
            //write new node at the file
            int new_pos = write_node(key, value, -1, -1);
            cur->left = new node(key, value, -1, -1, new_pos);
            cur->left_pos = new_pos;
        }else{
            if(cur->left == nullptr){
                populate_node(cur->left_pos, &(cur->left));
            }
            insert(key, value, cur->left);
        }
        
    }else if(key > cur->key){
        cout<<"key > cur->key"<<endl;
        if(cur->right_pos == -1){
            cout<<"cur->right_pos = -1"<<endl;
            int new_pos = write_node(key, value, -1,-1);
            cur->right = new node(key, value, -1,-1, new_pos);
            cur->right_pos = new_pos;
        }else{
            cout<<"cur->right_pos != -1"<<endl;
            if(cur->right == nullptr){
                cout<<"cur->right = nullptr"<<endl;
                populate_node(cur->right_pos, &(cur->right));
            }
            insert(key, value, cur->right);
        }
    }else{
        //the key = cur->key
        if (value == cur->value)
        {
            cout<<"value = value"<<endl;
        }else{
            cout<<"Changing value"<<endl;
            change_value(value, cur);
        }
    }
}
int main(int argc, char const *argv[])
{
    file_opener fo;
    cout<<fo.root->key<<endl;
    cout<<fo.root->right_pos<<endl;
    cout<<fo.root->left_pos<<endl;
    fo.insert("kkkk", "vvvv", fo.root);
    cout<<fo.root->right->key<<endl;
    cout<<fo.root->right->value<<endl;
    fo.stream.close();
    return 0;
}
