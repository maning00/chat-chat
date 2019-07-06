//
//  clt.cpp
//  chat-client
//
//  Created by 马宁 on 2019/6/27.
//  Copyright © 2019 马宁. All rights reserved.
//


#include "clt.h"
#define PWDSIZE 10
#define ARRAY_SIZE(a)(sizeof(a) / sizeof(a[0]))
mutex rmtx,smtx;
static void *Send_Thread(client& clt);
static void *Recv_Thread(client& clt);
static void *Data_handle(client& clt);
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
    //SharedQueue<Proto_msg> snd;
    //SharedQueue<Proto_msg> rcv;
    auto *na = new std::vector<my_friend>();
    
    //recv_queue = rcv;
    //send_queue = snd;
    friends=*na;
    serversock=0;
    
}


int client::connectosvr(char *ipadd, int port)
{
    attron(A_BOLD);
    char a[]="Welcome to the Chat\n";
    print_in_middle(NULL, 10, 0, 0, a, COLOR_BLUE);
    //printw("Welcome to the Chat\n");
    attroff(A_BOLD);
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
    //printw("connected");
    char buf[10];
    //recv(sockfd,buf,PWDSIZE*sizeof(char),0);
    //printw("%s\n",buf);
    char b[]="Press any key to continue";
    print_in_middle(NULL, 0, 0, 0, b, COLOR_BLUE);
    refresh();
    getch();
    clear();
    echo();
    char mseg[]="User Number:";
    char usrnumb[PWDSIZE];
    char msg2[]="Password:";
    char passwd[PWDSIZE];
    int row,col;
    getmaxyx(stdscr,row,col);
    attron(A_BOLD);
    mvprintw(row/3,(col-strlen(mseg))/2, "%s",mseg);
    //getstr(usrnumb);
    scanw("%s",usrnumb);
    int usrnum=atoi(usrnumb);
    mvprintw(row/3+2, (col-strlen(mseg))/2, "%s",msg2);
    attroff(A_BOLD);
    //getstr(passwd);
    scanw("%s",passwd);
    send(sockfd,usrnumb,10*sizeof(char),0);
    send(sockfd,passwd,10*sizeof(char),0);
    char buf1[BUFFSIZE];
    Proto_msg ident;
    memset(&buf1, 0, BUFFSIZE*sizeof(char));
    sleep(1);
    recv(sockfd,buf1, sizeof(buf1),0);
    ident.ParseFromArray(buf1,BUFFSIZE);
    string towhom = ident.towhom();
    if(ident.flag()==LOGIN_FLAG && atoi(towhom.c_str())==1) //登陆成功
        return sockfd;
    //printf("buf1 is %s \n",buf1);
    //printf("didn't get\n");
    return -1;

}

void client::login()
{
    thread t[3];
    t[0]=thread(Send_Thread,ref(*this));
    t[1]=thread(Recv_Thread,ref(*this));
    t[2]=thread(Data_handle,ref(*this));
    power=TRUE;

}

void client::Send(Proto_msg &msg) {
    smtx.lock();
    send_queue.push_back(msg);
    smtx.unlock();
}

void client::Receive(Proto_msg &msg) {
    recv_queue.push_back(msg);
}

void client::Print_Prompt(const char* str) {
    int y=getmaxy(stdscr);
    mvprintw(y-3,3,str);
}

void client::Reqst_Online_List()
{
    Proto_msg rqst;
    rqst.set_flag(RQST_ONLINE_FLAG);
    Send(rqst);
}

void client::Init_ChatUI()
{
    Reqst_Online_List();

    WINDOW *my_wins[3];

    PANEL *my_panels[3]; PANEL *top; int ch;
    ITEM **my_items,**my_items2; MENU *my_menu,*my_menu2;
    ITEM *cur_item2;
    int n_choices,n_choices2,i;
    /* Initialize all the colors */
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    /* Initialize curses */
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
        /* 设置用户指针 */
        set_item_userptr(my_items2[i],(void *)func);
        //void *item_userptr(const ITEM *my_items2[i]);
    }
    //my_items2[n_choices2] = (ITEM *)NULL;
    /* 创建菜单 */
    my_menu = new_menu((ITEM **)my_items);
    my_menu2 = new_menu((ITEM **)my_items2);
    init_wins(my_wins, 3);
    keypad(my_wins[0], TRUE);
    keypad(my_wins[1], TRUE);
    /* Attach a panel to each window */
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    my_panels[2] = new_panel(my_wins[2]);

    /* Order is bottom up */
    /* Push 0, order: stdscr−0 */
    /* Push 1, order: stdscr−0−1 */
    /* Push 2, order: stdscr−0−1−2 */

    /* Set up the user pointers to the next panel */
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[2]);
    set_panel_userptr(my_panels[2], my_panels[0]);
    /* Update the stacking order. 2nd panel will be on top */
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
    /* Show it on the screen */
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
                case 10: /* Enter 键 */
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
                case 10: /* Enter 键 */
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

void client::Client_driver(Proto_msg &msg)
{
    int ch = msg.flag();
    switch(ch)
    {
        case 1:
        {
            if(atoi(msg.towhom().c_str())==0)
                power= false;
        }
        case 2:
        {

        }
    }
}

static void *Send_Thread(client& clt)
{
    while (clt.power)
    {
        if(clt.send_queue.size()!=0)
        {
            while(!smtx.try_lock()){usleep(100000);}
            Proto_msg presend=clt.send_queue.front();
            clt.send_queue.pop_front();
            smtx.unlock();
            char buff[BUFFSIZE];
            presend.SerializePartialToArray(buff,BUFFSIZE);
            if(send(clt.serversock,buff,strlen(buff),0) < 0)
            {
                clt.Print_Prompt("send failed...");
            }
        }
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
        while (!rmtx.try_lock()) { usleep(100000); }
        clt.recv_queue.push_back(recv_msg);
        rmtx.unlock();
    }
    pthread_exit(nullptr);
}
static void *Data_handle(client& clt)
{
    while(clt.power)
    {
        
        Proto_msg data = clt.recv_queue.front();
        clt.recv_queue.pop_front();
        clt.Client_driver(data);
        
    }
    pthread_exit(nullptr);
}