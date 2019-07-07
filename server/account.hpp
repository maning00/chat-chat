#ifndef SERVER_HPP
#define SERVER_HPP

#include<stdio.h>
#include<stdlib.h>
#include <string.h>

#include <vector>
#include <sstream>
#include <iostream>
//#define PWDSIZE 20
using namespace std;
class my_friend
{
public:
    std::string name;
    bool is_online;
};

class User{
public:
    ~User(){delete &name;delete &passwd;};
    explicit User(std::string &nic,std::string &pass);
    bool operator==(const User& usrr);
    bool Add_Friend(std::string frid);
    void set_status(bool on);
    void set_friend_status(std::string fname,bool status);
    std::string name;
    bool status;
private:
    std::string passwd;
    std::vector<my_friend> Friend_list;
};

class accountList
{   //All User Info
    public:
    accountList();
    explicit accountList(std::vector<User> &acclis);
    ~accountList() {delete &usr;};
    User *Registration(std::string name,std::string passw);//register
    bool Deleteacct(std::string &del); //delete
    bool empty() {return usr.size()==0;}
    bool isLegal(std::string &accoum,std::string &passw);
    User *findusr(std::string m);

    private:
    std::vector<User> usr;
    
};

class onlineuser{   //在线用户名称及sockfd
public:
    explicit onlineuser(std::string accnum,int sock);
    ~onlineuser() {};
    bool operator==(const onlineuser &usrr){if (this->name==usrr.name) return true; else return false;} //compare name;
    std::string Encode_ol_list();
    std::string name;
    int sock;
};

class OnlineUsr_List{  //Online User List
    public:
    OnlineUsr_List();
    explicit OnlineUsr_List(std::vector<onlineuser> &olst);
    ~OnlineUsr_List() {delete &olusr;};
    void addusr(std::string &accnum,int sock);
    std::vector<onlineuser> Getolusr(){return olusr;}
    void addusr(onlineuser &usr);
    void removusr(std::string usrnum);
    int findusr(std::string sb);   //return sockfd
    std::string findusrbysock(int sockfd); //return usrnum
    
private:
    std::vector<onlineuser> olusr;
};


#endif
