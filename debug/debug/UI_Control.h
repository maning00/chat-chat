#include <ncurses.h>
#include <panel.h>
//#include <menu.h>
#include <string>
#include "clt.h"

const int PWDSIZE=20;
using namespace std;


class UI_Control{
public:
    UI_Control(){};
    int Init_MainUI();
    int UI_Login();
    void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
    void init_wins(WINDOW **wins, int n);
    void Print_Prompt(const char* str);
    void Init_ChatUI(vector<string> &choices);

private:
/*WINDOW *my_wins[3];
PANEL *my_panels[3]; 
PANEL *top;
ITEM **my_items,**my_items2;
MENU *my_menu,*my_menu2;
ITEM *cur_item2;*/
client core;
};


