#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#define file_name   "redis"
#define MAX_KEY     6
#define KEY_LEN     7
#define MAX_VAL     64
#define VAL_len     65
#define MAX_POS     10
#define POS_LEN     11

/*
*       TO DO: keep tracking of deleted nodes and their positions to use then for subsequent inserts
*/

class file_node{
public:
    signed char key_len;//hold actual number of bytes written as key out of MAX_KEY bytes
    char key[MAX_KEY];
    signed char val_len;//hold actual number of bytes written as value out of MAX_VAL bytes
    char value[MAX_VAL];
    signed char left_len;
    char left[MAX_POS];
    signed char right_len;
    char right[MAX_POS];
    signed char pos_len;
    char pos[MAX_POS];
    
};

class mem_node{
public:
    string key;
    string value;
    mem_node* left;
    mem_node* right;
    char file_pos[MAX_POS];

    mem_node(file_node* f_node): key(f_node->key), value(f_node->value)
    {
        left = nullptr;
        right = nullptr;
        memcpy(file_pos, f_node->pos, f_node->pos_len);
    }
};

class file_handler
{
public:
    
    file_node* root;
    mem_node* mem_root;
    unsigned char num_nodes = 0;
    fstream input_file;
    mem_node* mem_parent;
    file_handler(){
        cout<<"In constructor"<<endl;
        fstream i_file("redis");
        input_file = move(i_file);
        cout<<"after move(i_file)"<<endl;
        mem_parent = nullptr;
        cout<<"before load_root"<<endl;
        load_root();
        cout<<"after load_root"<<endl;
    }
    
    void load_root(){
        //get first byte to see the number of nodes
        input_file.seekg(0,ios::beg);
        num_nodes = input_file.get();
        if (num_nodes == 0)
        {
            root = nullptr;
            mem_root = nullptr;
        }else{

            //get the next byte to see the number of characters used to write root address
            char num_node_off = input_file.get();
            cout<<"num_nodes_off = "<<num_node_off<<endl;
            char buf[num_node_off+1];
            input_file.read(buf, num_node_off);//now we have got the node offset as text in buf
            buf[num_node_off] = '\0';
            root = new file_node;
            read_node(buf, root);
            //now populate mem_root
            mem_root = new mem_node(root);
            delete root;
        }   
    }
    void read_node(const char* pos, file_node* node){
        cout<<"Entering read_node"<<endl;
        input_file.seekg(stoi(pos));//seek to the node offset
        cout<<"after input_file.seekg(stoi(pos))"<<endl;
        //now first read the length of node key
        node->key_len = input_file.get();
        //then read the next key_len bytes into node->key
        input_file.read(node->key, node->key_len);
        //now read the length of node val
        node->val_len = input_file.get();
        //then read the next key_len bytes into node->key
        input_file.read(node->value, node->val_len);
        //now read the length of node left
        node->left_len = input_file.get();
        //then read the next key_len bytes into node->key
        input_file.read(node->left, node->val_len);
        //now read the length of node right
        node->right_len = input_file.get();
        //then read the next key_len bytes into node->key
        input_file.read(node->right, node->val_len);
        //now read the length of node pos
        node->pos_len = input_file.get();
        //then read the next key_len bytes into node->key
        input_file.read(node->pos, node->val_len);
    }

    mem_node* find(string& key, mem_node* cur){
        if (key.compare(cur->key) < 0)
        {
            //key < cur->key
            //check first if cur has a left or not
            mem_parent = cur;
            if (cur->left == nullptr)
            {
                //check if there is a node on disk or not
                file_node* tmp = new file_node;//represents the cur file_node on disk
                read_node(cur->file_pos, tmp);
                if(tmp->left_len == 0){
                    delete tmp;
                    return cur;
                }
                //now it has a left -> load it from disk and put in into memory

                read_node(tmp->left, tmp);//now tmp contains on-disk node of left
                cur->left = new mem_node(tmp);//put on-disk left in mmeory
                delete tmp;//no longer needs the in-disk node of left or cur
            }
            //it has a left
            return find(key, cur->left);
        }else if (key.compare(cur->key) > 0)
        {
            mem_parent = cur;
            //key > cur->key
            if (cur->right == nullptr)
            {
                //check if there is a node on disk or not
                file_node* tmp = new file_node;//represents the cur file_node on disk
                read_node(cur->file_pos, tmp);
                if(tmp->right_len == 0){
                    delete tmp;
                    return cur;
                }
                //now it has a left -> load it from disk and put in into memory
                read_node(tmp->right, tmp);
                cur->right = new mem_node(tmp);
                delete tmp;
            }
            //it has a left
            return find(key, cur->right);
        }else{
            //key == cur->key
            return cur;
        }
        //unexpected case
        cout<<"Error in find UNEXPECTED CASE"<<endl;
        exit(0);
    }
    //TODO: detect whether a left or right node is deleted
    mem_node* find_key(string& key){
        if (num_nodes == 0)
        {
            cout<<"Empty tree"<<endl;
            return nullptr;
        }
        return find(key, mem_root);
    }
    void extend_file_from_pos(const char* pos, size_t amount_to_extend){
        string tmp;
        //we need to get the data from pos to eof in memory then make room then write them again
        long from_offset = stoi(pos);
        long to_offset = from_offset + amount_to_extend;

        //seek to pos
        input_file.seekg(from_offset, ios::beg);
        //then read from here until the eof and store it in a string
        while (input_file >> tmp);//now tmp contains all data after pos
        //write 0's as dummy data to preserve space
        input_file.seekp(from_offset, ios::beg);
        for(int i = 0; i < amount_to_extend; i++)
            input_file.put('0');
        //then write the tmp data starting from pos offset+amount_to_extend
        input_file.seekp(to_offset, ios::beg);
        while (input_file << tmp);
        
    }
    void write_node(file_node* f_cihld){
        input_file.put(f_cihld->key_len);
        input_file.write(f_cihld->key, f_cihld->key_len);

        input_file.put(f_cihld->val_len);
        input_file.write(f_cihld->value, f_cihld->val_len);

        input_file.put(f_cihld->left_len);
        input_file.write(f_cihld->left, f_cihld->left_len);

        input_file.put(f_cihld->right_len);
        input_file.write(f_cihld->right, f_cihld->right_len);

        input_file.put(f_cihld->pos_len);
        input_file.write(f_cihld->pos, f_cihld->pos_len);
        
        input_file.flush();//to synchronize writing
    }
    string write_file_node_as_left(string& file_pos,file_node* f_child){
        //seek to the end of the file
        input_file.seekp(0, ios::end);

        long cur_offset = input_file.tellp();//the new_offset that the new node will be written at
        write_node(f_child);//now the new child is on disk

        return to_string(cur_offset);
    }
    string write_file_node_as_right(string& file_pos,file_node* f_child){
        //seek to the end of the file
        input_file.seekp(0, ios::end);

        long cur_offset = input_file.tellp();//the new_offset that the new node will be written at
        write_node(f_child);//now the new child is on disk

        return to_string(cur_offset);
    }
    file_node* create_new_file_node(string& key, string& value){
        file_node* f_child = new file_node;
        f_child->key_len = key.size();
        memcpy(f_child->key, key.c_str(), key.size());
        f_child->val_len = value.size();
        memcpy(f_child->value , value.c_str(), value.size());
        f_child->left_len = 0;
        memcpy(f_child->left, "", 0);
        f_child->right_len = 0;
        memcpy(f_child->right, "", 0);
        f_child->pos_len = 0;
        memcpy(f_child->pos, "", 0);

        return f_child;
    }
    void insert_left(string& key, string& value,mem_node* mem_par){
        cout<<"inside insert_left of"<<mem_par->key<<endl;
        file_node* f_child = create_new_file_node(key, value);

        string tmp(mem_par->file_pos);
        string left_pos = write_file_node_as_left(tmp , f_child);
        mem_node* new_left = new mem_node(f_child);
        //assign new_left to be left in memory
        mem_par->left = new_left;
        //assign f_child to be left on disk
        long left_ptr_pos_on_disk = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + 2;
        extend_file_from_pos(to_string(left_ptr_pos_on_disk).c_str(), left_pos.size() + 1);

        input_file.seekp(left_ptr_pos_on_disk, ios::beg);
        //write the byte containing the left pos size
        input_file.put(left_pos.size());
        input_file.write(left_pos.c_str(), left_pos.size());
        //no longer need the child file node in memory->delete it
        delete f_child;
    }
    void insert_right(string& key, string& value,mem_node* mem_par){
        cout<<"inside insert_right of "<<mem_par->key<<endl;
        file_node* f_child = create_new_file_node(key, value);
        string tmp(mem_par->file_pos);
        cout<<"before write_file_node_as_right"<<endl;
        string right_pos = write_file_node_as_right(tmp, f_child);
        cout<<"after write_file_node_as_right"<<endl;
        mem_node* new_right = new mem_node(f_child);
        mem_par->right = new_right;

        //get the position on disk where we will write the right_node position
        long left_ptr_pos_on_disk = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + 2;
        char left_ptr_size_on_disk;
        input_file.seekg(left_ptr_pos_on_disk, ios::beg);
        left_ptr_size_on_disk = input_file.get();
        long right_ptr_pos_on_disk = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + left_ptr_size_on_disk + 3;
        cout<<"before extend_file_from_pos"<<endl;
        extend_file_from_pos(to_string(right_ptr_pos_on_disk).c_str(), right_pos.size() + 1);
        cout<<"after extend_file_from_pos"<<endl;
        input_file.seekp(right_ptr_pos_on_disk, ios::beg);
        input_file.put(right_pos.size());
        input_file.write(right_pos.c_str(), right_pos.size());

        delete f_child;
    }    
    void insert(string& key,string& value ,mem_node* cur){
        cout<<"in insert cur="<<cur->key<<" for key="<<key<<endl;
        if (cur->key.compare(key) < 0)
        {
            /* cur->key < key */
            cout<<"inserting into right of "<<cur->key<<endl;
            insert_right(key, value, cur);
        }else{
            //cur->key > key
            cout<<"inserting into left of "<<cur->key<<endl;
            insert_left(key, value, cur);
        }
        
    }
    void increment_num_nodes(){
        input_file.seekg(0,ios::beg);
        num_nodes = input_file.get();
        num_nodes++;
        input_file.seekp(0, ios::beg);
        input_file.put(num_nodes);
    }
    void decrement_num_nodes(){
        input_file.seekg(0,ios::beg);
        num_nodes = input_file.get();
        num_nodes--;
        input_file.seekp(0, ios::beg);
        input_file.put(num_nodes);
    }
    //TODO: keep track of deleted nodes and their sizes to overwrite them with new nodes
    void insert_key(string& key, string& value){
        cout<<"in insert_key "<<key<<endl;
        if(key.size() > MAX_KEY){
            cout<<"Key Lenght > MAX_KEY"<<endl;
            return;
        }
        if(value.size() > MAX_VAL){
            cout<<"Value Lenght > MAX_VAL"<<endl;
            return;
        }
        mem_node* last_search = find_key(key);
        if (last_search != nullptr && last_search->key.compare(key) == 0)
        {
            cout<<"Already Exsiting key"<<endl;
            return;
        }
        //check if root == nullptr
        if(mem_root == nullptr){
            cout<<"in inset_key mem_root"<<endl;
            //insert node as root
            file_node* f_root = create_new_file_node(key, value);
            input_file.seekp(1,ios::beg);//write at byte 1: always an address for root
            write_node(f_root);
            mem_root = new mem_node(f_root);
            delete f_root;
            increment_num_nodes();
            return;
        }
        cout<<"getting into insert last_search="<<last_search->key<<endl;
        insert(key, value, last_search);
        increment_num_nodes();
    }
    void shrink_file_node_pos(const char* pos, long amount_to_shrink){
        //first store data from pos+amount_to_shrink until eof
        string tmp;
        long store_offset = stoi(pos) + amount_to_shrink;
        input_file.seekg(store_offset);

        while(input_file >> tmp);//now tmp contains the data from store_offset until eof
        //now write tmp data at pos
        input_file.seekp(stoi(pos));
        while(input_file<<tmp);
        //now the file has been shrunk
    }
    void remove(string& key, mem_node* to_delete, mem_node* mem_par){
        //check no on-disk left and right
        file_node* tmp = new file_node;
        read_node(to_delete->file_pos, tmp);//reads the on-disk node into tmp
        //case 1: it has no left or right (LEAF NODE)
        if (tmp->left_len == 0 && tmp->right_len == 0)
        {//set mem_par child to nullptr
            /* ACTUALLY A LEAF NODE */
            //shrink the node space
            long shrink_pos = stoi(to_delete->file_pos);
            long shrink_size = sizeof(*tmp);
            shrink_file_node_pos(to_delete->file_pos, shrink_size);

            //make parent->left or parent->right = 0 in file and = nullptr in memory
                
            //making parent left or right on disk = 0
            read_node(mem_par->file_pos, tmp);//now tmp holds the in-disk node of the parent
            shrink_size = string(to_delete->file_pos).size();//keep 1 byte for holding 0 as left_len or right_len
            shrink_pos = stoi(mem_par->file_pos);

            //making memory parent->to_delete child as nullptr
            if(mem_par->key.compare(to_delete->key) < 0){
                //to_delete is right_child
                shrink_pos += mem_par->key.size() + mem_par->value.size() + 2;//+2 for the 2 bytes holding the key_len and val_len in file
                shrink_file_node_pos(to_string(shrink_pos).c_str(), string(to_delete->file_pos).size());
                mem_par->right = nullptr;
            }else{
                shrink_pos += mem_par->key.size() + mem_par->value.size() + tmp->left_len + 3;
                shrink_file_node_pos(to_string(shrink_pos).c_str(), string(to_delete->file_pos).size());
                mem_par->left = nullptr;
            }
            //write 0 as the left_len or right_len
            input_file.seekp(shrink_pos, ios::beg);
            input_file.put(0);
            delete to_delete;//remove it from memory   
            
        }else if(tmp->left_len == 0){
            //has only right child => shrink the in-file node and replace mem_par->right to be to_delete->right
            long shrink_pos, shrink_size;
            long new_right_pos = stoi(tmp->right);//holds the in-file position of the new_right
            shrink_pos = stoi(to_delete->file_pos);//shrink the to_delete node in file
            shrink_size = sizeof(*tmp);//holds the size of the in-file node to be deleted
            shrink_file_node_pos(to_string(shrink_pos).c_str(), shrink_size);//now we have shrinked the in-file node
            //then shrink the parent->right in file 
            read_node(mem_par->file_pos, tmp);//tmp now contains in-file mem_par
            shrink_pos = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + tmp->left_len + 3;
            shrink_size = tmp->right_len;
            shrink_file_node_pos(to_string(shrink_pos).c_str(), shrink_size);
            //make the new_right as the right of the parent
            if (to_delete->right == nullptr)
            {
                //load the new_right from file
                file_node* new_right = new file_node;
                read_node(to_string(new_right_pos).c_str(), new_right);
                mem_node* mem_new_right = new mem_node(new_right);
                to_delete->right = mem_new_right;
            }
            mem_par->right = to_delete->right;
            //write the new_right in-file node to be in-file parent->right
            long extend_pos, extend_size;
            extend_pos = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + tmp->left_len + 4;
            extend_size = string(mem_par->right->file_pos).size();
            extend_file_from_pos(to_string(extend_pos).c_str(), extend_size);
            //put the new right pos in right position in file
            input_file.seekp(extend_pos-1, ios::end);
            input_file.put(extend_size);
            input_file.write(mem_par->right->file_pos, extend_size);
            delete to_delete;
        }else{
            //has only left child => shrink the in-file node and replace mem_par->left to be to_delete->left
            long shrink_pos, shrink_size;
            long new_left_pos = stoi(tmp->left);//holds the in-file position of the new_right
            
            shrink_pos = stoi(to_delete->file_pos);//shrink the to_delete node in file
            shrink_size = sizeof(*tmp);//holds the size of the in-file node to be deleted
            shrink_file_node_pos(to_string(shrink_pos).c_str(), shrink_size);//now we have shrinked the in-file node
            //then shrink the parent->right in file 
            read_node(mem_par->file_pos, tmp);//tmp now contains in-file mem_par
            shrink_pos = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + 3;
            shrink_size = tmp->left_len;
            shrink_file_node_pos(to_string(shrink_pos).c_str(), shrink_size);
            //make the new_right as the right of the parent
            if (to_delete->left == nullptr)
            {
                //load the new_right from file
                file_node* new_left = new file_node;
                read_node(to_string(new_left_pos).c_str(), new_left);
                mem_node* mem_new_left = new mem_node(new_left);
                to_delete->left = mem_new_left;
            }
            mem_par->left = to_delete->left;
            //write the new_right in-file node to be in-file parent->right
            long extend_pos, extend_size;
            extend_pos = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + tmp->left_len + 3;
            extend_size = string(mem_par->left->file_pos).size();
            extend_file_from_pos(to_string(extend_pos).c_str(), extend_size);
            //put the new right pos in right position in file
            input_file.seekp(extend_pos-1, ios::end);
            input_file.put(extend_size);
            input_file.write(mem_par->left->file_pos, extend_size);
            delete to_delete;

        }
    }
    
    void remove_key(string& key){
        mem_node* last_search = find_key(key);
        if(key.compare(last_search->key) == 0){
            remove(key, last_search, mem_parent);
            mem_parent = nullptr;//make it null again for subsequent remove operations
            decrement_num_nodes();
            if(num_nodes == 0)
                prepare_file_no_nodes();
        
        }else{
            cout<<"Non Existing key"<<endl;
        }
    }
    void prepare_file_no_nodes(){
        mem_root = nullptr;
        //TODO: resize input_file
    }
    ~file_handler(){
        input_file.flush();
        input_file.close();
    }
};

int main(int argc, char const *argv[])
{
    file_handler fh;
    cout<<"after constructing fh"<<endl;
    if (fh.mem_root == nullptr)
    {
        cout<<"mem root = nullptr"<<endl;
    }else{
        cout<<"mem root != nullptr"<<endl;
    }
    string k = "key4";
    string v = "value4";
    cout<<"before insert"<<endl;
    fh.insert_key(k, v);
    cout<<"after insert"<<endl;
    cout<<fh.find_key(k)->key<<endl;
    cout<<fh.find_key(k)->value<<endl;
    return 0;
}
