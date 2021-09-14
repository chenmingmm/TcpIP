//
//  open_socket.h
//  computersystem
//
//  Created by 陈柚子 on 2021/9/13.
//

#ifndef open_socket_h
#define open_socket_h

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#define  MAXLINE 1024

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
    char * buf = reinterpret_cast<char*>(usrbuf);
    size_t nleft = n;
    ssize_t nread = 0;

    while (nleft > 0) {
        if ((nread = read(fd, buf, n)) < 0)
        {
            if (errno == EINTR)
            {
                nread = 0;
            }
            else{
                return -1;
            }
        }else if (nread == 0){
            break;
        }
        nleft -= nread;
        buf += nread;
    }
    return 0;
}

ssize_t rio_wirten(int fd, void* usrbuf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char * buff = reinterpret_cast<char*>(usrbuf);
    
    while (nleft > 0)
    {
        if ((nwritten = write(fd, buff, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                nwritten = 0;
            }
            else{
                return -1;
            }
        }
        nleft -= nwritten;
        buff += nwritten;
    }
    return n;
}
#define RIO_BUFSIZE 8192

typedef struct {
    int rio_fd;
    int rio_cnt;
    char *rio_bufptr;
    char rio_buf[RIO_BUFSIZE];
}rio_t;

void rio_readinitb(rio_t* rp,int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

ssize_t rio_read(rio_t* rp, char *buf, size_t n)
{
    int cnt = n;
    
    while (rp->rio_cnt <= 0) {
        
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0){
            if (errno != EINTR){
                return -1;
            }
        } else if (rp->rio_cnt == 0){
            return 0;
        } else {
            rp->rio_bufptr = rp->rio_buf;
        }
    }
    if (rp->rio_cnt < n){
        cnt = rp->rio_cnt;
    }
    memcpy(buf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_readlineb(rio_t*rp, void* usrbuf, size_t maxlen)
{
    int n,rc;
    char c;
    char *bufp = reinterpret_cast<char*>(usrbuf);
    
    for (n = 1;n < maxlen; n++) {
        if ((rc = rio_read(rp, &c, 1)) == 1){
            *bufp++ =c;
            if (c == '\n'){
                n++;
                break;
            }
        } else if (rc == 10)
        {
            if (n == 1){
                return 0;
            } else {
                break;
            }
        } else {
            return -1;
        }
    }
    *bufp = 0;
    return n -1 ;
}

ssize_t rio_readnb(rio_t* rp, char *buf, int n)
{
    size_t nleft = n;
    ssize_t nread;
    char * buftemp = buf;
    
    while (nleft > 0) {
        if ((nread = rio_read(rp, buftemp, nleft)) < 0){
            return -1;
        } else if (nread == 0)
        {
            break;
        }
        nleft -= nread;
        buftemp += nread;
    }
    return (n - nleft);
}


int open_clientfd(char *hostname, char*port)
{
    int clientfd;
    struct addrinfo hints, *listp, *p;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
    getaddrinfo(hostname, port, &hints, &listp);
    
    for (p = listp; ; p = p->ai_next) {
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            continue;
        }
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != 1){
            break;
        }
        close(clientfd);
    }
    freeaddrinfo(listp);
    if (!p){
        return -1;
    } else {
        return clientfd;
    }
    return clientfd;
}

int open_listenfd(char *port)
{
    struct addrinfo hints,*listp, *p;
    int listenfd, optval = 1;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    hints.ai_flags |= AI_NUMERICSERV;
    getaddrinfo(NULL, port, &hints, &listp);
    
    for (p =  listp; p; p = p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol) < 0)) {
            continue;
        }
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
        
        if (::bind(listenfd, p->ai_addr,p->ai_addrlen) == 0){
            break;
        }
        close(listenfd);
    }
    freeaddrinfo(listp);
    if (!p){
        return -1;
    }
    if (listen(listenfd, 10) < 0){
        close(listenfd);
        return -1;
    }
    
    return listenfd;
}


#endif /* open_socket_h */
