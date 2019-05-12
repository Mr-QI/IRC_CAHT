#ifndef IRC_MSG_HPP
#define IRC_MSG_HPP
#include "head.hpp"


struct Message
{
    std::string time;   /* current time when user login in*/
    unsigned int msgtype;   /*message type*/
    std::string msg;        /*message content*/
    std::string from_name;     /*message recource*/
    std::string to_name;       /*message destination*/
}Msg_t;

struct User{
    std::string nick;
    std::string note;
    std::string hostname;
    std::string  mode; //模式
    char state;   //状态标识
    char logonup; //注册标识
    int connfd;
};
struct Channel{
    std::string channel_name;
    std::string root_name;
    std::string theme;
    std::vector<User>channel_user_names;
    std::string mode;
};
std::list<int>clientfd;
std::vector<User>users;
std::vector<Channel>channels;


#endif // !IRC_MSG_H

