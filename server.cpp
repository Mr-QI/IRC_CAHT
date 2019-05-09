#include "msg.hpp"
#include "command.hpp"
#include "head.hpp"
#include "socket.hpp"
#include "io.hpp"


int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    size_t addrLen = sizeof(struct sockaddr);
    memset(&serverAddr, 0, addrLen);
    memset(&clientAddr, 0, addrLen);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serverAddr.sin_zero), 8);

    int socketfd = 0;
    char buffer[BUFFER_SIZE] = {0};
    char welcome[BUFFER_SIZE] = {" Welcome To Connect Server!"};
    Socket(socketfd);
    Bind(socketfd, (struct sockaddr*)&serverAddr, addrLen);
    Listen(socketfd, 5);

    /* ev: register event. events: triger event*/
    struct epoll_event ev, events[MAXEPOLL_FD];   
    int connfd = 0, epollfd = 0, nfds = 0;

    /*create epoll*/
    epollfd = epoll_create(MAXNUM_CLIENT);
    if(epollfd < 0){ perror("Epoll_Create Error."); exit(-1);}

    /* socket can be read*/
    ev.events = EPOLLIN;
    ev.data.fd = socketfd;

    /* register event*/    
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &ev) < 0)
    { perror("Epoll_Ctl Error."); exit(-1);}

    /**/
    while(1)
    {
        /* wait for triggering event.
            nfds: event numbers.
        */
        nfds = epoll_wait(epollfd, events, sizeof(events), -1);
        if(nfds < 0){ perror("Epoll_Wait Error."); exit(-1);}
        
        /* to itrate loop*/
        for(int i = 0; i < nfds; i++)
        {
            /* if socket fd has been changed.*/
            if(events[i].data.fd == socketfd)
            {
                connfd = Accept(socketfd, (struct sockaddr*)&clientAddr, (socklen_t*)&addrLen); 
                /* set client nonblocking*/
                fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFL)|O_NONBLOCK);                

                /*set trigger events*/
                ev.events = EPOLLIN|EPOLLET;
                ev.data.fd = connfd;
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) < 0)
                {
                    perror(" Epoll_Ctl Error."); exit(-1);
                }    
                send(connfd,welcome, sizeof(welcome), 0);

            }else{
                /* client triggers event*/
                memset(buffer, 0, sizeof(buffer));
                recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                std::cout<< "meassge from client: " << buffer << std::endl;
                conduct_message(buffer,events[i].data.fd);
            }
        }
    }
    close(socketfd);
    close(connfd);
    close(epollfd);
    return 0;
}



