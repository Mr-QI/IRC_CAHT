#ifndef IRC_MSG_HPP
#define IRC_MSG_HPP
#include "head.hpp"


typedef struct Message
{
    std::string time;   /* current time when user login in*/
    unsigned int msgtype;   /*message type*/
    std::string msg;        /*message content*/
    std::string from_name;     /*message recource*/
    std::string to_name;       /*message destination*/
}Msg_t;

typedef struct User{
    std::string nick;
    std::string note;
    std::string hostname;
    std::string  mode;
    char state;
    int connfd;
};
void init_User(User s)
{
    s.nick = nullptr;
    s.note = nullptr;
    s.hostname = "fengfan-PC";
    s.mode = nullptr;
    s.state = NULL;
    s.connfd = 0;
}
typedef struct Channel{
    std::string channel_name;
    std::string root_name;
    std::string theme;
    std::vector<User>channel_user_names;
    std::string mode;
};
void init_Channel(Channel s)
{
    s.channel_name = nullptr;
    s.root_name = nullptr;
    s.theme = nullptr;
    s.mode = nullptr;
}
std::list<int>clientfd;
std::vector<User>users;
std::vector<Channel>channels;


#endif // !IRC_MSG_H

