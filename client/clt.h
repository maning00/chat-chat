#ifndef UNTITLED1_CLT_H
#define UNTITLED1_CLT_H
//
//  clt.hpp
//  chat-client
//
//  Created by 马宁 on 2019/6/27.
//  Copyright © 2019 马宁. All rights reserved.
//
#include<sys/types.h>
//#include <sys/stat.h>
#include<sys/socket.h>
#include<stdio.h>
#include<cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include<unistd.h>
#include <string>
#include "protomsg.pb.h"
#include <cstring>
#include <sstream>
#include <iostream>
#include <list>
#include <vector>
#include <thread>
#include <fstream>

//#include <dirent.h>
#include <mutex>
#include <queue>
#include "SharedQueue.h"
#include <menu.h>

/*

message Proto_msg{
        int32 flag=1;      //1为发送登陆验证，2为发送给客户端,3为请求好友列表，询问是否在线
        string towhom=2;   //接受者
        string info=3;
};

客户端发送：
explian                 flag                         toWhom                    info
登陆验证                   1                           用户名                     密码
请求在线用户列表            2                           自己的用户名
发给其他用户               3                            目的用户名             2位数+自己用户名#+2位数+消息^
错误消息                   4                          usrname                 消息内容
下线                       5                            自己用户名
 */
using namespace std;
using namespace protomsg::protobuf;
const int BUFFSIZE = 128;




enum Flag{
    LOGIN_FLAG =1,
    GETOL_FLAG=2,
    CHAT_TEXT_FLAG=3,
    ERR_UNKNOWN_FLAG=4,
    EXIT_FLAG=5
};

class my_friend
{
public:
    bool is_online;
    string name;

};
class client{
public:
    explicit client();
    int connectosvr(char *ipadd,int port);
    int login(string username,string password);
    void Reqst_Online_List();
    void Client_driver(Proto_msg &msg);
    void Decode_OL_List(string mesg);
    void Chat_Text_Handle(Proto_msg &msg);
    bool Add_Friend(std::string frid,bool status);
    void set_status(bool on);
    void Sendto_User(string username,string message);
    void Send(Proto_msg &msg);
    void set_serversock(int sockfd){serversock = sockfd;};
    bool power;
    string Getmyname(){return myname;}
    void Setmyname(string nnm){myname = nnm;}
    int serversock;
    bool online_status;
    void Exit();
    bool friends_empty(){ return friends.empty();};
    vector<string> Get_Friend_List();
    void Set_Sendto(string &name){sendto = name;}
    string Get_Sendto(){ return sendto;}
    bool online_remider;
    
private:
    vector<my_friend> friends;
    string myname;
    string sendto;
    thread t[2];

};


#endif //UNTITLED1_CLT_H
