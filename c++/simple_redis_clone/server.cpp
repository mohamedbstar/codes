#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/poll.h>
#include "conn.h"
#include <vector>

//#include "conn.h"
using namespace std;
Connection::Connection(){
    fd = -1;
    status = STATUS_REQ;
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
    cout<<"in read_msg()"<<endl;
    //fill the read_buf
    int num_read = 0;
    int read_so_far = 0;
    while ((num_read = read(this->fd, (char*)(this->read_buf+read_so_far), msg_buf_max-read_so_far)) > 0 && read_buf_size > 0)
    {
        read_so_far += num_read;
        read_buf_size -= num_read;
        cout<<"num_read "<<num_read<<endl;
        cout<<"read_so_far: "<<read_so_far<<endl;
    }
    if (num_read == 0)
    {
        /* do nothing as its EOF */
        cout<<"num_read = 0"<<endl;
        read_buf[read_so_far-1] = '\0';
        read_buf_size = read_so_far;
        return 0;
    }
    if(num_read < 0){
        perror("read_msg");
        return -1;
    }
}
void Connection::write_msg(string msg){
    int num_written = 0;
    int msg_len = msg.length();
    int written_so_far = 0;
    strcpy(write_buf, msg.c_str());
    while ( (num_written = write( fd, write_buf, msg_len - written_so_far )) > 0 && msg_len > 0)
    {
        written_so_far += num_written;
        msg_len -= num_written;
    }
    if (num_written < 0)
    {
        perror("write_msg");
    }
    
}

void Connection::io(int lfd,const vector<Connection*>& connections){
    if(status == STATUS_REQ){
        cout<<"Entering handle_state_req()"<<endl;
        handle_state_req();
    }else if(status == STATUS_RES){
        cout<<"Entering handle_state_req()"<<endl;
        handle_state_res();
    }else{
        cout<<"in assert(0)"<<endl;
        assert(0);//not anticipated situation
    }
}
void Connection::handle_state_req(){
    cout<<"In handle_state_req()"<<endl;
    int res = read_msg();
    if(res)
        return;
    //analyze the msg
    cout<<"before analyze_request"<<endl;
    analyze_request();
    cout<<"before analyze_request"<<endl;
    /* TODO: do something to tell the client that you got the msg */
    write_msg("Got Your message as " + string(read_buf));
}
void Connection::analyze_request(){
    cout<<"in analyze request()"<<endl;
    //the message is in read_buf
    int msg_len;//first 4 bytes in the read_buf
    memcpy(&msg_len, read_buf, 4);
    msg_len = atoi((char*) &msg_len);
    read_buf_size = msg_len;
    //now msg_len is the length of the message

    //get the command and keys and values
    //determine what to do forthe user and send him a message
    status = STATUS_RES;
}

void Connection::handle_state_res(){
    cout<<"in handle_state_res()"<<endl;
    //handle res
    //here we will reply to the sender with some reply
    //write(fd, ("[Received]" + string(read_buf)).c_str(),("[Received]" + string(read_buf)).size());
    write_msg("[Received]" + string(read_buf));
}
Connection::~Connection(){
    delete read_buf;
    delete write_buf;
}


#define BUF_MAX 4096
int init_server();
void handle_one_request(int conn_fd);
void handle_connections(int fd);
void fd_set_nb(int fd);
void accept_new_connection(int fd, vector<Connection*>& connections);

int main(int argc, char const *argv[])
{
    int fd = init_server();//fd is the listening socket
    if(fd < 0){
        cout<<"Error making a server socket"<<endl;
        return 0;
    }
    cout<<"listening on "<<fd<<endl;
    vector<Connection*> connections(0, nullptr);//indexed by its fd
    vector<struct pollfd> poll_fds;//holds the fds to be watched bu poll
    struct pollfd pfd{fd, POLLIN, 0}, nfd{0};
    poll_fds.push_back(pfd);//add listen socket to be polled
    int res;
    //mainloop
    while (true)
    {
        //add all connections to be watched
        poll_fds.push_back(pfd);
        for(Connection* C : connections){
            if(C != nullptr){
                nfd.fd = C->fd;
                nfd.events = C->status == STATUS_REQ? POLLIN : POLLOUT;
                nfd.events |= POLLERR;
                poll_fds.push_back(nfd);
                nfd = {0};//reset the nfd                
            }
        }
        cout<<"num connetions "<<connections.size()<<endl;
        //then start polling
        res = poll(poll_fds.data(), poll_fds.size(), 1000);
        if(res < 0){
            perror("poll");
            continue;
        }
        
        //iterate over all events and serve each one
        cout<<"poll_fds.size "<<poll_fds.size()<<endl;
        for(int i = 1; i < poll_fds.size(); i++){
            if(poll_fds[i].revents > 0){
                cout<<"New Connectin polled"<<endl;
                //there is an event from that fd
                Connection* conn = connections[poll_fds[i].fd];//get the corresponding connection
                cout<<"before calling conn->io"<<endl;
                conn->io(fd, connections);//see what to do with this connection
                cout<<"after conn->io"<<endl;
                if(conn->status == STATUS_END){
                    //close the connection
                    connections[conn->fd] = nullptr;
                    close(conn->fd);
                    delete conn;
                }
            }
        }
        if(poll_fds[0].revents){
            cout<<"before entering accept_new"<<endl;
            accept_new_connection(fd, connections);
        }
        poll_fds.resize(0);
        //check for upcoming connetion requests
        
    }
    return 0;
}

void accept_new_connection(int fd, vector<Connection*>& connections){
    cout<<"adding new connection ";
    int new_fd;
    struct sockaddr client_addr{};
    socklen_t length;
    cout<<"fd: "<<fd<<endl;
    new_fd = accept(fd, nullptr, nullptr);
    if(new_fd < 0){
        perror("accept");
        exit(0);
    }
    
    //put the new connection into connections
    Connection* new_conn = new Connection;
    new_conn->fd = new_fd;
    if(connections.size() < new_conn->fd){
        connections.resize(new_conn->fd + 10);
    }
    connections[new_fd] = new_conn;
    cout<<"with fd "<<new_fd<<endl;
}

void fd_set_nb(int fd){
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

//returns the created socket
int init_server(){
    int fd;
    int val = 1;
    struct sockaddr_in addr_in{};
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return -1;
    }
    //then enable the socket to be used again shortly
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    //then setup the address on which we will receive requests
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &(addr_in.sin_addr));
    //then bind the socket to that address
    int ret = bind(fd, (struct sockaddr*)&addr_in, sizeof(addr_in));
    if (ret < 0)
    {
        perror("bind");
        close(fd);
        return -1;
    }
    //then listen on that socket
    ret = listen(fd, 10);//make maximum connections = 10
    if(ret < 0){
        perror("listen");
        return -1;
    }
    return fd;
}
void handle_one_request(int conn_fd){
    char buffer[4 + BUF_MAX + 10];
    errno = 0;
    int msg_len = 0;
    int num_read = read(conn_fd, buffer, 4); //first read the length of the message from the protocol
    if(errno){
        perror("read");
        return;
    }//buffer now = length of the message
    memcpy(&msg_len, buffer, 4);
    msg_len = atoi((char*) &msg_len);
    if(msg_len > BUF_MAX){
        cout<<"HUGE MSG - Try smaller one"<<endl;
        return;
    }

    num_read = read(conn_fd, &buffer[4], msg_len);
    if(num_read < 0){
        perror("read");
        return;
    }

    cout<<"[Client] "<<buffer<<endl;
    memset(buffer, 0, BUF_MAX);
}
void handle_connections(int fd){
    struct sockaddr_in client_addr{};
    socklen_t sock_len;
    int conn_fd;

    while (true)
    {
    }
    
}