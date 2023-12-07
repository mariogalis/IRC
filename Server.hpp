#ifndef SERVER_HPP
# define SERVER_HPP

#include "Irc.hpp"

class Server
{
    public:
        Server(std::string const port, std::string const password);
        ~Server();
        void start();
    private:
        std::string_port;
        std::string_pass;
}