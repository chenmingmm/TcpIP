//
//  main.cpp
//  computersystem
//
//  Created by 陈柚子 on 2021/9/4.
//

#include <iostream>
#include "open_socket.h"
using namespace std;

int main(int argc, char * argv[]) {
    // insert code here...
    int clientfd;
    char* host,*port, buf[1024];
    rio_t rio;
    
    if (argc != 3){
        exit(0);
    }
    host = argv[1];
    port = argv[2];
    
    clientfd = open_clientfd(host, port);
    rio_readinitb(&rio, clientfd);
    
    while (fgets(buf, 1024, stdin) != NULL) {
        rio_wirten(clientfd, buf, strlen(buf));
        rio_readlineb(&rio, buf, 1024);
        fputs(buf, stdout);
    }
    
    close(clientfd);
    return 1;
}


