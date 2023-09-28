#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/*
  a node in the file is at least 32 bytes
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+++
            KEY 4 bytes ||  VALUE 64-bytes +
-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++
 LEFT 4-bytes  ||  RIGHT 4-bytes +
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

using namespace std;
#define file_name   "redis"
#define value_max    64

//node object is 20-bytes long
class node
{
public:
    int pos;
    int left_pos, right_pos, height; //file positions of the left node and right node
    node* left;
    node* right;
    string key;//4-bytes string
    string value;
    node(string k, string v, int l, int r, int p);
    
};

class file_opener
{
public:
    fstream stream;
    node* root;//references whole root node
    file_opener();
    int get_root();
    void populate_node(int pos, node** n);
    void insert(string key, string value, node* cur);
    bool exists(string key, node* cur);
    int write_node(string key, string value, int left_pos, int right_pos);
    void change_value(string v, node* cur);
    ~file_opener();
};
