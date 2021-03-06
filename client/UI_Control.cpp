#include"UI_Control.h"
#define ARRAY_SIZE(a)(sizeof(a) / sizeof(a[0]))
char *choices2[] = {
        "Settings",
        "Help",
        "About",
        "Exit",
        (char *)NULL,
};

int printCount;

void func(char *name,UI_Control& ui);

int UI_Control::Init_MainUI()
{
    initscr(); start_color(); cbreak(); echo(); keypad(stdscr, TRUE);
    attron(A_BOLD);
    char a[]="Welcome to the Chat\n";
    print_in_middle(nullptr, 10, 0, 0, a, COLOR_BLUE);
    attroff(A_BOLD);
    char b[]="Press any key to continue";
    print_in_middle(nullptr, 0, 0, 0, b, COLOR_BLUE);
    refresh();
    getch();clear();echo();
    char msegg[]="IP: ";
    char ipad[PWDSIZE];
    char msgg2[]="Port: ";
    char pot[PWDSIZE];
    int row,col;
    getmaxyx(stdscr,row,col);
    attron(A_BOLD);
    mvprintw(row/3,(col-strlen(msegg))/2-3, "%s",msegg);
    scanw("%s",ipad);
    mvprintw(row/3+2, (col-strlen(msegg))/2-3, "%s",msgg2);
    attroff(A_BOLD);
    scanw("%s",pot);
    int port1 = atoi(pot);
    //char ipad[]="127.0.0.1";
    //int port1=5019;
    if(core.connectosvr(ipad,port1)<0) {
        printw("connect failed\n");
        endwin();
        return -1;
    }
    UI_Login();
    return 0;
}

int UI_Control::UI_Login()
{
    clear();
    box(stdscr,0,0);
    char mseg[]="User Name:";
    char usrnumb[PWDSIZE];
    char msg2[]="Password:";
    char passwd[PWDSIZE];
    char msg3[]="Log in / Register";
    int row,col;
    getmaxyx(stdscr,row,col);
    attron(A_BOLD);
    mvprintw(row/3,(col-strlen(mseg))/2-2, "%s",mseg);
    scanw("%s",usrnumb);
    mvprintw(row/3+4,(col-strlen(mseg))/2-2,"%s",msg3);
    mvprintw(row/3+2, (col-strlen(mseg))/2-2, "%s",msg2);
    attroff(A_BOLD);
    scanw("%s",passwd);
    string usr = usrnumb;
    string pass = passwd;
    core.new_message_sign= false;
    string sstr="n";     //暂无发送目标
    core.Set_Sendto(sstr);
    //string usr="maning";
    //string pass = "mima";
    if(core.login(usr,pass)<0) {
        printw("Log in Failed...");
        endwin();
        return -1;
    }
    core.Reqst_Online_List();
    while(core.friends_empty()) {
        sleep(1);
        /*if(core.friends_empty()) {
            vector<string> loading;
            string str1="Loading...";
            string str2="Loading...";
            string str3="Loading...";
            string str4="Loading...";
            loading.push_back(str1);
            loading.push_back(str2);
            loading.push_back(str3);
            loading.push_back(str4);
            Init_ChatUI(loading);
            break;
        }
        else
        {*/
            vector<string> choicess = core.Get_Friend_List();
            Init_ChatUI(choicess);
            break;
        //}
    }

    return 1;
}

void func(char *name,UI_Control& ui)
{
    string selected = name;
    if(selected == "Help")
    {
        int c;
        clear();
        int y, x;
        getmaxyx(stdscr, y, x);
        init_pair(5, COLOR_WHITE, COLOR_BLACK);
        char about[] = "Press Tab to switch panel";
        char abou[] = "Press Enter to select";
        char an[] = "Any Question, Contact ningma1997@gmail.com";
        ui.print_in_middle(stdscr, y / 3, x / 3, sizeof(about), about, 5);
        ui.print_in_middle(stdscr, y / 3 + 2, x / 3 + 2, sizeof(abou), abou, 5);
        ui.print_in_middle(stdscr, y / 2 + 4, x / 3 - 8, sizeof(an), an, 5);
        while ((c=getch())!=KEY_F(1))
        {

        }
        ui.Backto_ChatUI();
    }
    else if(selected=="About")
    {
        int c;
        clear();
        int y, x;
        getmaxyx(stdscr, y, x);
        init_pair(5, COLOR_BLUE, COLOR_BLACK);
        char about[] = "Created by Ning.M";
        ui.print_in_middle(stdscr, y / 3, x / 3, sizeof(about), about, 5);
        char about2[] = "Copyright © 2019 All rights reserved.";
        ui.print_in_middle(stdscr, y*3 / 4 + 1, x / 4, sizeof(about2), about2, 5);
        while ((c=getch())!=KEY_F(1))
        {

        }
        ui.Backto_ChatUI();
    }
    else if(selected == "Exit")
    {
        ui.UI_EXIT();
    }
    else if (selected ==ui.GetMyName())
    {
        wprintw(ui.my_wins[2],"This is me");
    }
    else
    {
        ui.Set_Sendto(selected);
        ui.Clear_Chat();
        mvwprintw(ui.my_wins[2],1,3,"                   ");
        mvwprintw(ui.my_wins[2],1,3,selected.c_str());
        wrefresh(ui.my_wins[2]);
    }
    //mvprintw(20, 0, "Item selected is : %s", name);

}

void UI_Control::print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
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

/* Put all the windows */
void UI_Control::init_wins(WINDOW **wins, int n)
{
    int i,height,width; char label[80];
    getmaxyx(stdscr,height,width);
    int NLINES[3]={height*2/3,height/3,height*8/9};
    int NCOLS[3]={width/3,width/3,width*2/3};
    int y[3] = {0,height*2/3,0};
    int x[3] = {0,0,width/3};
    for(i = 0; i < n; ++i)
    {
        wins[i] = newwin(NLINES[i],NCOLS[i], y[i], x[i]);
    }

    for(i=0;i<3;i+=2)
    {
        int  width;
        width=getmaxx(*&wins[i]);
        //getmaxyx(*&wins[i], height, width);
        box(*&wins[i], 0, 0);
        mvwaddch(*&wins[i], 2, 0, ACS_LTEE);
        mvwhline(*&wins[i], 2, 1, ACS_HLINE, width - 2);
    }

}

void UI_Control::Print_Prompt(const char* str) {
    int y=getmaxy(stdscr);
    attron(COLOR_BLUE);
    mvprintw(y-3,3,str);
    attroff(COLOR_BLUE);
}


void UI_Control::Init_ChatUI(vector<string> choices)
{
    for(int k=0;k<choices.size();k++) {
        if (choices[k] == GetMyName())
            choices.erase(choices.begin() + k);
    }

    initscr(); start_color(); cbreak(); echo(); keypad(stdscr, TRUE);
    ITEM **my_items;
    clear();
    int y=getmaxy(stdscr);
    //for(int k=0;k<choices.size();k++)
        //mvprintw(y-3-k,3,choices[k].c_str());
    core.online_remider=false;
    int n_choices,n_choices2;
    int ch;
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);

    init_wins(my_wins, 4);
    keypad(my_wins[0], TRUE);
    keypad(my_wins[1], TRUE);
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    my_panels[2] = new_panel(my_wins[2]);
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[2]);
    set_panel_userptr(my_panels[2], my_panels[0]);
    update_panels();

    /*更新好友列表*/
    //unpost_menu(my_menu);
    //free_menu(my_menu);
    n_choices =choices.size();
    my_items = (ITEM **)calloc(n_choices+1, sizeof(ITEM *));
    int i=0;
    for(i = 0; i < n_choices; ++i)
    {
        my_items[i] = new_item(choices[i].c_str(), 0);
        set_item_userptr(my_items[i],(void *)func);
    }

    my_menu = new_menu((ITEM **)my_items);
    int h1,w1;
    getmaxyx(my_wins[0],h1,w1);
    set_menu_sub(my_menu, derwin(my_wins[0], h1-3, w1-3, 3, 2));
    post_menu(my_menu);

    wrefresh(my_wins[0]);
    doupdate();
    /*更新完成*/
    n_choices2 =ARRAY_SIZE(choices2);
    my_items2 = (ITEM **)calloc(n_choices2 + 1, sizeof(ITEM *));
    for(i = 0; i < n_choices2; ++i)
    {
        my_items2[i] = new_item(choices2[i], 0);

        set_item_userptr(my_items2[i],(void *)func);
    }
    my_menu2 = new_menu((ITEM **)my_items2);

    w1=getmaxx(my_wins[0]);
    set_menu_win(my_menu2, my_wins[1]);
    set_menu_mark(my_menu2, " * ");
    post_menu(my_menu2);

    top = my_panels[0];

    char mseg[]="Send: ";
    mvprintw(1, w1/3, "Friends");
    int row,col;
    getmaxyx(stdscr,row,col);
    attron(A_BOLD);
    mvprintw(row-2,col/3,mseg);
    wrefresh(my_wins[1]);
    wrefresh(my_wins[0]);
    refresh();
    doupdate();
    while(!core.online_remider)  //在线列表无变化则运行下面代码
    {
        ch = getch();
        Refresh_chatMsg();
        if(ch==9)   //tab
        {
            top = (PANEL *)panel_userptr(top);
            top_panel(top);
            update_panels();
            //Refresh_chatMsg();
        doupdate();
        }
        if(top->win == my_wins[2])
                {
            move(row - 2, col / 3+5);  //在对话框panel就把光标移到Send行
            if(core.Get_Sendto()!="n") {   //如果用户选择了sendto的人
                char usrnumb[30];
                mvprintw(row - 2, col / 3, mseg);
                refresh();
                echo();
                getstr(usrnumb);
                noecho();
                mvprintw(row - 2, col / 3+1, "                                ");
                refresh();
                if (strncmp(usrnumb, "", 1) != 0) {
                    string mes = usrnumb;
                    Send_Message(mes);
                    int xx =getmaxx(my_wins[2]);
                    mvwprintw(my_wins[2],printCount,xx-mes.size()-2,mes.c_str());
                    printCount++;
                    wrefresh(my_wins[2]);
                }
            }
                }
        else if(top->win==my_wins[0])
        {
            mvprintw(1, w1/3, "Friends");
            refresh();
            //wrefresh(my_wins[0]);
            move(4,2);
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
                case 10:   //Enter
                {
                    ITEM *cur;
                    void (*p)(char *,UI_Control&);
                    cur = current_item(my_menu);
                    p = reinterpret_cast<void (*)(char *,UI_Control&)>(item_userptr(cur));
                    p((char *) item_name(cur),ref(*this));
                    pos_menu_cursor(my_menu);
                    break;
                }
            }
            wrefresh(my_wins[0]);
            mvprintw(1, w1/3, "Friends");
        }
        else if(top->win == my_wins[1])
        {
            switch(ch)
            { case KEY_DOWN:
                    menu_driver(my_menu2, REQ_DOWN_ITEM);
                    break;
                case KEY_UP:
                    menu_driver(my_menu2, REQ_UP_ITEM);
                    break;
                case 10:
                { ITEM *cur2;
                    void (*q)(char *,UI_Control&);
                    cur2 = current_item(my_menu2);

                    q = reinterpret_cast<void (*)(char *,UI_Control&)>(item_userptr(cur2));
                    q((char *)item_name(cur2),ref(*this));
                    pos_menu_cursor(my_menu2);
                    break;
                }
            }
            wrefresh(my_wins[1]);
        }
        
    }
    endwin();
    unpost_menu(my_menu);
    free_menu(my_menu);
    //for(i = 0; i < n_choices; ++i)
      //  free_item(my_items[i]);

    /*unpost_menu(my_menu2);
    free_menu(my_menu2);
    for(i = 0; i < n_choices2; ++i)
        free_item(my_items2[i]);
    endwin();*/
    vector<string> nchoicess = core.Get_Friend_List();
    Init_ChatUI(nchoicess);
}

void UI_Control::Send_Message(string msg) {
    string namelen = core.Getmyname();
    string sen;
    sen.clear();
    if(namelen.size()<10)
    {
        sen = "0";
    }
    sen+=to_string(namelen.size());
    sen+=namelen;
    if(msg.size()<10)
    {
        sen += "0";
    }
    sen +=to_string(msg.size());
    sen +=msg;

    Proto_msg forsend;
    forsend.set_flag(CHAT_TEXT_FLAG);
    forsend.set_towhom(core.Get_Sendto());
    forsend.set_info(sen);
    core.Send(forsend);
    //Print_Prompt("msg sent!\n");
}


void UI_Control::Clear_Chat()
{
    int x,y;
    getmaxyx(my_wins[2],y,x);
    for(int i=3;i<y-1;i++)
    {
        mvwprintw(my_wins[2],i,1,"%*s", x-2, " ");
    }
    printCount=3;   //初始从第三行开始打印
}


void UI_Control::Refresh_chatMsg()
{
    //queue<tmp> que = core.waitting_response;
    if(core.new_message_sign)
    {
        string filename = core.Get_Sendto();
        filename+=".txt";
        int x=getmaxx(my_wins[2]);
        FILE *fp;
        char StrLine[x];             //每行最大读取的字符数
        if((fp = fopen(filename.c_str(),"r")) == nullptr) //判断文件是否存在及可读
        {
            usleep(10000);
        }

        int height,width;
        getmaxyx(stdscr,height,width);
        while (!feof(fp))
        {
            if(printCount>height*8/9-2) {
                Clear_Chat();
            }
            fgets(StrLine,x,fp);
            mvwprintw(my_wins[2],printCount,2,StrLine);
            wrefresh(my_wins[2]);
            printCount++;
            //printf("%s", StrLine);
        }
        fclose(fp);
        string del = "rm -f ";
        del+=filename;
        system(del.c_str());
        core.new_message_sign=false;
    }
    else if(!core.waitting_response.empty()) {
        if(core.waitting_response.size()>1)
        {
            core.waitting_response.pop_front();
            tmp temp = core.waitting_response.front();
            string sig = "A New Message From ";
            sig += temp.name;
            sig += ": ";
            sig += temp.content;
            //core.waitting_response.pop_front();
            Print_Prompt(sig.c_str());
            //getch();
            //Print_Prompt("                                               ");
        } else {
            tmp temp = core.waitting_response.front();
            string sig = "A New Message From ";
            sig += temp.name;
            sig += ": ";
            sig += temp.content;
            //core.waitting_response.pop_front();
            Print_Prompt(sig.c_str());
            //getch();
            //Print_Prompt("                                               ");
            //core.new_message_sign=false;
        }
        //else if ((!core.new_message_sign && core.waitting_response.front()==core.Get_Sendto()))
    }
}