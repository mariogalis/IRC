#ifndef CLIENTDATA_HPP
# define CLIENTDATA_HPP

#include "Irc.hpp"

class ClientData 
{
public:
    ClientData(int socket, const std::string& NickName);
    static int	CreateClientData(int fd, struct sockaddr *addr, socklen_t addrlen, std::vector<ClientData *> *clientes);
    static std::vector<ClientData*>::iterator find_ClientData_Socket(int fd, std::vector<ClientData *> *clientes);
    void setNickName(std::string newNickName);
    void setLoginName(std::string newLoginName);
    void setRealName(std::string newRealName);
    int getSocket();
    std::string getNickName();
    std::string getLoginName();
    std::string getRealName();
    void setHost(std::string host);
    void setService(std::string service);

private:
    ClientData();
    ~ClientData();
    ClientData(int socket);
    ClientData(const ClientData &other);
    ClientData	&operator=(const ClientData &other);
    int _socket;
    std::string _NickName;
    std::string _LoginName;
    std::string _RealName;
    std::string	_host;
    std::string	_service;
};

#endif
