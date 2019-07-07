#include "account.hpp"
#include <ncurses.h>

accountList::accountList(std::vector<User> &acclis)
{
    usr=acclis;
}

accountList::accountList()
{
    auto* lst = new std::vector<User>();
    usr = *lst;
}

User *accountList::Registration(std::string name,std::string passw){//Registration
    User *newusr=new User(name,passw);
    usr.push_back(*newusr);
    return newusr;
}


bool accountList::Deleteacct(std::string &del){//Delete account
    for(int k=0;k<usr.size();k++)
    {
    if(usr[k].name==del)
    {
        usr.erase(usr.begin()+k);
        return true;
    }
    }
    return false;
}

bool accountList::isLegal(std::string &accoum,std::string &passw){//check password is right or not
    User *nusr = new User(accoum,passw);
    for(int i=0;i<usr.size();i++)
    {
    if(usr[i]==*nusr)
    return true;
    }
    return false;
}

User *accountList::findusr(std::string m)
{
    if(usr.size()>0) {
        for (int j = 0; j < usr.size(); j++) {
            if (usr[j].name == m) {
                return &usr[j];
            }
        }
        return nullptr;
    }
    return nullptr;
}

OnlineUsr_List::OnlineUsr_List()
{
    auto* nlst=new std::vector<onlineuser>();
    olusr = *nlst;
}

OnlineUsr_List::OnlineUsr_List(std::vector<onlineuser> &olst)
{
    olusr=olst;
}

int OnlineUsr_List::findusr(std::string sb)
{
    for(auto l:olusr)
    {
        if(l.name==sb)
        {
            return l.sock;
        }
    }
    return -1;
}



void OnlineUsr_List::addusr(std::string &accnum, int sock)
{
    onlineuser *sob = new onlineuser(accnum,sock);
    olusr.push_back(*sob);
}

void OnlineUsr_List::addusr(onlineuser &usr)
{
    olusr.push_back(usr);
}

void OnlineUsr_List::removusr(std::string usrnum)
{
    for(int n=0;n<olusr.size();n++)
    {
        if(olusr[n].name==usrnum)
            olusr.erase(olusr.begin()+n);
    }
}

std::string OnlineUsr_List::findusrbysock(int sockfd)//find user by sockfd
{
    for(auto const &b : olusr)
    {
        if(b.sock==sockfd)
            return b.name;
    }
    return nullptr;
}

onlineuser::onlineuser(std::string accn,int sockfd)
{
    name=accn;
    sock=sockfd;
}

User::User(std::string &nic,std::string &pass)
{
    name=nic;
    passwd=pass;
    Friend_list = *(new std::vector<my_friend>);
}

bool User::Add_Friend(std::string frid) {
    my_friend newfriend;
    newfriend.name=frid;
    newfriend.is_online= false;
    Friend_list.push_back(newfriend);
}

void User::set_status(bool on) {
    status = on;
}

void User::set_friend_status(std::string fname, bool status) {
    for(auto& i:Friend_list)
    {
        if(i.name==fname)
            i.is_online=status;
    }
    /*for(int i=0;i<Friend_list.size();i++)
    {
        if(Friend_list[i].name==fname)
            Friend_list[i].is_online=status;
    }*/
}

bool User::operator==(const User &usrr) {
    if(this->name==usrr.name && this->passwd==usrr.passwd)
        return true;
    else
        return false;
}