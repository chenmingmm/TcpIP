#include <iostream>
#include <open_socket.h>

void echo(int connd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    rio_readinitb(&rio, connd);
    while ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0)
    {
        printf("server received %d bytes\n", (int)n);
        rio_wirten(connd, buf, n);
    }
}
int main(int argc, char **argv)
{
    int listenfd, connfd;

    socklen_t clientlen;

    struct sockaddr_storage clientaddr;
    char client_host[MAXLINE], client_port[MAXLINE];
    if (argc != 2)
    {
        fprintf(stderr, "usage : %s <port>\n", argv[0]);
        exit(0);
    }
    listenfd = open_listenfd(argv[1]);
    printf("listenfd is %d \n", listenfd);
    if (listenfd < 0) {
        exit(0);
    }
    while (true)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = ::accept(listenfd, (sockaddr *)&clientaddr, &clientlen);
        ::getnameinfo((sockaddr *)&clientaddr, clientlen, client_host, MAXLINE, client_port, MAXLINE, 0);

        printf("connect to (%s:%s)\n", client_host, client_port);
        echo(connfd);
        close(connfd);
    }
    exit(0);
}
