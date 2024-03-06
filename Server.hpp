#ifndef SERVER_HPP
# define SERVER_HPP

#include "Irc.hpp"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define HOSTLEN 64

class ClientData;
class ChannelData;

class Server
{
    public:
        Server(std::string const port, std::string const password);
        ~Server();
        int Start();
        int create_serversocket();
        bool CheckPassword(std::string buffer);
        bool CheckNickName(char* buffer);
        void processCommand(const std::string& command, ClientData &client);
        int firstCommand(const std::string& command, ClientData *client);
        std::vector<ClientData>::iterator find_ClientData_Socket(int fd);
        void sendToUser(ClientData *targetUser, std::string message);
        std::string makeUserMsg(ClientData *user, std::string code, std::string input);
        int CreateNewUser(struct sockaddr_storage client_addr, int server_socket, char *buffer);
        int ReceiveDataClient(size_t socket_num, char *buffer);
    private:
        Server(const Server &other);
        Server	&operator=(const Server &other);
        std::string _port;
        std::string _pass;
        std::string _supass;
        std::vector<ClientData> clients_vec;
        std::vector<pollfd>		_sockets;
        //std::vector<ChannelData> channel_vec;

};

#endif
