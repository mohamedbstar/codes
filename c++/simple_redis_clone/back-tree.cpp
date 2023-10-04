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
    file_handler(){
        fstream i_file("redis");
        input_file = move(i_file);

        load_root();
    }
    
    void load_root(){
        //get first byte to see the number of nodes
        num_nodes = input_file.get();
        if (num_nodes == 0)
        {
            root = nullptr;
            mem_root = nullptr;
        }else{

            //get the next byte to see the number of characters used to write root address
            char num_node_off = input_file.get();
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
    void read_node(char* pos, file_node* node){

        input_file.seekg(stoi(pos));//seek to the node offset
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
                read_node(tmp->left, tmp);
                cur->left = new mem_node(tmp);
                delete tmp;
            }
            //it has a left
            return find(key, cur->left);
        }else if (key.compare(cur->key) > 0)
        {
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
        file_node* f_child = create_new_file_node(key, value);
        string tmp(mem_par->file_pos);
        string right_pos = write_file_node_as_right(tmp, f_child);

        mem_node* new_right = new mem_node(f_child);
        mem_par->right = new_right;

        //get the position on disk where we will write the right_node position
        long left_ptr_pos_on_disk = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + 2;
        char left_ptr_size_on_disk;
        input_file.seekg(left_ptr_pos_on_disk, ios::beg);
        left_ptr_size_on_disk = input_file.get();
        long right_ptr_pos_on_disk = stoi(mem_par->file_pos) + mem_par->key.size() + mem_par->value.size() + left_ptr_size_on_disk + 3;

        extend_file_from_pos(to_string(right_ptr_pos_on_disk).c_str(), right_pos.size() + 1);

        input_file.seekp(right_ptr_pos_on_disk, ios::beg);
        input_file.put(right_pos.size());
        input_file.write(right_pos.c_str(), right_pos.size());

        delete f_child;
    }    
    void insert(string& key,string& value ,mem_node* cur){
        if (cur->key.compare(key) < 0)
        {
            /* cur->key < key */
            insert_left(key, value, cur);
        }else{
            //cur->key > key
            insert_right(key, value, cur);
        }
        
    }

    void insert_key(string& key, string& value){
        if(key.size() > MAX_KEY){
            cout<<"Key Lenght > MAX_KEY"<<endl;
            return;
        }
        if(value.size() > MAX_VAL){
            cout<<"Value Lenght > MAX_VAL"<<endl;
            return;
        }
        mem_node* last_search = find_key(key);
        if (last_search->key.compare(key) == 0)
        {
            cout<<"Already Exsiting key"<<endl;
            return;
        }
        insert(key, value, last_search);
    }
    /*~file_handler(){
        fstat(fd, &fd_stat);
        //write mapped file into data file
        write_all((char*)mapping_pos, mapping_size);
        //write(fd, mapping_pos, mapping_size);
        fsync(fd);//wait until data is written to desk
        munmap(mapping_pos, fd_stat.st_size);
        close(fd);
    }*/
};

int main(int argc, char const *argv[])
{
    return 0;
}
