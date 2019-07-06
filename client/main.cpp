//
//  main.cpp
//  chat-client
//
//  Created by 马宁 on 2019/6/27.
//  Copyright © 2019 马宁. All rights reserved.
//

#include <string.h>
#include <ncurses.h>
#include <panel.h>
#include <stdlib.h>
#include <menu.h>
#include "clt.h"


int main()
{
    initscr(); start_color(); cbreak(); noecho(); keypad(stdscr, TRUE);
    client clt=client();
    if(int fd=clt.connectosvr("127.0.0.1", 5019)>0) {
        clt.set_serversock(fd);
        clt.Init_ChatUI();
    }
    return 0;
}



