#include "Client.hpp"

Client::Client(std::string const port, std::string const pass, std::string const name) : _port(port), _pass(pass), _name(name)
{

}

Client::Client(const Client &other)
{
    *this = other;
}

Client::~Client(){}

Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _port = other._port;
        _pass = other._pass;
        _name = other._name;
    }
    return(*this);
}

