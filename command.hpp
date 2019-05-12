#ifndef IRC_COMMAND_HPP
#define IRC_COMMAND_HPP

#include "head.hpp"
#include  "msg.hpp"

#define SERVERPORT 8888
#define BUF_SIZE 512
#define EPOLL_SIZE 5000         //int epoll_create(int size) size is the maxnum supporting eploll
#define CONNECT "connect"    /* /connect  [网络]              连接IRC server*/
#define DISCONNECT "disconnect" /* /disconnect              断开服务器*/
#define NICK    "nick"      /* /nick    [昵称]              将自己的名字改为xxx */
#define WHOIS   "whois"     /* /whois   [昵称]              获取某人的一些信息，如对方的真实名字，对方的IP，对方是何时登录IRC服务器的，等等  */
#define LIST    "list"      /* /list                        列出当前服务器所有聊天室名称*/
#define NAMES   "names"     /* /names   [#聊天室名称]       列出当前聊天室所有昵称*/
#define PRIVMSG "privmsg"   /* /privmsg     [别名|#聊天室名称]  [消息内容]   向某人发出密聊话语，或者对某个闲聊室的所有人说话 */
#define USER    "user"      /* /user [消息内容] 对自己的阐述*/
#define NOTICE  "notice"    /* /query   [昵称] [消息内容]   私信某人*/
#define JOIN    "join"      /* /join    [#聊天室名称]       加入某个聊天室*/
#define PART    "part"      /* /part    [#聊天室名称]       退出某个聊天室*/
#define KICK    "kick"      /* /kick    [#聊天室名称] [昵称] [原因] 把某人从指定的闲聊室赶出去，管理员权限*/
#define QUIT    "quit"      /* /quit    离开IRC网络 */
#define PING    "ping"      /* /ping    检查是否连接*/
#define MOTD    "motd"      /* /motd    检查当天的消息记录*/
#define TOPIC   "topic"     /* /topic */
#define AWAY    "away"      /* /away    确定离开时的消息通告*/
#define LUSERS  "lusers"    /* /lusers  显示频道相关信息*/
#define MODE    "mode"
#define PONG    "pong"

#define RPL_WELCOME ":%s 001 %s:Welcome to the Internet Relay Network %s!%s" //主机名 Nick  Nick  User 主机名
#define ERR_NICKNAMEINUSE ":%s * %s ：Nickname is already in use"  //主机名 nick
#define SERVER_TO_CLIENT "%s!%s@%s %s" //nick 主机名 消息内容
#define RPL_ENDOFNAMES "%s :End of NAMES list"//主机名 nick 频道名称 发送完成
#define ERR_NONICKNAMEGIVEN ":No nickname given"
#define ERR_NEEDMOREPARAMS "%s :Not enough parameters" //命令名称
#define RPL_YOURHOST "Your host is %s, running version %s" //服务器名称 服务器版本
#define RPL_CREATED "This server was created %s" //日期
#define RPL_MYINFO "<servername> <version> <available user modes> <available channel modes>"
#define ERR_NOTREGISTERED ":You have not registered"
#define ERR_NORECIPIENT ":No recipient given (%s)"   //无收件人  命令
#define ERR_NOTEXTTOSEND ":No text to send"   
#define ERR_NOSUCHNICK "%s :No such nick/channel"    //名称
#define ERR_NOMOTD ":MOTD File is missing"
#define RPL_LUSERCLIENT ":%s %s %s:There are %d users and %d services on %d servers" //host nick channel_name
#define RPL_LUSEROP ":%s %s %s:%d :operator(s) online"
#define RPL_LUSERUNKNOWN ":%s %s %s:%d :unknown connection(s)"
#define RPL_LUSERCHANNELS ":%s %s %s:%d :channels formed"
#define RPL_LUSERME":%s %s %s::I have %d clients and %d servers"
#define RPL_WHOISUSER "nick:%s user:%s host:%s * :real name: %s"
#define RPL_WHOISSERVER ":nick:%s server:%s :%s"
#define RPL_ENDOFWHOIS "%s :End of WHOIS list"
#define RPL_WHOISOPERATOR "%s :is an IRC operator"
#define RPL_WHOISCHANNELS "<nick> :*( ( "@" / "+" ) <channel> " " )"
#define RPL_AWAY "<nick> :<away message>"
#define ERR_UNKNOWNCOMMAND "%s :Unknown command"
#define RPL_TOPIC "%s :%s"
#define ERR_CANNOTSENDTOCHAN "%s :Cannot send to channel"
#define ERR_NOTONCHANNEL "%s :You're not on that channel"
#define ERR_NEEDMOREPARAMS "<command> :Not enough parameters"
#define RPL_NOTOPIC "<channel> :No topic is set"
#define ERR_NOSUCHCHANNEL "%s :No such channel"
#define ERR_UMODEUNKNOWNFLAG ":Unknown MODE flag"
#define ERR_USERSDONTMATCH ":Cannot change mode for other users"
#define RPL_MODE ":%s %s"

#include "head.hpp"
#include "command.hpp"

char *name(char *a)
{
    gethostname(a,sizeof(a));
    return a;
}
std::string to_string(int num)
{
    std::ostringstream ostr;
    ostr << num;
    std::string astr = ostr.str();
    return astr;
}
bool judge(int fd)
{
    std::vector<User>::iterator it;
    int flag = 1;
    for(it = users.begin();it != users.end();++it)
    {
        if(it->connfd == fd)
        {
            if(it->nick != nullptr&&it->note != nullptr)
            {
                flag = 0;
                return true;
            }
        }
    }
    if(flag)
    {
        write(fd,"Please complete your login",27);
    }
}
std::string getTime()
{
    time_t rawtime;
    struct tm *ptminfo;

    time(&rawtime);
    ptminfo = localtime(&rawtime);
    std::string curtime = to_string(ptminfo->tm_year + 1900) + "-" + to_string(ptminfo->tm_mon + 1) + "-" + to_string(ptminfo->tm_mday) + " " + to_string(ptminfo->tm_hour) + ":" + to_string(ptminfo->tm_min) + ":" + to_string(ptminfo->tm_sec);

    return curtime;
}
void addEpollfd(int epollfd, int fd, bool enable_et)
{
    //声明epoll_event结构体的变量,ev用于注册事件
    struct epoll_event ev;
    //设置要处理的事件类型
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et)
        //EPOLLIN ：表示对应的文件描述符可以读
        //EPOLLET： 将EPOLL设为边缘触发ET模式，这是相对于水平触发LT来说的
        ev.events = EPOLLIN | EPOLLET;
    // 注册epoll事件 EPOLL_CTL_ADD：注册新的fd到epollfd中；
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    // 设置socket为非阻塞模式
    // 套接字立刻返回，不管I/O是否完成，该函数所在的线程会继续运行
    //eg. 在recv(fd...)时，该函数立刻返回，在返回时，内核数据还没准备好会返回WSAEWOULDBLOCK错误代码
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
    std::cout << "fd added to epoll.\n";
}

void conduct_name(char *a, int fd)
{
    int i = 0;
    std::string n1;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != '\0')
    {
        n1.push_back(a[i]);
        i++;
    }
    std::vector<User>::iterator it;
    int flag = 0;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->nick == n1)
        {
            flag = 1;
            char format_message[BUF_SIZE];
            sprintf(format_message, ERR_NICKNAMEINUSE, " ", n1.c_str());
            write(fd, format_message, strlen(format_message)); //发送名字已被注册信息 ???????
        }
    }
    if (flag == 0)
    {
        User user;
        user.nick = n1;
        user.connfd = fd;
        user.state = 0;
        user.hostname = "fengfan-PC";
        users.push_back(user);
        std::cout << user.nick << " " << user.connfd << " " << std::endl;
        write(fd," ",1);
    }
}
void conduct_user(char *a, int fd)
{
    int i = 0;
    std::string n1;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != '\0')
    {
        n1.push_back(a[i]);
        i++;
    }
    std::vector<User>::iterator it;
    int flag = 1;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->connfd == fd && it->nick != nullptr)
        {
            flag = 0;
            it->state = 1;
            char format_message[BUF_SIZE];
            char hos[100];
            gethostname(hos,sizeof(hos));
            sprintf(format_message, RPL_WELCOME,name(hos),it->nick.c_str(), n1.c_str(), it->hostname.c_str());
            write(fd, format_message, strlen(format_message));
        }
    }
    if(flag == 1)
    {
         write(fd,"Please tell you your nick first!",33);
    }
}
void conduct_whois(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != '\0')
    {
        n1.push_back(a[i]);
        i++;
    }
    std::vector<User>::iterator it;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->nick == n1)
        {
            char format_message[BUF_SIZE];
            memset(format_message, '0', sizeof(format_message));
            char s[100];
            gethostname(s,sizeof(s));
            sprintf(format_message, RPL_WHOISUSER, n1.c_str(), it->note.c_str(), it->hostname.c_str(), name(s));
            write(fd, format_message, strlen(format_message));

            memset(format_message, '0', sizeof(format_message));
            sprintf(format_message, RPL_WHOISSERVER, n1.c_str(), "default", name(s));
            write(fd, format_message, strlen(format_message));

            memset(format_message, '0', sizeof(format_message));
            sprintf(format_message, RPL_ENDOFWHOIS, n1.c_str());
            write(fd, format_message, strlen(format_message));
        }
    }
}
void conduct_list(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != '\0')
    {
        n1.push_back(a[i]);
        i++;
    }
    std::vector<Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        it->channel_name;
        write(fd, it->channel_name.c_str(), strlen(it->channel_name.c_str())); //输出 it->channel_name;
    }
}
void conduct_names(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != '\0')
    {
        n1.push_back(a[i]);
        i++;
    }
    std::vector<Channel>::iterator it;
    std::vector<User>::iterator is;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->channel_name == n1)
        {
            for (is = it->channel_user_names.begin(); is != it->channel_user_names.end(); ++is)
            {
                write(fd, is->nick.c_str(), strlen(is->nick.c_str())); //输出 is->nick列表
            }
            char format_message[BUF_SIZE];
            sprintf(format_message, RPL_ENDOFNAMES, n1);
            write(fd, format_message, strlen(format_message));
        }
    }
}
void conduct_privmsg(char *a, int fd)
{

    int i = 0;
    std::string n1;if(judge(fd) != true)
    {
        exit(0);
    }
    std::string n2;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    if (a[i] == '#')
    {
        while (a[i] != ' ')
        {
            n1.push_back(a[i]);
            i++;
        }
        i = i + 2;
        while (a[i] != '\0')
        {
            n2.push_back(a[i]);
            i++;
        }
        if (n2 == "")
        {
            char format_message[BUF_SIZE];
            sprintf(format_message, ERR_NOTEXTTOSEND);
            write(fd, format_message, strlen(format_message));
        }
        int flag = 0;
        std::vector<Channel>::iterator it;
        std::vector<User>::iterator is;
        for (it = channels.begin(); it != channels.end(); ++it)
        {
            if (it->channel_name == n1)
            {
                flag = 1;
                std::string na;
                std::string hos;
                for (is = it->channel_user_names.begin(); is != it->channel_user_names.end(); ++is)
                {
                    if (is->connfd == fd)
                    {
                        na = is->nick;
                        hos = is->hostname;
                    }
                }
                if (na == "" && hos == "")
                {
                    char format_message[BUF_SIZE];
                    sprintf(format_message, ERR_CANNOTSENDTOCHAN, n1);
                    write(fd, format_message, strlen(format_message));
                }
                else
                {
                    for (is = it->channel_user_names.begin(); is != it->channel_user_names.end(); ++is)
                    {
                        if (is->connfd != fd)
                        {
                            char format_message[BUF_SIZE];
                            sprintf(format_message, SERVER_TO_CLIENT, na.c_str(), na.c_str(),hos.c_str(), a);
                            write(fd, format_message, strlen(format_message));
                        }
                    }
                }
            }
        }
        if (flag == 0)
        {
            char format_message[BUF_SIZE];
            sprintf(format_message, ERR_NORECIPIENT, n1);
            write(fd, format_message, strlen(format_message));
        }
    }
    else
    {
        while (a[i] != ' ')
        {
            n1.push_back(a[i]);
            i++;
        }
        i = i + 2;
        while (a[i] != '\0')
        {
            n2.push_back(a[i]);
            i++;
        }
        if (n2 == " ")
        {
            char format_message[BUF_SIZE];
            sprintf(format_message, ERR_NOTEXTTOSEND);
            write(fd, format_message, strlen(format_message));
        }
        int flag = 0;
        std::vector<User>::iterator it;
        std::vector<User>::iterator is;
        for (it = users.begin(); it != users.end(); ++it)
        {
            if (it->nick == n1)
            {
                flag = 1;
                std::string na;
                std::string hos;
                for (is = users.begin(); is != users.end(); ++is)
                {
                    if (is->connfd == fd)
                    {
                        na = is->nick;
                        hos = is->hostname;
                    }
                }
                char format_message[BUF_SIZE];
                sprintf(format_message, SERVER_TO_CLIENT, na.c_str(), na.c_str(), hos.c_str(), a);
                write(it->connfd, format_message, strlen(format_message));
            }
        }
        if (flag == 0)
        {
            char format_message[BUF_SIZE];
            sprintf(format_message, ERR_NORECIPIENT, n1);
            write(fd, format_message, strlen(format_message));
        }
    }
}
void conduct_notice(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1;
    std::string n2;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != ' ')
    {
        n1.push_back(a[i]);
        i++;
    }
    i = i + 2;
    while (a[i] != '\0')
    {
        n2.push_back(a[i]);
        i++;
    }
    int flag = 0;
    std::vector<User>::iterator it;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->nick == n1)
        {
            flag = 1;
            std::string na;
            std::string hos;
            for (it = users.begin(); it != users.end(); ++it)
            {
                if (it->connfd == fd)
                {
                    na = it->nick;
                    hos = it->hostname;
                }
            }
            char format_message[BUF_SIZE];
            sprintf(format_message, SERVER_TO_CLIENT, na.c_str(), na.c_str(), hos.c_str(), a);
            write(fd, format_message, strlen(format_message));
        }
    }
    if (flag == 0)
    {
        char format_message[BUF_SIZE];
        sprintf(format_message, ERR_NORECIPIENT, n1.c_str());
        write(fd, format_message, strlen(format_message));
    }
}
void conduct_join(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != '\0')
    {
        n1.push_back(a[i]);
        i++;
    }
    std::vector<Channel>::iterator it;
    int flag = 0;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->channel_name == n1)
        {
            flag = 1;
            User user;
            user.connfd = fd;
            std::vector<User>::iterator is;
            for (is = users.begin(); is != users.end(); ++is)
            {
                if (is->connfd == fd)
                {
                    user.nick = is->nick;
                    user.note = is->note;
                }
            }
            it->channel_user_names.push_back(user);

            char format_message[BUF_SIZE];
            sprintf(format_message, SERVER_TO_CLIENT, user.nick.c_str(), user.nick.c_str(),user.hostname.c_str(), a);
            write(fd, format_message, strlen(format_message));

            memset(format_message, '0', sizeof(format_message));
            sprintf(format_message, RPL_TOPIC, n1.c_str(), it->theme.c_str());
            write(fd, format_message, strlen(format_message));

            std::vector<Channel>::iterator io;
            for (io = channels.begin(); io != channels.end(); ++io)
            {
                if (io->channel_name == n1)
                {
                    for (is = io->channel_user_names.begin(); is != io->channel_user_names.end(); ++is)
                    {
                        write(fd, is->nick.c_str(), strlen(is->nick.c_str())); //输出 is->nick列表
                        char format_message[BUF_SIZE];
                        sprintf(format_message, RPL_ENDOFNAMES, n1.c_str());
                        write(fd, format_message, strlen(format_message));
                    }
                }
            }
        }
    }
    if (flag == 0)
    {
        Channel channel;
        channel.channel_name = n1;
        std::vector<User>::iterator is;
        for (is = users.begin(); is != users.end(); ++is)
        {
            if (is->connfd == fd)
            {
                channel.root_name = is->nick;
                channel.channel_user_names.push_back(*is);
            }
        }
        channels.push_back(channel);
        write(fd," ",1);
    }
}
void conduct_part(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1, n2;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != ' ')
    {
        n1.push_back(a[i]);
        i++;
    }
    i = i + 2;
    while (a[i] != '\0')
    {
        n2.push_back(a[i]);
        i++;
    }
    std::vector<Channel>::iterator it;
    std::vector<User>::iterator is;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->channel_name == n1)
        {
            for (int j = 0; j < it->channel_user_names.size();)
            {
                if (it->channel_user_names[j].connfd != fd)
                {
                    ++j;
                }
                else
                {
                    it->channel_user_names.erase(it->channel_user_names.begin() + j);
                    conduct_privmsg(a, fd);
                }
                if (j == it->channel_user_names.size())
                {
                    char format_message[BUF_SIZE];
                    sprintf(format_message, ERR_NOTONCHANNEL, n1.c_str());
                    write(fd, format_message, strlen(format_message));
                }
            }
        }
        else
        {
            char format_message[BUF_SIZE];
            sprintf(format_message, ERR_NOSUCHCHANNEL, n1);
            write(fd, format_message, strlen(format_message));
        }
    }
}
void conduct_kick(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1, n2;
    while (a[i] != '0')
    {
        i++;
    }
    i = i + 1;
    while (a[i] != '0')
    {
        n1.push_back(a[i]);
        i++;
    }
    i = i + 1;
    while (a[i] != '0')
    {
        n2.push_back(a[i]);
        i++;
    }
    int flag = 0;
    std::vector<User>::iterator it;
    std::vector<Channel>::iterator is;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->connfd == fd)
        {
            for (is = channels.begin(); is != channels.end(); ++is)
            {
                if (is->root_name == it->nick && is->channel_name == n1)
                {
                    flag = 1;
                }
            }
        }
    }
    for (is = channels.begin(); is != channels.end(); ++is)
    {
        if (is->channel_name == n1)
        {
            for (int j = 0; j < is->channel_user_names.size();)
            {
                if (is->channel_user_names[j].nick != n2)
                {
                    ++j;
                }
                else
                {
                    is->channel_user_names.erase(is->channel_user_names.begin() + j);
                }
                if (j == is->channel_user_names.size())
                {
                    write(fd, "此用户不在当前频道", 100);
                }
            }
        }
    }
}
void conduct_quit(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1, n2;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 2;
    while (a[i] != '\0')
    {
        n1.push_back(a[i]);
        i++;
    }
    std::vector<Channel>::iterator is;
    std::vector<User>::iterator it;
    std::string na;
    std::string hos;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->connfd == fd)
        {
            na = it->nick;
            hos = it->hostname;
        }
    }
    for (is = channels.begin(); is != channels.end(); ++is)
    {
        for (it = is->channel_user_names.begin(); it != is->channel_user_names.end(); ++it)
        {
            if (it->nick == na)
            {
                std::vector<Channel>::iterator io;
                std::vector<User>::iterator iu;
                for (iu = io->channel_user_names.begin(); iu != io->channel_user_names.end(); ++iu)
                {
                    if (iu->connfd != fd)
                    {
                        char format_message[BUF_SIZE];
                        sprintf(format_message, SERVER_TO_CLIENT, na.c_str(), na.c_str(), hos.c_str(), a);
                        write(fd, format_message, strlen(format_message));
                    }
                }
            }
        }
    }
}
void conduct_ping(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    write(fd, "PONG", 4);
}
void conduct_motd(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    FILE *fp;
    char ch;
    if ((fp = fopen("motd.txt", "r")))
    {
        char format_message[BUF_SIZE];
        sprintf(format_message, ERR_NOMOTD);
        write(fd, format_message, strlen(format_message));
        exit(0);
    }
    while (!feof(fp))
    {
        ch = fgetc(fp);
        putchar(ch);
    }
}
void conduct_topic(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    std::string n1;
    std::string n2;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    if (a[i] == '#')
    {
        while (a[i] != ' ')
        {
            n1.push_back(a[i]);
            i++;
        }
        i = i + 2;
        while (a[i] != '\0')
        {
            n2.push_back(a[i]);
            i++;
        }
    }
    std::vector<Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->channel_name == n1)
        {
            it->theme = n2;
            break;
        }
    }
}
void conduct_away(char *a, int fd)
{
    //待填充
}
void conduct_lusers(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    std::vector<User>::iterator it;
    char format_message[BUF_SIZE];
    sprintf(format_message, RPL_LUSERCLIENT, users.size(), users.size(), 1);
    write(fd, format_message, strlen(format_message));

    memset(format_message, '0', sizeof(format_message));
    int num = 0;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->state == 1)
        {
            num++;
        }
    }
    sprintf(format_message, RPL_LUSEROP, num);
    write(fd, format_message, strlen(format_message));
    memset(format_message, '0', sizeof(format_message));
    int numb = 0;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (it->nick != "" && it->note == "")
        {
            numb++;
        }
    }
    memset(format_message, '0', sizeof(format_message));

    sprintf(format_message, RPL_LUSERUNKNOWN, numb);
    write(fd, format_message, strlen(format_message));

    memset(format_message, '0', sizeof(format_message));
    sprintf(format_message, RPL_LUSERCHANNELS, channels.size());
    write(fd, format_message, strlen(format_message));

    memset(format_message, '0', sizeof(format_message));
    sprintf(format_message, RPL_LUSERME, clientfd.size());
    write(fd, format_message, strlen(format_message));
}
std::string delete_char(std::string a, char b)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    std::string::iterator it; //指向string类的迭代器。你可以理解为指针
    std::string str;
    str = a;
    for (it = str.begin(); *it != ' '; it++)
    {
        if (*it == b)
        {
            str.erase(it); //删除it处的一个字符
            break;
        }
    }
    return str;
}
void conduct_mode(char *a, int fd)
{
    if(judge(fd) != true)
    {
        exit(0);
    }
    int i = 0;
    char format_message[BUF_SIZE];
    std::string n1, n2, n3;
    while (a[i] != ' ')
    {
        i++;
    }
    i = i + 1;
    if (a[i] != '#')
    {
        while (a[i] != ' ')
        {
            n1.push_back(a[i]);
            i++;
        }
        i = i + 1;
        while (a[i] != '\0')
        {
            n2.push_back(a[i]);
            i++;
        }
        int flag = 1;
        std::vector<User>::iterator is;
        for (is = users.begin(); is != users.end(); ++is)
        {
            if (is->nick == n1 && is->connfd == fd)
            {
                flag = 0;
                if (n2.at(0) == '+' && n2.at(1) == 'm')
                {
                    is->mode = is->mode + "m";
                    sprintf(format_message, RPL_MODE, n1.c_str(), a);
                    write(fd, format_message, strlen(format_message));
                }
                else if (n2.at(0) == '+' && n2.at(1) == 't')
                {
                    is->mode = is->mode + "t";
                    sprintf(format_message, RPL_MODE, n1.c_str(), a);
                    write(fd, format_message, strlen(format_message));
                }
                else if (n2.at(0) == '-' && n2.at(1) == 'm')
                {
                    is->mode = delete_char(is->mode, 'm');
                    sprintf(format_message, RPL_MODE, n1.c_str(), a);
                    write(fd, format_message, strlen(format_message));
                }
                else if (n2.at(0) == '-' && n2.at(1) == 't')
                {
                    is->mode = delete_char(is->mode, 't');
                    sprintf(format_message, RPL_MODE, n1.c_str(), a);
                    write(fd, format_message, strlen(format_message));
                }
                else
                {
                    write(fd, ERR_UMODEUNKNOWNFLAG, strlen(ERR_UMODEUNKNOWNFLAG));
                }
            }
        }
        if (flag)
        {
            write(fd, ERR_USERSDONTMATCH, strlen(ERR_USERSDONTMATCH));
        }
    }
    else
    {
        while (a[i] != ' ')
        {
            n1.push_back(a[i]);
            i++;
        }
        i = i + 1;
        if (a[i] == '-' || a[i] == '+')
        {
            while (a[i] != '\0')
            {
                n2.push_back(a[i]);
                i++;
            }
            int flag = 1;
            std::vector<Channel>::iterator it;
            for (it = channels.begin(); it != channels.end(); ++it)
            {
                if (it->channel_name == n1)
                {
                    flag = 0;
                    if (n2.at(0) == '+' && n2.at(1) == 'm')
                    {
                        it->mode = it->mode + "m";
                        sprintf(format_message, RPL_MODE, n1.c_str(), a);
                        write(fd, format_message, strlen(format_message));
                    }
                    else if (n2.at(0) == '+' && n2.at(1) == 't')
                    {
                        it->mode = it->mode + "t";
                        sprintf(format_message, RPL_MODE, n1.c_str(), a);
                        write(fd, format_message, strlen(format_message));
                    }
                    else if (n2.at(0) == '-' && n2.at(1) == 'm')
                    {
                        it->mode = delete_char(it->mode, 'm');
                        sprintf(format_message, RPL_MODE, n1.c_str(), a);
                        write(fd, format_message, strlen(format_message));
                    }
                    else if (n2.at(0) == '-' && n2.at(1) == 't')
                    {
                        it->mode = delete_char(it->mode, 't');
                        sprintf(format_message, RPL_MODE, n1.c_str(), a);
                        write(fd, format_message, strlen(format_message));
                    }
                    else
                    {
                        write(fd, ERR_UMODEUNKNOWNFLAG, strlen(ERR_UMODEUNKNOWNFLAG));
                    }
                }
            }
            if (flag)
            {
                memset(format_message, '0', sizeof(format_message));
                sprintf(format_message, ERR_NOSUCHCHANNEL, n1.c_str());
                write(fd, format_message, strlen(format_message));
            }
        }
        else
        {
            while (a[i] != ' ')
            {
                n2.push_back(a[i]);
                i++;
            }
            i = i + 1;
            while (a[i] != '\0')
            {
                n3.push_back(a[i]);
                i++;
            }
            std::vector<Channel>::iterator it;
            std::vector<User>::iterator is;
            for (it = channels.begin(); it != channels.end(); ++it)
            {
                if (it->channel_name == n1)
                {
                    if (n3.at(0) == '+' && n3.at(1) == 'o')
                    {
                        for (is = users.begin(); is != users.end(); ++is)
                        {
                            if (is->nick == n2)
                            {
                                is->mode = is->mode + "o";
                                sprintf(format_message, RPL_MODE, (n1 + " " + n2).c_str(), a);
                                write(fd, format_message, strlen(format_message));
                            }
                        }
                    }
                    else if (n3.at(0) == '+' && n3.at(1) == 'v')
                    {
                        for (is = users.begin(); is != users.end(); ++is)
                        {
                            if (is->nick == n2)
                            {
                                is->mode = is->mode + "v";
                                sprintf(format_message, RPL_MODE,  (n1 + " " + n2).c_str(), a);
                                write(fd, format_message, strlen(format_message));
                            }
                        }
                    }
                    else if (n3.at(0) == '-' && n3.at(1) == 'v')
                    {
                        for (is = users.begin(); is != users.end(); ++is)
                        {
                            if (is->nick == n2)
                            {
                                is->mode = delete_char(is->mode, 'v');
                                sprintf(format_message, RPL_MODE,  (n1 + " " + n2).c_str(), a);
                                write(fd, format_message, strlen(format_message));
                            }
                        }
                    }
                    else if (n3.at(0) == '-' && n3.at(1) == 'o')
                    {
                        for (is = users.begin(); is != users.end(); ++is)
                        {
                            if (is->nick == n1)
                            {
                                is->mode = delete_char(is->mode, 'o');
                                sprintf(format_message, RPL_MODE,  (n1 + " " + n2).c_str(), a);
                                write(fd, format_message, strlen(format_message));
                            }
                        }
                    }
                    else
                    {
                        write(fd, ERR_UMODEUNKNOWNFLAG, strlen(ERR_UMODEUNKNOWNFLAG));
                    }
                }
            }
        }
    }
}
void conduct_message(char *a, int fd)
{
    char *b = a;
    b[strlen(b) - 1] = '\0';
    if (strstr(b, NICK) != NULL)
    {
        conduct_name(b, fd);
    }
    else if (strstr(b, USER) != NULL)
    {
        conduct_user(b, fd);
    }
    else if (strstr(b, WHOIS) != NULL)
    {
        conduct_whois(b, fd);
    }
    else if (strstr(b, LIST) != NULL)
    {
        conduct_list(b, fd);
    }
    else if (strstr(b, NAMES) != NULL)
    {
        conduct_names(b, fd);
    }
    else if (strstr(b, PRIVMSG) != NULL)
    {
        conduct_privmsg(b, fd);
    }
    else if (strstr(b, NOTICE) != NULL)
    {
        conduct_notice(b, fd);
    }
    else if (strstr(b, JOIN) != NULL)
    {
        conduct_join(b, fd);
    }
    else if (strstr(b, PART) != NULL)
    {
        conduct_part(b, fd);
    }
    else if (strstr(b, KICK) != NULL)
    {
        conduct_kick(b, fd);
    }
    else if (strstr(b, QUIT) != NULL)
    {
        conduct_quit(b, fd);
    }
    else if (strstr(b, PING) != NULL)
    {
        conduct_ping(b, fd);
    }
    else if (strstr(b, MOTD) != NULL)
    {
        conduct_motd(b, fd);
    }
    else if (strstr(b, TOPIC) != NULL)
    {
        conduct_topic(b, fd);
    }
    else if (strstr(b, AWAY) != NULL)
    {
        conduct_away(b, fd);
    }
    else if (strstr(b, LUSERS) != NULL)
    {
        conduct_lusers(b, fd);
    }
    else if (strstr(b, MODE) != NULL)
    {
        conduct_mode(b, fd);
    }
    else if (strstr(b, PONG) != NULL)
    {
        exit(0);
    }
    else
    {
        char format_message[BUF_SIZE];
        sprintf(format_message, ERR_UNKNOWNCOMMAND, b);
        // printf("b:%s", a);
        // printf("%s\n",format_message);
        write(fd, format_message, strlen(format_message));
    }
}
#endif