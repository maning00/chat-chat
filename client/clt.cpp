//
//  clt.cpp
//  chat-client
//
//  Created by 马宁 on 2019/6/27.
//  Copyright © 2019 马宁. All rights reserved.
//


#include "clt.h"
#define PWDSIZE 20

SharedQueue<Proto_msg> send_queue;
static void *Send_Thread(client& clt);
static void *Recv_Thread(client& clt);


client::client()
{
    //SharedQueue<Proto_msg> rcv;
    auto *na = new std::vector<my_friend>();
    online_status = false;
    //recv_queue = rcv;
    friends=*na;
    serversock=0;
    
}


int client::connectosvr(char *ipadd, int port)
{
    
    struct sockaddr_in mysock;
    mysock.sin_family = AF_INET;
    mysock.sin_addr.s_addr=inet_addr(ipadd);
    mysock.sin_port=htons(port);
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if( sockfd == -1)
        perror("socket create failed");
    if(connect(sockfd,(struct sockaddr *)&mysock,sizeof(struct sockaddr))==-1)
    {
        perror("connect error");
        endwin();
        return -1;
    }
    power=TRUE;
    set_serversock(sockfd);
    
    return 1;

}
bool swtch = false;
int client::login(string username,string password)
{
    t[0]=thread(Send_Thread,ref(*this));
    t[1]=thread(Recv_Thread,ref(*this));
    Setmyname(username);
    Proto_msg ident;
    ident.set_flag(LOGIN_FLAG);
    ident.set_towhom(username);
    ident.set_info(password);
    Send(ident);
    while(!online_status)
    {
        //cout<<"sleep"<<endl;
        usleep(200000);
    }
    return 1;
}

void client::Send(Proto_msg &msg) {
    send_queue.push_back(msg);
    //cout<<"push_back "<<msg.flag()<<endl<<msg.towhom()<<endl<<msg.info()<<endl;
}

void client::Sendto_User(string username, string message)
{
    string forsend1,forsend2;
    unsigned int length1 = myname.size();
    unsigned int length2 = message.size();
    if(length1<10)
    {
        forsend1 = "0";
        forsend1 += std::to_string(length1);
        forsend1 += myname;
    }
    else
    {
        string len =to_string(length1);
        forsend1.append(len);
        forsend1 += myname;
    }
    forsend1 += "#";
    if(length2<10)
    {
        forsend2="0";
        forsend2 += std::to_string(length2);
        forsend2 += message;
    }
    else
    {
        string len =to_string(length2);
        forsend2.append(len);
        forsend2 += message;
    }
    forsend2 += "^";
    forsend1.append(forsend2);
    Proto_msg snd;
    snd.set_flag(CHAT_TEXT_FLAG);
    snd.set_towhom(username);
    snd.set_info(forsend1);
    Send(snd);
}



void client::Reqst_Online_List()
{
    Proto_msg rqst;
    rqst.set_flag(GETOL_FLAG);
    rqst.set_towhom(myname);
    Send(rqst);
}





void client::set_status(bool on) {
    online_status=on;
}

bool client::Add_Friend(std::string frid,bool status) {
    my_friend newfriend;
    newfriend.name = frid;
    newfriend.is_online = status;
    friends.push_back(newfriend);

    return true;
}



void client::Chat_Text_Handle(Proto_msg &msg)
{
    const string &originmsg = msg.info();
    int namelen = atoi(originmsg.substr(0,2).c_str());
    int msglen = atoi(originmsg.substr(2+namelen,2).c_str());
    string sender_name =originmsg.substr(2,namelen);
    string messg = originmsg.substr(4+namelen,msglen);
    if(sendto == sender_name) {
        if(new_message_sign)
            usleep(10000);
        else {
            FILE *fp;
            sender_name += ".txt";    //以每位好友名字创建一个聊天记录临时文件
            if ((fp = fopen(sender_name.c_str(), "wt+")) == nullptr) //打开文件string，不存在则自动创建
            {
                sleep(1);
            }
            time_t t;
            t = time(nullptr);
            std::string tt = ctime(&t);
            std::string st = tt.substr(11, 8);
            std::string ch = ":   ";
            st += ch;
            st += messg;
            fwrite(st.c_str(), sizeof(st), 1, fp);    //size of///sizeof .c_str()?

            fclose(fp);
            new_message_sign = true;
        }
    } else {
        tmp temp;
        temp.name = sender_name;
        temp.content = messg;
        waitting_response.push_back(temp);     //有其他用户发来消息
    }
    /*if(new_message_sign)
    {
        if(sendto !=sender_name)
        {
            tmp temp;
            temp.name = sender_name;
            temp.content = messg;
            waitting_response.push(temp);     //有其他用户发来消息
        }

    } else {
        FILE *fp;
        sender_name += ".txt";    //以每位好友名字创建一个聊天记录临时文件
        if ((fp = fopen(sender_name.c_str(), "wt+")) == nullptr) //打开文件string，不存在则自动创建
        {
            sleep(1);
        }
        time_t t;
        t = time(nullptr);
        std::string tt = ctime(&t);
        std::string st = tt.substr(11, 8);
        std::string ch = ":   ";
        st += ch;
        st += messg;
        fwrite(st.c_str(), sizeof(st), 1, fp);    //size of///sizeof .c_str()?
        //fwrite(ch,sizeof(ch),1,fp);
        //fwrite(messg.c_str(),sizeof(messg),1,fp);
        fclose(fp);
        new_message_sign = true;
    }*/
}

void client::Decode_OL_List(string mesg)
{
    string strbuf="";
    string res;
    int i=0;
    friends.clear();
    while(strbuf!="#")
    {
        int len = atoi(mesg.substr(i,2).c_str());
        //cout<<"len="<<len<<endl<<"mesg is"<<mesg<<endl;
        i+=2;
        string newfrid=mesg.substr(i,len);
        if(newfrid!=myname) {
            Add_Friend(newfrid, true);
        }
        i+=len;
        strbuf = mesg[i];
    }
    online_remider = true;
    //swtch=true;
}

void client::Client_driver(Proto_msg &msg)
{
    int ch = msg.flag();
    string twh = msg.towhom();
    string ifo = msg.info();
    switch(ch)
    {
        case LOGIN_FLAG:
        {
            cout<<"Received :  "<<msg.flag()<<endl<<msg.towhom()<<endl<<msg.info()<<endl;
            if(atoi(ifo.c_str())==1)
                online_status=true;
            break;
        }
        case GETOL_FLAG:
        {
            Decode_OL_List(ifo);
            break;
        }
        case CHAT_TEXT_FLAG:
        {
            Chat_Text_Handle(msg);
            break;
        }
        default:
        {
            //msg.set_flag(ERR_UNKNOWN_FLAG);
            //cout<<"Got an Unknown message: they are"<<endl<<msg.flag()<<endl<<msg.towhom()<<endl<<msg.info()<<endl;
            break;
        }
    }
}


static void *Send_Thread(client& clt)
{
    while (clt.power)
    {
        if(!send_queue.empty())
        {
            Proto_msg presend=send_queue.front();
            //cout<<"send "<<presend.flag()<<endl<<presend.towhom()<<endl<<presend.info()<<endl;
            char buff[BUFFSIZE];
            presend.SerializePartialToArray(buff,BUFFSIZE);
            if(send(clt.Get_serversock(),buff,strlen(buff),0) < 0)
            {
                cout<<"send failed..."<<endl;
            }

            send_queue.pop_front();
        }
        usleep(100000);
    }
    pthread_exit(nullptr);
}
static void *Recv_Thread(client& clt)
{
    while(clt.power) {
        Proto_msg recv_msg;
        char buf1[BUFFSIZE];
        memset(&buf1, 0, BUFFSIZE * sizeof(char));
        if (recv(clt.Get_serversock(), buf1, sizeof(buf1), 0) < 0)
            cout<<"Receive failed..."<<endl;
        recv_msg.ParseFromArray(buf1, BUFFSIZE);
        //cout<<"Received "<<recv_msg.flag()<<endl<<recv_msg.towhom()<<endl<<"and "<<recv_msg.info()<<endl;
        clt.Client_driver(recv_msg);
    }
    pthread_exit(nullptr);
}

void client::Exit() {
    Proto_msg ext;
    ext.set_flag(EXIT_FLAG);
    ext.set_towhom(myname);
    Send(ext);
    sleep(1);
    power= false;
    close(serversock);
    exit(0);
}

vector<string> client::Get_Friend_List() {
    if(friends.empty())
    {
        vector<string> loading;
        string str1="Oops, There's no one";
        loading.push_back(str1);
        return loading;
    }
    else
    {
        vector<string> choices;
        int size = friends.size();
        for(int i=0;i<size;i++)
        {
            choices.push_back(friends[i].name);
        }
        return choices;
    }
}
