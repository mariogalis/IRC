#ifndef CLIENTDATA_HPP
# define CLIENTDATA_HPP

#include "Irc.hpp"

class ClientData 
{
public:
    ClientData(int socket, const std::string& NickName) : socket(socket), NickName(NickName) {}

    int getSocket() const {
        return socket;
    }

    std::string getNickName() const {
        return NickName;
    }

private:
    int socket;
    std::string NickName;
};

#endif
