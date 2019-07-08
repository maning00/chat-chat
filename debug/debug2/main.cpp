//
//  main.cpp
//  chat-svr
//
//  Created by 马宁 on 2019/6/27.
//  Copyright © 2019 马宁. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "account.hpp"
#include "Msgsvr.hpp"

int main() {
    msgsvr svr;
    svr.Init();
    
    return 0;
}
