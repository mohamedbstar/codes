#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdlib>

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
    char key[KEY_LEN];
    signed char val_len;//hold actual number of bytes written as value out of MAX_VAL bytes
    char value[VAL_len];
    signed char left_len;
    char left[POS_LEN];
    signed char right_len;
    char right[POS_LEN];
    signed char pos_len;
    char pos[POS_LEN];
};

class file_handler
{
public:
    int fd;
    void* mapping_pos;
    struct stat fd_stat;
    int root_pos_len;
    file_node* root;
    long mapping_size;
    long initial_file_size;

    file_handler(){
        fd = open(file_name, O_RDWR);
        if (fd < 0)
        {
            perror("open");
            exit(0);
        }
        //get file status to get its size
        fstat(fd, &fd_stat);

        mapping_pos = mmap(NULL,fd_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,fd, 0 );
        if(mapping_pos == MAP_FAILED){
            perror("mmap");
            exit(0);
        }
        //get the mapping size
        mapping_size = fd_stat.st_size;
        initial_file_size = fd_stat.st_size;
        //now we have the file mapped at memory
        load_root();
    }
    void create_file(){
        int f_fd = open(file_name, O_RDWR);
        if (f_fd < 0)
        {
            perror("open at create file");
            exit(0);
        }
        //write fist 10 bytes as "0"
        for (size_t i = 0; i < 10; i++)
        {
            write(f_fd, "0", 1);
        }
        char c = '\0';
        lseek(fd, 4096, SEEK_END);
        write(fd, &c, 1);
    }
    void load_root(){
        //first 10 bytes hold the position of root node inside the file /*FIRST 10 BYTES IN THE MAPPING*/
        char s_pos[MAX_POS];
        long i_pos;
        memcpy(s_pos, mapping_pos, MAX_POS);
        i_pos = stoi(s_pos);
        if(i_pos == 0){
            root = nullptr;
        }else{
            root = (file_node*)(mapping_pos + i_pos);
        }
    }
    void print_nodes(file_node* cur){
        if (root == nullptr)
        {
            cout<<"No nodes to print"<<endl;
            return;
        }
        if(cur == nullptr){
            return;
        }
        if(stoi(cur->left) != 0){
            //there is a left node
            file_node* left_node = (file_node*)(mapping_pos + stoi(cur->left));
            print_nodes(left_node);
        }
        cout<<cur->key<<endl;
        if(stoi(cur->right) != 0){
            file_node* right_node = (file_node*) (mapping_pos + stoi(cur->right));
            print_nodes(right_node);
        }
    }
    bool find(const char key[], file_node* cur, int len){
        if (cur == nullptr)
        {
            return false;
        }
        if(strncmp(key, cur->key, len) < 0){
            //key is less than cur->key
            file_node* left_node = stoi(cur->left) != 0? (file_node*)(mapping_pos + stoi(cur->left)) : nullptr;
            return find(key, left_node, len);
        }else if (strncmp(key, cur->key, len) > 0)
        {
            /* key > cur->key */
            file_node* right_node = stoi(cur->right) != 0? (file_node*)(mapping_pos + stoi(cur->right)) : nullptr;
            return find(key, right_node, len);
        }else if (strncmp(key, cur->key, len) == 0)
        {
            return true;
        }
        //unexpected case
        return false;
    }
    bool find_key(char key[], file_node* cur){
        //append necessary characters to key to match stored key format
        string k(key);
        int len = k.size();
        if (len < MAX_KEY)
        {
            for(int i = 0; i < (MAX_KEY - len); i++){
                k += "0";
            }
        }
        return find(k.c_str(), cur, len);
    }
    void write_node(long offset, signed char key_len, signed char val_len, signed char left_len, signed char right_len, signed char pos_len, string key, string value, string left, string right, string pos){
        void* write_offset_key_len = (void*)(mapping_pos + offset);//here key_len
        void* write_offset_key = (void*)(write_offset_key_len + 1);
        
        void* write_offset_val_len = (void*)(write_offset_key + KEY_LEN);  //here val_len
        void* write_offset_val = (void*)(write_offset_val_len + 1);
        
        void* write_offset_left_len = (write_offset_val + VAL_len);//here left len
        void* write_offset_left = (void*)(write_offset_left_len + 1);

        void* write_offset_right_len = (write_offset_left + POS_LEN);
        void* write_offset_right = (void*)(write_offset_right_len + 1);
        
        void* write_offset_pos_len = (void*) (write_offset_right + POS_LEN);
        void* write_offset_pos = (void*)(write_offset_pos_len + 1);
        
        memcpy(write_offset_key_len, &key_len, 1);
        memcpy(write_offset_key, key.c_str(), KEY_LEN);
        
        memcpy(write_offset_val_len, &val_len, 1);
        memcpy(write_offset_val , value.c_str(), VAL_len);
        
        memcpy(write_offset_left_len, &left_len, 1);
        memcpy(write_offset_left , left.c_str(), POS_LEN);
        
        memcpy(write_offset_right_len, &right_len, 1);
        memcpy(write_offset_right , right.c_str(), POS_LEN);
        
        memcpy(write_offset_pos_len, &pos_len, 1);
        memcpy(write_offset_pos , pos.c_str(), POS_LEN);
    }
    long get_next_empty_pos_for_root(){
        //TODO: limit the file size to 10 digits as max
        //resize the mapping
        /*file_node dummy_node;
        int old_pos = mapping_size;
        void* new_pos = mremap(mapping_pos, mapping_size, mapping_size+sizeof(dummy_node), MREMAP_MAYMOVE);
        mapping_pos = new_pos;
        mapping_size += sizeof(dummy_node);
        return old_pos;*/
        return 11;
    }
    void insert_util(char key[], char value[], file_node* cur){
    }
    void insert(char key[], char value[]){
        if (find_key(key, root) == true)
        {
            cout<<"Already Existing key"<<endl;
            return;
        }
        if (root == nullptr)
        {
            cout<<"in insert root = nullptr"<<endl;
            //empty file -> create the root node
            long new_pos = get_next_empty_pos_for_root();
            string k(key);
            string v(value);
            string left;
            string right;
            //set new_pos to a suitable format for storage
            char dummy[MAX_POS];
            string pos = to_string(new_pos);
            int pos_len = pos.size();
            if(pos_len < MAX_POS){
                for(int i = 0; i < MAX_POS-pos_len ;i++ ){
                    pos += "0";
                }
            }
            pos += '\0';
            //set the key and value to suitable formats for storage
            int k_len = k.size();
            if (k_len < MAX_KEY)
            {
                for(int i = 0; i < (MAX_KEY - k_len); i++){
                    k += "0";
                }
            }
            k += '\0';

            int v_len = v.size();
            if (v_len < MAX_VAL)
            {
                for (size_t i = 0; i < (MAX_VAL - v_len); i++)
                {
                    v += "0";
                }
            }
            v += '\0';
            //set left and right = 0
            for(int i = 0; i < MAX_POS; i++){
                left += "0";
                right += "0";
            }
            left += '\0';
            right += '\0';
            signed char root_pos_l = 1;
            write_node(new_pos, k_len, v_len, 0,0,0,k, v, left, right, pos);
            //update the root position
            memcpy(mapping_pos+1, pos.c_str(), 10);
            memcpy(mapping_pos, &root_pos_l, 1);

            cout<<"after write_node"<<endl;
            root = (file_node*)(mapping_pos + new_pos);
            cout<<"root is "<<root->key<<" : "<<root->value<<endl;
            cout<<"key len: "<<strlen(root->key)<<endl;
            cout<<"val len: "<<strlen(root->value)<<endl;
            return;
        }
        cout<<"in insert before insert_util"<<endl;
        insert_util(key, value, root);
    }
    void write_all(char* to_write, int size){
        int written_so_far = 0;
        int num_written;
        while((num_written = write(fd, to_write+written_so_far, size - written_so_far)) > 0 && (size - written_so_far) > 0){
            written_so_far += num_written;
        }
    }
    ~file_handler(){
        fstat(fd, &fd_stat);
        //write mapped file into data file
        write_all((char*)mapping_pos, mapping_size);
        //write(fd, mapping_pos, mapping_size);
        fsync(fd);//wait until data is written to desk
        munmap(mapping_pos, fd_stat.st_size);
        close(fd);
    }
};

int main(int argc, char const *argv[])
{
    file_handler fo;
    if (fo.root == nullptr)
    {
        cout<<"root is nullptr"<<endl;
    }
    fo.insert("key1", "value1");
    return 0;
}
