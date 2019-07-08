#include"UI_Control.h"
#define ARRAY_SIZE(a)(sizeof(a) / sizeof(a[0]))
char *choices2[] = {
        "Add Friend",
        "Settings",
        "Help",
        "Exit",
        (char *)NULL,
};



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
    mvprintw(row/3,(col-strlen(msegg))/2, "%s",msegg);
    scanw("%s",ipad);
    mvprintw(row/3+2, (col-strlen(msegg))/2, "%s",msgg2);
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
    /*clear();
    char mseg[]="User Number:";
    char usrnumb[PWDSIZE];
    char msg2[]="Password:";
    char passwd[PWDSIZE];
    int row,col;
    getmaxyx(stdscr,row,col);
    attron(A_BOLD);
    mvprintw(row/3,(col-strlen(mseg))/2, "%s",mseg);
    scanw("%s",usrnumb);
    mvprintw(row/3+2, (col-strlen(mseg))/2, "%s",msg2);
    attroff(A_BOLD);
    scanw("%s",passwd);
    string usr = usrnumb;
    string pass = passwd;*/
    string usr="maning";
    string pass = "mima";
    if(core.login(usr,pass)<0) {
        printw("Log in Failed...");
        endwin();
        return -1;
    }
    core.Reqst_Online_List();
    while(core.friends_empty()) {
        sleep(1);
        if(core.friends_empty()) {
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
        {
            vector<string> choicess = core.Get_Friend_List();
            Init_ChatUI(choicess);
            break;
        }
    }

    return 1;
}

void func(char *name,UI_Control& ui)
{
    mvprintw(20, 0, "Item selected is : %s", name);

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
        int startx, starty, height, width;
        getmaxyx(*&wins[i], height, width);
        box(*&wins[i], 0, 0);
        mvwaddch(*&wins[i], 2, 0, ACS_LTEE);
        mvwhline(*&wins[i], 2, 1, ACS_HLINE, width - 2);
    }

}

void UI_Control::Print_Prompt(const char* str) {
    int y=getmaxy(stdscr);
    mvprintw(y-3,3,str);
}

void UI_Control::Init_ChatUI(vector<string> &choices)
{
    clear();
    cout<<"I'm In!!!!!!!"<<endl;
    /*int n_choices,n_choices2,i;
    int ch;
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);

    n_choices =choices.size();
    n_choices2 =ARRAY_SIZE(choices2);
    my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
    my_items2 = (ITEM **)calloc(n_choices2 + 1, sizeof(ITEM *));
    for(i = 0; i < n_choices; ++i)
    {
        my_items[i] = new_item(choices[i].c_str(), 0);
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
    init_wins(my_wins, 4);
    keypad(my_wins[0], TRUE);
    keypad(my_wins[1], TRUE);
    //keypad(my_wins[3], TRUE);
    
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    my_panels[2] = new_panel(my_wins[2]);
    //my_panels[3] = new_panel(my_wins[3]);
    
   
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[2]);
    set_panel_userptr(my_panels[2], my_panels[0]);
    //set_panel_userptr(my_panels[3], my_panels[0]);
    
    update_panels();

    int h1,h2,w1,w2;
    getmaxyx(my_wins[0],h1,w1);    //getmaxyx(stdscr,row,col);
    getmaxyx(my_wins[1],h2,w2);
    //set_menu_win(my_menu, my_wins[0]);
    int height,width;
    getmaxyx(stdscr,height,width);
    //mvprintw(10,2,"h2 is %d,w2 is %d\n",h2,w2);
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
    doupdate();
    top = my_panels[0];
    char mseg[]="Send: ";
    mvprintw(1, w1/3, "Friends");
    int row,col;
    getmaxyx(stdscr,row,col);
    attron(A_BOLD);
    mvprintw(row-2,col/3,mseg);
    while((ch = getch()) != KEY_F(1))
    { 
        if(ch==9)   //tab
        {
            top = (PANEL *)panel_userptr(top);
            top_panel(top);
            update_panels();
        doupdate();
        }
        if(top->win == my_wins[2])
                {
                    char usrnumb[10];
                    mvprintw(row-2,col/3,mseg);
                    echo();
                    getstr(usrnumb);
                    noecho();
                }
        else if(top->win==my_wins[0])
        {
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
    unpost_menu(my_menu);
    free_menu(my_menu);
    for(i = 0; i < n_choices; ++i)
        free_item(my_items[i]);

    unpost_menu(my_menu2);
    free_menu(my_menu2);
    for(i = 0; i < n_choices2; ++i)
        free_item(my_items2[i]);
    endwin();*/
}
