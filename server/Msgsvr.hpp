#ifndef MSGSVR_HPP
#define MSGSVR_HPP

#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<assert.h>
#include<pthread.h>
#include "protomsg.pb.h"
#include <time.h>
#include <string.h>
#include<unistd.h>
#include <vector>
#include "SharedQueue.h"
#include "account.hpp"
using namespace std;
using namespace protomsg::protobuf;
const int BUFFSIZE = 128;
typedef void * (*THREADFUNCPTR)(void *);
class msgsvr{
    public:
    void Init();
    ~msgsvr() {delete &onlinelst;delete &acclist;};
    int bindipport(char *ip,int port);  //bind ip and port  return sockfd
    int acpt(int sockfd);        //accept connections
    int identfy(int nsock);    //identify user,illegal return nsock, not illegal return -1
    void Send(Proto_msg &msg);
    void Receive(Proto_msg &msg);
    void login(onlineuser *usr);
    void logoff(std::string usrnum);
    OnlineUsr_List GetOL_List(){return onlinelst;}
    accountList GetACC_List(){return acclist;}
    bool power;
    vector<Proto_msg> send_queue;
    vector<Proto_msg> recv_queue;
    


private:
    OnlineUsr_List onlinelst;
    accountList acclist;
    
};


#endif
