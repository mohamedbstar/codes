#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

#define file_name   "redis"
#define MAX_KEY     10
#define MAX_VAL     64
/*
*   KEY LEN 1-BYTE | KEY 10 bytes | VAL LEN 1-BYTE | VALUE 64-bytes | LEFT 4-bytes | RIGHT 4-bytes | POS 4-bytes
*/

/*
*       TO DO: keep tracking of deleted nodes and their positions to use then for subsequent inserts
*/

class file_node{
public:
    unsigned char key_len;//hold actual number of bytes written as key out of MAX_KEY bytes
    char key[MAX_KEY];
    unsigned char val_len;//hold actual number of bytes written as value out of MAX_VAL bytes
    char value[MAX_VAL];
    unsigned int left;
    unsigned int right;
    unsigned int pos;
    
};

class mem_node{
public:
    char key_tmp[MAX_KEY];
    char val_tmp[MAX_VAL];
    string key;
    string value;
    mem_node* left;
    mem_node* right;
    unsigned int file_pos;

    mem_node(file_node* f_node)
    {
        strncpy(key_tmp, f_node->key, f_node->key_len);
        memcpy(val_tmp, f_node->value, f_node->val_len);
        key = string(key_tmp);
        value = string(val_tmp);
        left = nullptr;
        right = nullptr;
        file_pos = f_node->pos;
    }
};

class file_handler
{
public:
    
    file_node* root;
    mem_node* mem_root;
    mem_node* mem_parent;
    unsigned char num_nodes = 0;
    fstream input_file;
    file_handler(){
        fstream i_file("redis");
        input_file = move(i_file);
        mem_parent = nullptr;
        load_root();
    }
    
    void load_root(){
        //get first byte to see the number of nodes
        input_file.seekg(0,ios::beg);
        num_nodes = input_file.get();//get the on-disk number of nodes
        if (num_nodes == 0)
        {
            root = nullptr;
            mem_root = nullptr;
        }else{
            //the next byte is the key_len of root key
            root = new file_node;
            read_node(1, root);//the root node is always at offset 1 in the file
            mem_root = new mem_node(root);
            delete root;
            cout<<"mem_root->key: "<<mem_root->key<<endl;
        }
    }
    void input_file_refresh(){
        input_file.close();
        input_file.open(file_name);
    }
    long get_find_size(){
        int fd = open(file_name, O_RDWR);
        struct stat file_status;
        fstat(fd, &file_status);
        close(fd);
        return file_status.st_size;
    }
    void read_node(long pos, file_node* node){
        cout<<"Entering read_node"<<endl;
        input_file.seekg(pos);//seek to the node offset
        //now first read the length of node key
        node->key_len = input_file.get();
        //then read the next key_len bytes into node->key
        input_file.read(node->key, MAX_KEY);
        //now read the length of node val
        node->val_len = input_file.get();
        //then read the next key_len bytes into node->key
        input_file.read(node->value, MAX_VAL);
        //then read the next key_len bytes into node->key
        input_file.read((char*)&(node->left), sizeof(int));
        //then read the next key_len bytes into node->key
        input_file.read((char*)&(node->right), sizeof(int));
        //then read the next key_len bytes into node->key
        input_file.read((char*)&(node->pos), sizeof(int));
    }

    mem_node* find(string& key, mem_node* cur){
        if (key.compare(cur->key) < 0)
        {
            //key < cur->key
            //check first if cur has a left or not
            mem_parent = cur;
            if (cur->left == nullptr)
            {
                cout<<cur->key<<"->left = nullptr"<<endl;
                //check if there is a node on disk or not
                file_node* tmp = new file_node;//represents the cur file_node on disk
                read_node(cur->file_pos, tmp);
                if(tmp->left == 0){
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
                cout<<cur->key<<"->right = nullptr"<<endl;

                //check if there is a node on disk or not
                file_node* tmp = new file_node;//represents the cur file_node on disk
                cout<<cur->key<<"->file_pos = "<<cur->file_pos<<endl;
                read_node(cur->file_pos, tmp);
                if(tmp->right == 0){
                    cout<<"tmp->right_len = 1"<<endl;
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
        cout<<"entering find "<<key<<" from find_key"<<endl;
        return find(key, mem_root);
    }
    void write_node(file_node* f_cihld){

        input_file.put(f_cihld->key_len);
        input_file.write(f_cihld->key, f_cihld->key_len);
        //complate the MAX_KEY by 0's
        for(int i = 0; i < (MAX_KEY - f_cihld->key_len); i++){
            input_file.put('0');
        }

        input_file.put(f_cihld->val_len);
        input_file.write(f_cihld->value, f_cihld->val_len);
        for(int i = 0; i < (MAX_VAL - f_cihld->val_len); i++){
            input_file.put('0');
        }


        input_file.write((char*) &(f_cihld->left), sizeof(int));

        input_file.write((char*) &(f_cihld->right), sizeof(int));

        input_file.write((char*) &(f_cihld->pos), sizeof(int));

        input_file.flush();//to synchronize writing
    }
    
    file_node* create_new_file_node(string& key, string& value){
        file_node* f_child = new file_node;
        f_child->key_len = key.size();
        strcpy(f_child->key, key.c_str());
        f_child->val_len = value.size();
        strcpy(f_child->value , value.c_str());
        f_child->left = 0;
        f_child->right = 0;
        f_child->pos =  0;
        return f_child;
    }
    void insert_left(string& key, string& value,mem_node* mem_par){
        //first create a new file_node for the child
        file_node* new_child = create_new_file_node(key, value);
        //then write the node at the end of the file
        input_file.seekp(0, ios::end);
        int child_offset = input_file.tellp();
        new_child->pos = child_offset;
        write_node(new_child);

        //assign new_child as left to the mem_par in disk and in memory
        mem_node* new_child_mem = new mem_node(new_child);
        mem_par->left = new_child_mem;

        int par_file_left_ptr_pos = mem_par->file_pos + MAX_KEY + MAX_VAL + 2;
        input_file.seekp(par_file_left_ptr_pos, ios::beg);
        input_file.write((char*)&child_offset, sizeof(int));

        delete new_child;//no longer needs the on-disk node of the new_child to be in memory anymore
    }
    void insert_right(string& key, string& value,mem_node* mem_par){
        //first create a new file_node for the child
        file_node* new_child = create_new_file_node(key, value);
        //then write the node at the end of the file
        input_file.seekp(0, ios::end);
        int child_offset = input_file.tellp();
        new_child->pos = child_offset;
        write_node(new_child);

        //assign new_child as right to the mem_par in disk and in memory
        mem_node* new_child_mem = new mem_node(new_child);
        mem_par->right = new_child_mem;

        int par_file_right_ptr_pos = mem_par->file_pos + MAX_KEY + MAX_VAL + sizeof(int) + 2;
        input_file.seekp(par_file_right_ptr_pos, ios::beg);
        input_file.write((char*)&child_offset, sizeof(int));

        delete new_child;//no longer needs the on-disk node of the new_child to be in memory anymore
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
        num_nodes++;
    }
    void decrement_num_nodes(){
        num_nodes--;
    }
    void insert_root(string& key, string& value){
        input_file.seekp(1, ios::beg);
        root = new file_node;
        root->key_len = key.size();
        strcpy(root->key, key.c_str());
        root->val_len = value.size();
        strcpy(root->value, value.c_str());
        root->left = 0;
        root->right = 0;
        root->pos = 1;
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
        if (mem_root == nullptr)
        {
            insert_root(key, value);
            write_node(root);
            mem_root = new mem_node(root);
        }else{
            mem_node* last_search = find_key(key);//the node->key == key or node is to be a parent of key
            if(last_search->key.compare(key) == 0){
                cout<<"Already Existing key"<<endl;
                return;
            }
            insert(key, value, last_search);
        }
        increment_num_nodes();
    }

    ~file_handler(){
        input_file.seekp(0, ios::beg);
        input_file.put(num_nodes);
        input_file.flush();
        input_file.close();
    }
};

int main(int argc, char const *argv[])
{
   
    
    return 0;
}
