#ifndef MSGSVR_HPP
#define MSGSVR_HPP

#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include <cstdio>
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

/*

message Proto_msg{
        int32 flag=1;      //1为发送登陆验证，2为发送给客户端,3为请求好友列表，询问是否在线
        string towhom=2;   //接受者
        string info=3;
};

服务端发送：
explian                 flag                         toWhom                    info
登陆验证回应               1                          usrname                 1成功 0失败
 在线用户列表回应            2                          usrname              #无人 两位数+用户名#结束
用户间文字消息              3                         目的用户名             位数+发方用户名#+位数+消息^
错误消息                   4                          usrname                 消息内容    


 */
enum Flag{
    LOGIN_FLAG =1,
    GETOL_FLAG=2,
    CHAT_TEXT_FLAG=3,
    ERR_UNKNOWN_FLAG=4
};

class msgsvr{
    public:
    void Init();
    ~msgsvr() {delete &onlinelst;delete &acclist;};
    int bindipport(char *ip,int port);  //bind ip and port  return sockfd
    int acpt(int sockfd);        //accept connections
    int identfy(string name,string passwd,int sockfd);    //identify user,illegal return nsock, not illegal return -1
    void Send(Proto_msg &msg);
    void login(onlineuser *usr);
    void logoff(std::string usrnum);
    int findOnlineusr(string nnam){return onlinelst.findusr(nnam);}
    void Online_list_Response(string receiver);//receiver是在线列表接收者用户名
    OnlineUsr_List GetOL_List(){return onlinelst;}
    accountList GetACC_List(){return acclist;}
    void Message_Driver(Proto_msg &msg,int sockfd);
    bool power;
    bool isLoged_In(string name){
        if(onlinelst.findusr(name)>0)
            return true;
        else
            return false;}
    
    
    


private:
    OnlineUsr_List onlinelst;
    accountList acclist;
    
};


#endif
