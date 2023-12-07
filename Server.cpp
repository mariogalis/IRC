#include "Server.hpp"

Server::Server(std::string const port, std::string const pass) : _port(port), _pass(pass)
{

}

Server::Server(const Server &other)
{
    *this = other;
}

Server::~Server(){}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _port = other._port;
        _password = other._password;
    }
}

