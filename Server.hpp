#ifndef SERVER_HPP
# define SERVER_HPP

#include "Irc.hpp"
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <algorithm> 
class Server
{
    public:
        Server(std::string const port, std::string const password);
        ~Server();
        int start();
    private:
        Server(const Server &other);
        Server	&operator=(const Server &other);
        std::string _port;
        std::string _pass;
        std::vector<std::string> users;
        //std::map<std::string, Channel> channels;
};

#endif
