//
//  clt.cpp
//  chat-client
//
//  Created by 马宁 on 2019/6/27.
//  Copyright © 2019 马宁. All rights reserved.
//


#include "clt.h"
#define PWDSIZE 20
#define ARRAY_SIZE(a)(sizeof(a) / sizeof(a[0]))
SharedQueue<Proto_msg> send_queue;
static void *Send_Thread(client& clt);
static void *Recv_Thread(client& clt);
char *choices[] = {
        "Choice 1",
        "Choice 2",
        "Choice 3",
        "Choice 4",
        "Exit",
        (char *)NULL,
};

char *choices2[] = {
        "Add Friend",
        "Settings",
        "Help",
        "Exit",
        (char *)NULL,
};
void func(char *name,client &clt)
{
    mvprintw(20, 0, "Item selected is : %s", name);
    std::string ss = name;

}

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
    //attron(A_BOLD);
    char a[]="Welcome to the Chat\n";
    //print_in_middle(NULL, 10, 0, 0, a, COLOR_BLUE);
    //printw("Welcome to the Chat\n");
    //attroff(A_BOLD);
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
    char b[]="Press any key to continue";
    //print_in_middle(NULL, 0, 0, 0, b, COLOR_BLUE);
    thread t[2];
    t[0]=thread(Send_Thread,ref(*this));
    //t[0].join();
    t[1]=thread(Recv_Thread,ref(*this));
    //t[1].join();

    refresh();
    //getch();clear();echo();
    char mseg[]="User Number:";
    char usrnumb[PWDSIZE];
    char msg2[]="Password:";
    char passwd[PWDSIZE];
    int row,col;
    //getmaxyx(stdscr,row,col);attron(A_BOLD);mvprintw(row/3,(col-strlen(mseg))/2, "%s",mseg);
    //getstr(usrnumb);scanw("%s",usrnumb);int usrnum=atoi(usrnumb);mvprintw(row/3+2, (col-strlen(mseg))/2, "%s",msg2);attroff(A_BOLD);
    //getstr(passwd);
    //scanw("%s",passwd);
    //string username = usrnumb;
    //string password = passwd;
    string username = "maning";
    string password = "mima";
    Setmyname(username);
    Proto_msg ident;
    ident.set_flag(LOGIN_FLAG);
    ident.set_towhom(username);
    ident.set_info(password);
    Send(ident);
    login();
    return 1;

}
bool swtch = false;
void client::login()
{
    while(!online_status)
    {
        //cout<<"sleep"<<endl;
        usleep(100000);
    }
    cout<<"登陆成功！"<<endl;
    Reqst_Online_List();
    /*while (!swtch) {
        usleep(100000);
    }
    for (std::vector<my_friend>::iterator it = friends.begin() ; it != friends.end(); ++it)
        std::cout << ' ' << it->name;
    std::cout << '\n';*/
    //Init_ChatUI();

}

void client::Send(Proto_msg &msg) {
    send_queue.push_back(msg);
    cout<<"push_back "<<msg.flag()<<endl<<msg.towhom()<<endl<<msg.info()<<endl;
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

void client::Print_Prompt(const char* str) {
    int y=getmaxy(stdscr);
    mvprintw(y-3,3,str);
}

char *client::Reqst_Online_List()
{
    Proto_msg rqst;
    rqst.set_flag(GETOL_FLAG);
    Send(rqst);
}

void client::Init_ChatUI()
{
    cout<< "shit!"<<endl;
    /*Reqst_Online_List();

    WINDOW *my_wins[3];

    PANEL *my_panels[3]; PANEL *top; int ch;
    ITEM **my_items,**my_items2; MENU *my_menu,*my_menu2;
    ITEM *cur_item2;
    int n_choices,n_choices2,i;
    
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    
    initscr(); start_color(); cbreak(); noecho(); keypad(stdscr, TRUE);
    n_choices =ARRAY_SIZE(choices);
    n_choices2 =ARRAY_SIZE(choices2);
    my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
    my_items2 = (ITEM **)calloc(n_choices2 + 1, sizeof(ITEM *));
    for(i = 0; i < n_choices; ++i)
    {
        my_items[i] = new_item(choices[i], choices[i]);
        set_item_userptr(my_items[i],(void *)func);
    }

    for(i = 0; i < n_choices2; ++i)
    {
        my_items2[i] = new_item(choices2[i], 0);
     
        set_item_userptr(my_items2[i],(void *)func);
        //void *item_userptr(const ITEM *my_items2[i]);
    }
    //my_items2[n_choices2] = (ITEM *)NULL;
    
    my_menu = new_menu((ITEM **)my_items);
    my_menu2 = new_menu((ITEM **)my_items2);
    init_wins(my_wins, 3);
    keypad(my_wins[0], TRUE);
    keypad(my_wins[1], TRUE);
    
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    my_panels[2] = new_panel(my_wins[2]);

    
   
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[2]);
    set_panel_userptr(my_panels[2], my_panels[0]);
    
    update_panels();

    int h1,h2,w1,w2;
    getmaxyx(my_wins[0],h1,w1);    //getmaxyx(stdscr,row,col);
    getmaxyx(my_wins[1],h2,w2);
    //set_menu_win(my_menu, my_wins[0]);
    int height,width;
    getmaxyx(stdscr,height,width);
    mvprintw(10,2,"h2 is %d,w2 is %d\n",h2,w2);
    set_menu_sub(my_menu, derwin(my_wins[0], h1-3, w1-3, 3, 2));
    set_menu_win(my_menu2, my_wins[1]);
    //set_menu_sub(my_menu2, derwin(my_wins[1], height-h1-3, w2-3, h1+5, 2));
    //set_menu_mark(my_menu, " * ");
    set_menu_mark(my_menu2, " * ");
    //set_menu_grey(my_menu, 0);

    post_menu(my_menu);
    post_menu(my_menu2);
    wrefresh(my_wins[0]);
    wrefresh(my_wins[1]);

    mvprintw(1, w1/3, "Friends");
  
    doupdate();
    top = my_panels[2];
    while((ch = getch()) != KEY_F(1))
    { switch(ch)
        { case 9:
                top = (PANEL *)panel_userptr(top);
                top_panel(top);
                break;
        }
        update_panels();
        doupdate();
        if(top->win==my_wins[0])
        {
            //int c;c = wgetch(my_wins[0]);
            //while((c = wgetch(my_wins[0]))!= KEY_F(1))
            switch(ch)
            { case KEY_DOWN:
                    menu_driver(my_menu, REQ_DOWN_ITEM);
                    break;
                case KEY_UP:
                    menu_driver(my_menu, REQ_UP_ITEM);
                    break;
                case KEY_RIGHT:
                    menu_driver(my_menu, REQ_SCR_DPAGE);
                    break;
                case KEY_LEFT:
                    menu_driver(my_menu, REQ_SCR_UPAGE);
                    break;
                case 10:
                {
                    ITEM *cur;
                    void (*p)(char *,client&);
                    cur = current_item(my_menu);
                    p = reinterpret_cast<void (*)(char *,client&)>(item_userptr(cur));
                    p((char *) item_name(cur),std::ref(*this));
                    pos_menu_cursor(my_menu);
                    break;
                }
            }
            wrefresh(my_wins[0]);
            mvprintw(1, w1/3, "Friends");
        }
        else if(top->win == my_wins[1])
            switch(ch)
            { case KEY_DOWN:
                    menu_driver(my_menu2, REQ_DOWN_ITEM);
                    break;
                case KEY_UP:
                    menu_driver(my_menu2, REQ_UP_ITEM);
                    break;
                case 10:
                { ITEM *cur2;
                    void (*q)(char *,client&);
                    cur2 = current_item(my_menu2);
                    q = reinterpret_cast<void (*)(char *,client&)>(item_userptr(cur2));
                    q((char *)item_name(cur2),std::ref(*this));
                    pos_menu_cursor(my_menu2);
                    break;
                }
            }
    }
    unpost_menu(my_menu);
    free_menu(my_menu);
    for(i = 0; i < n_choices; ++i)
        free_item(my_items[i]);

    unpost_menu(my_menu2);
    free_menu(my_menu2);
    for(i = 0; i < n_choices2; ++i)
        free_item(my_items2[i]);
    endwin();
*/
}

void client::print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{ int length, x, y;
    float temp;
    if(win == NULL)
        win = stdscr;
    getyx(win, y,x);
    if(startx != 0)
        x = startx;
    if(starty != 0)
        y = starty;
    if(width == 0)
        width = 80;
    length = strlen(string);
    temp = (width-length)/ 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y,x, "%s", string);
    wattroff(win, color);
    refresh();
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

/* Put all the windows */
void client::init_wins(WINDOW **wins, int n)
{
    int i,height,width; char label[80];
    getmaxyx(stdscr,height,width);
    int NLINES[3]={height*2/3,height/3,height};
    int NCOLS[3]={width/3,width/3,width*2/3};
    int y[3] = {0,height*2/3,0};
    int x[3] = {0,0,width/3};
    for(i = 0; i < n; ++i)
    {
        wins[i] = newwin(NLINES[i],NCOLS[i], y[i], x[i]);
    }

    for(i=0;i<n;i+=2)
    {
        int startx, starty, height, width;
        getmaxyx(*&wins[i], height, width);
        box(*&wins[i], 0, 0);
        mvwaddch(*&wins[i], 2, 0, ACS_LTEE);
        mvwhline(*&wins[i], 2, 1, ACS_HLINE, width - 2);
    }
    //int ht,wh;
    //getmaxyx(wins[1], ht, wh);
    //box(wins[1], 0, 0);
    //mvwaddch(wins[1], 2, 0, ACS_LTEE);
    //mvwhline(wins[1], 2, 1, ACS_HLINE, wh - 2);

}

void client::Chat_Text_Handle(Proto_msg &msg)
{
    string originmsg = msg.info();
    int namelen = atoi(originmsg.substr(0,2).c_str());
    int msglen = atoi(originmsg.substr(2+namelen,2).c_str());
    string sender_name =originmsg.substr(2,namelen);
    string messg = originmsg.substr(4+namelen,msglen);
}

void client::Decode_OL_List(string mesg)
{
    string strbuf="";
    string res;
    int i=0;
    while(strbuf!="#")
    {
        int len = atoi(mesg.substr(i,2).c_str());
        i+=2;
        Add_Friend(mesg.substr(i,len), true);
        i+=len;
        strbuf = mesg[i];
    }
    swtch=true;
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
            cout<<"   i recv :  "<<msg.flag()<<msg.towhom()<<"    "<<msg.info()<<endl;
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
            cout<<"send "<<presend.flag()<<presend.towhom()<<"+++"<<presend.info()<<endl;
            char buff[BUFFSIZE];
            presend.SerializePartialToArray(buff,BUFFSIZE);
            if(send(clt.serversock,buff,strlen(buff),0) < 0)
            {
                clt.Print_Prompt("send failed...");
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
        if (recv(clt.serversock, buf1, sizeof(buf1), 0) < 0)
            clt.Print_Prompt("Receive failed...");
        recv_msg.ParseFromArray(buf1, BUFFSIZE);
        clt.Client_driver(recv_msg);
    }
    pthread_exit(nullptr);
}
