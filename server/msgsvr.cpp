//
//  msgsvr.cpp
//  chat-svr
//
//  Created by 马宁 on 2019/6/27.
//  Copyright © 2019 马宁. All rights reserved.
//

#include "Msgsvr.hpp"
#include <cstdlib>
#include <thread>
#include <cstring>

const int PWDSIZE=10;
const int client_number=6;
SharedQueue<Proto_msg> send_queue;
static void *Data_snd(msgsvr& svr);    //一个发送线程处理所有发送
static void *Data_Handle(int sockf,msgsvr& svr);   /* 一个客户端对应一个处理线程（收数据、处理数据） */

void msgsvr::Init(){
    printf("Welcome!\n");
    accountList aclist=accountList();
    acclist=aclist;
    OnlineUsr_List onli =OnlineUsr_List();
    onlinelst = onli;
    string all = "all";
    onlinelst.addusr(all,0);
    printf("Please Input IP address to bind:  ");
    char ipadd[12];
    scanf("%s",ipadd);
    printf("Please Input Port:  ");
    int port1;
    scanf("%d",&port1);
    int sockfd=bindipport(ipadd,port1);
    if(sockfd==-1)
    {
        printf("bind error\n");
        exit(-1);
    }
    else
        printf("success!\n");
        power=true;
    acpt(sockfd);
    
}

int msgsvr::bindipport(char *ip, int port)
{
    
    struct sockaddr_in mysock;
    mysock.sin_family = AF_INET;
    mysock.sin_addr.s_addr=inet_addr(ip);
    mysock.sin_port=htons(port);
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if( sockfd == -1)
    {
        perror("socket create failed");
        return -1;
    }
    
    if(::bind(sockfd,(struct sockaddr *)&mysock,sizeof(struct sockaddr))==-1)
    {
        perror("bind error");
        return -1;
    }
    if(listen(sockfd,client_number)==-1)
    {
        perror("Listen error");
        return -1;
    }
    return sockfd;
}

int msgsvr::acpt(int sockfd) //accept connection
{
    int nsock=-1,i=0;
    thread t[client_number];
    thread snd_t=thread(Data_snd,ref(*this));
    //snd_t.join();
    while(power)
    {
        time_t tm=time(nullptr);
        cout<<ctime(&tm)<<"    "<<"waiting for new connection..."<<endl;
        //pthread_t thread_id;
        
        socklen_t len=sizeof(struct sockaddr);
        struct sockaddr_in peer_addr;
        //Block here. Until server accpets a new connection.
        nsock=accept(sockfd,(struct sockaddr *)&peer_addr,&len); //接受，获得新sockfd
        if(nsock == -1)
        {
            fprintf(stderr,"Accept error!\n");
            continue;                               //ignore current socket ,continue while loop.
        }
        time_t tim=time(nullptr);
        cout<<ctime(&tim)<<"    "<<"A new connection occurs!,sockfd is "<<"  "<<nsock<<endl;

        t[i] = std::thread(Data_Handle, std::ref(nsock), std::ref(*this));

        //t[i].join();
        i++;
    }
    
        return nsock;
}

int msgsvr::identfy(string name,string passwd,int sockfd)//check user, return usernumber
{
    User *oneusr=acclist.findusr(name);
    Proto_msg ident;


    if(oneusr !=nullptr)
    {
        if(acclist.isLegal(name, passwd))  //whether it's right or not
        {
            onlinelst.addusr(name,sockfd);//login(namebuf, nsock);
            oneusr->set_status(1);
            vector<onlineuser> olllusr=onlinelst.Getolusr();       //获取此时在线列表
            for(int i=0;i<olllusr.size();i++) {
                string friend_name = olllusr[i].name;
                if(friend_name != name) {
                    Online_list_Response(friend_name);   //发送的是所有在线人的列表
                }
            }
            time_t t=time(nullptr);
            cout<<ctime(&t)<<"    "<<name<<" log in,recvbuf is"<<passwd<<endl;
            ident.set_flag(LOGIN_FLAG);   //response
            ident.set_towhom(name);
            ident.set_info("1");
            Send(ident);
            return sockfd;
        }
        else{
            ident.set_flag(LOGIN_FLAG);
            ident.set_towhom(name);
            ident.set_info("0");
            Send(ident);   //发送登陆失败回复
            return -1;
        }

    }
    else if (oneusr== nullptr)
    {
        User *nusr=acclist.Registration(name,passwd);  //regist new user
        nusr->set_status(1);
        onlinelst.addusr(name,sockfd);

        vector<onlineuser> olllusr=onlinelst.Getolusr();       //获取此时在线列表
        for(int i=0;i<olllusr.size();i++) {           //通知所有朋友我上线了
            string friend_name = olllusr[i].name;      //发送的是所有在线人的列表
            if(friend_name != name) {
                Online_list_Response(friend_name);
            }
        }
        //login(namebuf, nsock);
        time_t tim=time(nullptr);
        cout<<ctime(&tim)<<"    "<<name<<" registed,recvbuf is"<<passwd<<endl;
        //int a=acclist.empty();
        //printf("\n%d\n",a);
        //int cc=1;
        ident.set_flag(LOGIN_FLAG);
        ident.set_towhom(name);
        ident.set_info("1");
        Send(ident);
        return sockfd;
    }
        return -1;
}

void msgsvr::Send(Proto_msg &msg) {
    send_queue.push_back(msg);
    //cout<<"push_back "<<msg.flag()<<endl<<msg.towhom()<<endl<<msg.info()<<endl;
}

void msgsvr::login(onlineuser *usr)
{
    onlinelst.addusr(*usr);
}


void msgsvr::logoff(const std::string usrnum)
{
    int sock=onlinelst.findusr(usrnum);
    close(sock);
    onlinelst.removusr(usrnum);
}

void msgsvr::Online_list_Response(string receiver)
{
    string message;
    vector<onlineuser> list=onlinelst.Getolusr();
    if(list.size()>0)
    {
    while (list.size()>0) {
        message+=list.back().Encode_ol_list();
        list.pop_back();
    }
    message+="#";
    }
    else
        message+="#";
    //string message ="06maning07lizixun11james click#";
    Proto_msg forsnd;
    forsnd.set_flag(GETOL_FLAG);
    forsnd.set_towhom(receiver);
    forsnd.set_info(message);
    Send(forsnd);
}

void msgsvr::Message_Driver(Proto_msg &msg,int sockfd)     //对收取的消息进行处理
{
    int f = msg.flag();
    string twhm = msg.towhom();
    string info = msg.info();
    if (f==EXIT_FLAG)
    {
        logoff(twhm);
        vector<onlineuser> olllusr=onlinelst.Getolusr();       //获取此时在线列表
        for(int i=0;i<olllusr.size();i++) {
            string friend_name = olllusr[i].name;
            if(friend_name != twhm) {
                Online_list_Response(friend_name);   //发送的是所有在线人的列表
            }
        }
    }
    else {
        switch (f) {
            case LOGIN_FLAG: {
                //string ume=msg.towhom();
                //string password=msg.info();
                if (isLoged_In(twhm))
                    break;
                else
                    identfy(twhm, info, sockfd);
                break;
            }
            case GETOL_FLAG: {
                Online_list_Response(twhm);
                break;
            }
            case CHAT_TEXT_FLAG: {
                Send(msg);
                break;
            }
            case GROUP_FLAG: {
                GROUP_Msg(msg);
            }
            default: {
                time_t tim = time(nullptr);
                cout << ctime(&tim) << "    " << "Got an Unknown message: they are" << "     " << msg.flag() << "     "
                     << msg.towhom() << "     " << msg.info() << endl;
                break;
            }
        }
    }
    
}

void msgsvr::GROUP_Msg(Proto_msg &msg) {
    vector<onlineuser> list=onlinelst.Getolusr();
    string originmsg = msg.info();
    int namelen = atoi(originmsg.substr(0,2).c_str());
    int msglen = atoi(originmsg.substr(2+namelen,2).c_str());
    string sender_name =originmsg.substr(2,namelen);
    if(list.size()>1)
    {
        for(int i =0;i<list.size();i++)
        {
            Proto_msg ori = msg;
            if(list[i].name != sender_name)
            ori.set_towhom(list[i].name);
            Send(ori);
        }
    }
}

static void *Data_snd(msgsvr& svr)
{
    while (svr.power)
    {
        if(!send_queue.empty())
        {
            if(!svr.power)
                break;
            Proto_msg presend=send_queue.front();
            send_queue.pop_front();
            string toWhom = presend.towhom();
            int sockf = svr.findOnlineusr(toWhom);//通过towhom判断发给谁
            char buff[BUFFSIZE];
            presend.SerializePartialToArray(buff,BUFFSIZE);
            time_t tim=time(NULL);
            cout<<ctime(&tim)<<"    ""prepare to send, presend is "<<presend.flag()<<"     "<<presend.towhom()<<"     "<<presend.info()<<endl;
            if(send(sockf,buff,strlen(buff),0) < 0)
            {
                cout<<"send to"<<svr.GetOL_List().findusrbysock(sockf)<<"failed..."<<endl;
            }
            cout<<"sent"<<endl;
            
            
        }
        else
        {
            usleep(100000);
            //cout<<"im sleeping"<<endl;
        }
    }
    pthread_exit(nullptr);
}


static void *Data_Handle(int sockf,msgsvr& svr)  /*一个客户端对应一个处理线程 接收然后处理 */
{

    while(svr.power) {
        usleep(1000);
        Proto_msg recv_msg;
        char buf1[BUFFSIZE];
        memset(&buf1, 0, BUFFSIZE * sizeof(char));
        cout<<"listening "<<"sockf"<<sockf<<endl;
        if (recv(sockf, buf1, sizeof(buf1), 0) < 0) {
            cout << "Receive from " << svr.GetOL_List().findusrbysock(sockf) << "failed..." << endl;
            break;
        }
        else
        {
        recv_msg.ParseFromArray(buf1, BUFFSIZE);
        time_t tim=time(nullptr);
            cout<<ctime(&tim)<<"    "<<"Received "<<recv_msg.flag()<<"   "<<recv_msg.towhom()<<"    "<<"and "<<recv_msg.info()<<endl;
            //cout<<"imhere"<<endl;
        svr.Message_Driver(recv_msg,sockf);
        if (recv_msg.flag()==EXIT_FLAG)
            break;
        }
    }
    time_t tim=time(nullptr);
    cout<<ctime(&tim)<<"    "<<"Client "<<sockf<<"exited"<<endl;
    pthread_exit(nullptr);
}





/*

{
    //int sockf = *((int *)sock_fd);
    if(svr.identfy(sockf)!=-1)
    {
    while(1)
    {
        
        char buffer[200];
        int iDataNum;
        //printf("从%d读取消息:",sockf);
        buffer[0] = '\0';
        iDataNum = recv(sockf, buffer, 1024, 0);
        if(iDataNum < 0)
        {
            perror("recv null");
            continue;
        }
        buffer[iDataNum] = '\0';
        if(strcmp(buffer, "quit") == 0)
            break;
        printf("从%d读取消息: %s\n", sockf,buffer);
        printf("发送消息给%d:",sockf);
        scanf("%s", buffer);
        printf("\n");
        send(sockf, buffer, strlen(buffer), 0);
        if(strcmp(buffer, "quit") == 0)
            break;
    }
    printf("terminating current client_connection...\n");
    close(sockf);            //close a file descriptor.
    pthread_exit(NULL);   //terminate calling thread!
    }
    else
        close(sockf);
    pthread_exit(NULL);
}
*/
