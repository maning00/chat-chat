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
static void *Data_rcv(int& sockf,msgsvr& svr);
static void *Data_snd(int& sockf,msgsvr& svr);
static void *Data_handle(int& sockf,msgsvr& svr);
mutex rmtx,wmtx;

void msgsvr::Init(){
    printf("Welcome!\n");
    accountList aclist=accountList();
    acclist=aclist;
    OnlineUsr_List onli =OnlineUsr_List();
    onlinelst = onli;
    vector<Proto_msg> sq;
    vector<Proto_msg> rq;
    send_queue=sq;
    recv_queue=rq;
    printf("Please Input IP address to bind:  ");
    char ipadd[12]="127.0.0.1";
    //scanf("%s",ipadd);
    //printf("Please Input Port:  ");
    int port1=5019;
    //scanf("%d",&port1);
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
    std::thread t[client_number];
    while(power)
    {
        printf("waiting for new connection...\n");
        //pthread_t thread_id;
        
        socklen_t len=sizeof(struct sockaddr);
        struct sockaddr_in peer_addr;
        //Block here. Until server accpets a new connection.
        nsock=accept(sockfd,(struct sockaddr *)&peer_addr,&len);
        if(nsock == -1)
        {
            fprintf(stderr,"Accept error!\n");
            continue;                               //ignore current socket ,continue while loop.
        }
        printf("A new connection occurs!,sockfd is %d\n",nsock);
        t[i]=std::thread(Data_rcv,std::ref(nsock),std::ref(*this));
        i++;
        //char bufff[]="isenconne";
        //send(nsock,bufff,PWDSIZE*sizeof(char),0);
            //printf("send \n");
        //void (msgsvr::*func)(int&);
        //func = &msgsvr::Data_handle;
        //if(identfy(nsock)!=-1)
        //{
            
            /*if(pthread_create(&thread_id,NULL,Data_handle,(void *)(&nsock)) == -1){
            fprintf(stderr,"pthread_create error!\n");
            break;                             //break while loop
        }*/
       // }
        //else
            //close(nsock);
    }
    
        return nsock;
}

int msgsvr::identfy(int nsock)//check user, return usernumber
{
    std::string namebuf;
    //char buf1[]="hello,Welcome to chat\n";
    //int Send = send(nsock, buf1, sizeof(buf1) , 0);
    //printf("buf1 sent");
    //if(Send == -1){
    //    perror("send failed");
     //   return -1;
    //}
    char recvBuf[PWDSIZE];
    
    memset(recvBuf, 0, sizeof(recvBuf));
    //      //接收数据
    
    recv(nsock, recvBuf, PWDSIZE*sizeof(char), 0);  //Receive user's name;
    //printf("imhere");
    namebuf=recvBuf;
    cout<<"namebuf is "<<namebuf<<endl;
    memset(recvBuf, 0, PWDSIZE*sizeof(char));
    recv(nsock, recvBuf, PWDSIZE*sizeof(char), 0);  //receive password
    printf("pass %s",recvBuf);
    User *oneusr=acclist.findusr(namebuf);
    std::string s1=recvBuf;
    Proto_msg ident;

    if(oneusr !=nullptr)
    {
        if(acclist.isLegal(namebuf, s1))  //whether it's right or not
        {
            this->onlinelst.addusr(namebuf,nsock);//login(namebuf, nsock);
            oneusr->set_status(1);
            cout<<namebuf<<" log in,recvbuf is"<<recvBuf<<endl;
            ident.set_flag(1);
            ident.set_towhom("1");
            Send(ident);
            return nsock;
        }
    }
    else if (oneusr== nullptr)
    {
        std::string passwd = recvBuf;
        User *nusr=acclist.Registration(namebuf,passwd);  //regist new user
        nusr->set_status(1);
        this->onlinelst.addusr(namebuf,nsock);
        //login(namebuf, nsock);
        cout<<namebuf<<" registed,recvbuf is"<<recvBuf<<endl;
        //int a=acclist.empty();
        //printf("\n%d\n",a);
        //int cc=1;
        ident.set_flag(1);
        ident.set_towhom("1");
        char buff[BUFFSIZE];
        ident.SerializePartialToArray(buff,BUFFSIZE);
        if(send(nsock,buff,strlen(buff),0) < 0)
        {
            cout<<": send failed ..."<<endl;
        }
        Send(ident);
        return nsock;
    }
        return -1;
}

/* void msgsvr::Send(Proto_msg &msg) {
    send_queue.insert(msg);
}

void msgsvr::Receive(Proto_msg &msg) {
    recv_queue.insert(msg);
}*/

void msgsvr::login(onlineuser *usr)
{
    onlinelst.addusr(*usr);
}


void msgsvr::logoff(std::string usrnum)
{
    int sock=onlinelst.findusr(usrnum);
    
    onlinelst.removusr(onlinelst.findusrbysock(sock));
}

/* 
static void *Data_rcv(int& sockf,msgsvr& svr)
{
    while(svr.power) {
        Proto_msg recv_msg;
        char buf1[BUFFSIZE];
        memset(&buf1, 0, BUFFSIZE * sizeof(char));
        if (recv(sockf, buf1, sizeof(buf1), 0) < 0)
            cout<<"Receive from "<<svr.GetOL_List().findusrbysock(sockf)<<"failed..."<<endl;
        recv_msg.ParseFromArray(buf1, BUFFSIZE);
        while (!rmtx.try_lock()) { usleep(100000); }
        svr.recv_queue.insert(recv_msg);
        rmtx.unlock();
    }
    pthread_exit(nullptr);
}
*/
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