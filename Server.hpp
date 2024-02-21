#ifndef SERVER_HPP
# define SERVER_HPP

#include "Irc.hpp"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define HOSTLEN 64

class ClientData;

class Server
{
    public:
        Server(std::string const port, std::string const password);
        ~Server();
        int Start();
        int create_socket();
        bool CheckPassword(std::string buffer);
        bool CheckNickName(char* buffer);
        void processCommand(const std::string& command);
        int firstCommand(const std::string& command, ClientData *client);
        std::vector<ClientData*>::iterator find_ClientData_Socket(int fd);
        //int WaitClient(int server_socket);
    private:
        Server(const Server &other);
        Server	&operator=(const Server &other);
        std::string _port;
        std::string _pass;
        std::vector<ClientData*> clientes;
        std::vector<pollfd>		_sockets;
        //std::vector<std::string> users;
        //std::map<std::string, Channel> channels;
};

#endif
