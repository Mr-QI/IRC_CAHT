#include "command.hpp"
#include "socket.hpp"
#include "threadpool.hpp"

#define FLAGS_WAIT_TASK_EXIT 1
typedef struct
{
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    size_t addrLen;
    char buffer[BUFFER_SIZE];
    struct epoll_event ev;
    struct epoll_event events[MAXEPOLL_FD];
    int epollfd;
    int socketfd;
} server_t;

//memset(buffer, 0, sizeof(char)*BUFFER_SIZE);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int initServer(server_t *server)
{
    server->serverAddr.sin_family = AF_INET;
    server->serverAddr.sin_port = htons(SERVER_PORT);
    server->serverAddr.sin_addr.s_addr = INADDR_ANY;
    server->addrLen = sizeof(struct sockaddr);
    bzero(&server->serverAddr.sin_zero, 8);
    server->addrLen = sizeof(struct sockaddr);
    memset(server->buffer, 0, sizeof(server->buffer));
    memset(server->events, 0, sizeof(server->events));
    server->socketfd = 0;
    server->epollfd = 0;
    return 0;
}

void *closeServer(server_t *server)
{
    close(server->socketfd);
    close(server->epollfd);
}

void *handleServer(void* arg)
{
    server_t *&server = *(server_t**)arg;
    int nfds = epoll_wait(server->epollfd, server->events, sizeof(server->events), -1);
    if (nfds < 0)
    {
        perror("Epoll_Wait Error.");
        exit(-1);
    }
    int connfd;
    /* to itrate loop*/
    char welcome[BUFFER_SIZE] = {" Welcome To Connect Server!"};
    for (int i = 0; i < nfds; i++)
    {
        /* if socket fd has been changed.*/
        if (server->events[i].data.fd == server->socketfd)
        {
            int connfd = Accept(server->socketfd, (struct sockaddr *)&server->clientAddr, (socklen_t *)&server->addrLen);
            clientfd.push_back(connfd);
            fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFL) | O_NONBLOCK);
            server->ev.events = EPOLLIN | EPOLLET;
            server->ev.data.fd = connfd;
            if (epoll_ctl(server->epollfd, EPOLL_CTL_ADD, server->ev.data.fd, &server->ev) < 0)
            {
                perror(" Epoll_Ctl Error.");
                exit(-1);
            }
            send(connfd, welcome, sizeof(welcome), 0);
        }
        else
        {
            recv(server->events[i].data.fd, server->buffer, sizeof(server->buffer), 0);
            std::cout << "meassge from client: " << server->buffer << std::endl;
            conduct_message(server->buffer, server->events[i].data.fd);
        }
    }
}
 
int main (int argc, char **argv)
{
    //daemon(0,0);
    server_t *server;
    server = (server_t*)malloc(sizeof(server_t));
    initServer(server);
    //Socket(server->socketfd);
    Socket(server->socketfd);
    Bind(server->socketfd, (struct sockaddr *)&server->serverAddr, server->addrLen);
    Listen(server->socketfd, 5);
    /* socket can be read*/
    server->ev.events = EPOLLIN | EPOLLET;
    server->ev.data.fd = server->socketfd;
    /*create epoll*/
    server->epollfd = epoll_create(MAXNUM_CLIENT);
    if (server->epollfd < 0)
    {
        perror("Epoll_Create Error.");
        exit(-1);
    }

    /* register event*/
    if (epoll_ctl(server->epollfd, EPOLL_CTL_ADD, server->socketfd, &server->ev) < 0)
    {
        perror("Epoll_Ctl Error.");
        exit(-1);
    }

   
    threadpool_init(10);
    while (1)
    {
        /* code */
        pool_add_worker (handleServer, (server_t*)&server);
    }
    closeServer(server);
    sleep (5);
    threadpool_destroy ();
    free (server);
    return 0;
}
