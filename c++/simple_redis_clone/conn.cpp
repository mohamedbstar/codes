#include "conn.h"

Connection::Connection(){
    fd = -1;
    status = 0;
    msg_buf_max = 4096;
    read_buf = new char[msg_buf_max];
    write_buf = new char[msg_buf_max];
    read_buf_size = 0;
    write_buf_size = 0;
    num_bytes_written = 0;
}
void Connection::set_non_blocking(){
    int res, flags;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        perror("fcntl");
        return;
    }
    flags |= O_NONBLOCK;
    res = fcntl(fd, F_SETFL, flags);
    if (res < 0)
    {
        perror("fcntl");
        return;
    }
}

int Connection::read_msg(){
    //fill the read_buf
    int num_read = 0;
    int read_so_far = 0;
    while ((num_read = read(this->fd, (this->read_buf+num_read), msg_buf_max-read_so_far)) > 0)
    {
        read_so_far += num_read;
    }
    if (num_read == 0)
    {
        /* do nothing as its EOF */
        read_buf[read_so_far-1] = '\0';
        read_buf_size = read_so_far;
        return 0;
    }else{
        //error occurred
        perror("read_msg");
        return -1;
    }
}

void Connection::write_msg(string msg){
    int num_written = 0;
    int msg_len = msg.length();
    int written_so_far = 0;
    strcpy(write_buf, msg.c_str());
    while ( (num_written = write( fd, write_buf, msg_len - written_so_far )) > 0)
    {
        written_so_far += num_written;
    }
    if (num_written < 0)
    {
        perror("write_msg");
    }
    
}

void Connection::io(int lfd,const vector<Connection*>& connections){
    if(connections[fd]->status == STATUS_REQ){
        handle_state_req();
    }else if(connections[fd]->status == STATUS_RES){
        handle_state_res();
    }else{
        assert(0);//not anticipated situation
    }
}
void Connection::handle_state_req(){
    
    int res = read_msg();
    if(res)
        return;
    //analyze the msg
    analyze_request();
    /* TODO: do something to tell the client that you got the msg */
    write_msg("Got Your message as " + string(read_buf));
}
void Connection::analyze_request(){
    //the message is in read_buf
    int msg_len;//first 4 bytes in the read_buf
    memcpy(&msg_len, read_buf, 4);
    msg_len = atoi((char*) &msg_len);

    //now msg_len is the length of the message
    //get the command and keys and values
    //determine what to do forthe user and send him a message
    status = STATUS_RES;
    handle_state_res();
}

void Connection::handle_state_res(){
    //handle res
    //here we will reply to the sender with some reply
    //write(fd, ("[Received]" + string(read_buf)).c_str(),("[Received]" + string(read_buf)).size());
    write_msg("[Received]" + string(read_buf));
}
Connection::~Connection(){
    delete read_buf;
    delete write_buf;
}