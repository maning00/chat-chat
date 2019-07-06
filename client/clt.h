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
#include <panel.h>
/*

message Proto_msg{
        int32 flag=1;      //1为发送登陆验证，2为发送给客户端,3为请求好友列表，询问是否在线
        int32 towhom=2;   //接受者
        string info=3;
};


explian                 flag                         toWhom                    info
登陆验证                   1                       1成功 0失败/下线                  NULL
请求在线用户列表            2
发给其他用户                3


 */
using namespace std;
using namespace protomsg::protobuf;
const int BUFFSIZE = 128;
const int LOGIN_FLAG = 1;
const int RQST_ONLINE_FLAG = 2;

const string END_FLAG="^";
const string NUMBER_FLAG="#";

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
    void login();
    void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
    void init_wins(WINDOW **wins, int n);
    void Init_ChatUI();
    void Reqst_Online_List();
    void Client_driver(Proto_msg &msg);
    void Print_Prompt(const char* str);
    bool Add_Friend(std::string frid,bool status);
    void set_status(bool on);
    void Send(Proto_msg &msg);
    void Receive(Proto_msg &msg);
    void set_serversock(int sockfd){serversock = sockfd;};
    bool power;
    SharedQueue<Proto_msg> send_queue;
    SharedQueue<Proto_msg> recv_queue;
    int serversock;
    bool online_status;
    
private:
    vector<my_friend> friends;


};


#endif //UNTITLED1_CLT_H
