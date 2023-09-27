#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <assert.h>
#include <string.h>

using namespace std;

enum {STATUS_REQ=0, STATUS_RES=1, STATUS_END=2}; //connection status



class Connection
{
public:
    int fd;
    int status;
    int msg_buf_max;
    char* read_buf;
    char* write_buf;
    int read_buf_size;
    int write_buf_size;
    int num_bytes_written;
    Connection();
    void set_non_blocking();
    int read_msg();
    void write_msg(string msg);
    void io(int lfd,const vector<Connection*>& connections);
    void handle_state_req();
    void handle_state_res();
    void analyze_request();
    ~Connection();
};