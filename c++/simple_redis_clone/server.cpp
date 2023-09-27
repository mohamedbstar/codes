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
#include <sstream>

/*============================================================
****when netcat is interrupted and closed, ****num_read = 0***
=============================================================*/

//#include "conn.h"
using namespace std;
#define BUF_MAX 4096
int init_server();
void handle_one_request(int conn_fd);
void handle_connections(int fd);
void fd_set_nb(int fd);
void accept_new_connection(int fd, vector<Connection*>& connections);

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
    //fill the read_buf
    int num_read = 0;
    int read_so_far = 0;
    while ((read_buf_size-read_so_far) > 0 && (num_read = read(this->fd, (char*)(this->read_buf+read_so_far), msg_buf_max-read_so_far)) > 0)
    {
        read_so_far += num_read;
    }
    if(read_so_far == 0){
        return -2;//to indicate that client closed connection so that making its ststus STATUS_END
    }
    if(num_read < 0){
        perror("read_msg");
        return -1;
    }
    read_buf[read_so_far-1] = '\0';
    return 0;
}
void Connection::write_msg(string msg){
    int num_written = 0;
    int msg_len = write_buf_size;
    int written_so_far = 0;
    strcpy(write_buf, msg.c_str());
    while (  msg_len > 0 && (num_written = write( fd, write_buf, msg_len - written_so_far )) > 0 )
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
        handle_state_req();
    }else if(status == STATUS_RES){
        handle_state_res();
    }else{
        //cout<<"in assert(0)"<<endl;
        assert(0);//not anticipated situation
    }
}
void Connection::handle_state_req(){
    int res = analyze_request();//now i have read the whole message from connection
    //then change connection state
    status = res < 0 ? STATUS_REQ : STATUS_RES;
    if(res == 0){
        //now successfully read the whole message
        memcpy(write_buf, read_buf, read_buf_size);
        write_buf_size = read_buf_size + 11;

    }else if(res == -2){
        cout<<"res = -2"<<endl;
        status = STATUS_END;
    }
    //then reset every thing related to reading from connection
    read_buf_size = 0;
    memset(read_buf, 0, msg_buf_max);
    
}
int Connection::check_all_numerics(string word){
    for(auto i : word){
        if (!isdigit((int)i))
        {
            return -1;
        }
    }
    return 0;
}
int Connection::parse_protocol(){
    //the protocol message is inside read_buf
    string s(read_buf);
    stringstream parser(s);
    stringstream builder;
    char builder_buffer[msg_buf_max];
    int res;
    string word;

    string nstrs;
    parser >> nstrs;//word now contains nstr
    if (check_all_numerics(nstrs))
    {
        cout<<"nstrs not all digits"<<endl;
        return -1;
    }
    for(int i = 0; i < stoi(nstrs); i++){
        parser >> word;
        if(i % 2 == 0){
            //len word
            continue;
        }else{
            //str word
            //builder << word <<" ";

        }
    }
    builder.read(builder_buffer, msg_buf_max);
    cout<<"User Sent "<<builder_buffer<<endl;
    return 0;
}
int Connection::analyze_request(){
    int num_read = 0;
    int read_so_far = 0;
    int msg_len; //to hold the length of the message sent by the user
    int res;

    while((num_read = read(fd, read_buf, (4-read_so_far))) > 0 && (4-read_so_far) > 0){
        read_so_far += num_read;
    }
    
    memcpy(&msg_len, read_buf, 4);
    msg_len = atoi((char*) &msg_len);
    read_buf_size = msg_len;
    //then read the whole message sent by the user
    res = read_msg();
    if (res < 0)
    {
        perror("read_msg");
        return res;
    }
    
    /*
    the read_buf now contains a string of the form below
    +------+-----+------+-----+------+-----+-----+------+
    | nstr | len | str1 | len | str2 | ... | len | strn |
    +------+-----+------+-----+------+-----+-----+------+
    */
    res = parse_protocol();
    if(res < 0){
        cout<<"Error parsing protocol"<<endl;
        return res;
    }
    return 0;
}

void Connection::handle_state_res(){
    write_msg("[Received]" + string(write_buf)+string("\n"));
    status = STATUS_REQ;
    write_buf_size = 0;
    memset(write_buf, 0, msg_buf_max);
    status = STATUS_END;
}
Connection::~Connection(){
    delete read_buf;
    delete write_buf;
}


int main(int argc, char const *argv[])
{
    int fd = init_server();//fd is the listening socket
    if(fd < 0){
        cout<<"Error making a server socket"<<endl;
        return 0;
    }
    //cout<<"listening on "<<fd<<endl;
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
                nfd.events |= (POLLHUP | POLLERR);
                poll_fds.push_back(nfd);
                nfd = {0,0,0};//reset the nfd                
            }
        }
        //cout<<"num connetions "<<connections.size()<<endl;
        //then start polling
        res = poll(poll_fds.data(), poll_fds.size(), 1000);
        if(res < 0){
            perror("poll");
            continue;
        }
        
        //iterate over all events and serve each one
        //cout<<"poll_fds.size "<<poll_fds.size()<<endl;
        for(int i = 1; i < poll_fds.size(); i++){
            if(poll_fds[i].revents > 0){
                int r_events = poll_fds[i].revents;
                if((r_events & POLLERR) | (r_events & POLLHUP)){
                    //the connection is closed and needs to be removed
                    connections.erase(connections.begin() + poll_fds[i].fd);
                    close(poll_fds[i].fd);
                    continue; 
                }
                //there is an event from that fd
                Connection* conn = connections[poll_fds[i].fd];//get the corresponding connection
                cout<<"POLLIN from "<<poll_fds[i].fd<<endl;
                //cout<<"before calling conn->io"<<endl;
                conn->io(fd, connections);//see what to do with this connection
                //cout<<"after conn->io"<<endl;
                if(conn->status == STATUS_END){
                    //close the connection
                    connections[conn->fd] = nullptr;
                    close(conn->fd);
                    delete conn;
                }
            }
        }
        if(poll_fds[0].revents){
            //cout<<"before entering accept_new"<<endl;
            accept_new_connection(fd, connections);
        }
        poll_fds.resize(0);
        //check for upcoming connetion requests
        
    }
    return 0;
}

void accept_new_connection(int fd, vector<Connection*>& connections){
    int new_fd;
    struct sockaddr client_addr{};
    socklen_t length;
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