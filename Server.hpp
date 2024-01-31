#ifndef SERVER_HPP
# define SERVER_HPP

#include "Irc.hpp"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
class Server
{
    public:
        Server(std::string const port, std::string const password);
        ~Server();
        int Start();
        int create_socket();
        int Try_01();
        bool CheckPassword(std::string buffer);
        bool CheckNickName(char* buffer);
        //int WaitClient(int server_socket);
    private:
        Server(const Server &other);
        Server	&operator=(const Server &other);
        std::string _port;
        std::string _pass;
        std::vector<ClientData> clientes;
        //std::vector<std::string> users;
        //std::map<std::string, Channel> channels;
};

#endif
