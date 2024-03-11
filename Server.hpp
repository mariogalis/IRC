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
        int processCommand(const std::string& command, ClientData &client, size_t socket_num, std::vector<ClientData>::iterator it_client);
        int firstCommand(const std::string& command, ClientData *client);
        int suCommand(const std::string& command, std::vector<ClientData>::iterator it_client);
        //int inChannelCmds(const std::string& command, ClientData &client, size_t socket_num, std::vector<ClientData>::iterator it_client);
        std::vector<ClientData>::iterator find_ClientData_Socket(int fd);
        void sendToUser(ClientData *targetUser, std::string message);
        std::string makeUserMsg(ClientData *user, std::string code, std::string input);
        int CreateNewUser(struct sockaddr_storage client_addr, int server_socket, char *buffer);
        int ReceiveDataClient(size_t socket_num, char *buffer);
        void deleteClient(size_t socket_num, std::vector<ClientData>::iterator it_client);
        void CloseServer();
        static void CloseServer01();
        //std::string makeChanMsg(ClientData *user, std::string input);
        //std::string Server::makeChanMsg(ClientData *user, std::string code, std::string input);
        void send_PersonalMessage(std::string name, std::string message, ClientData *sender);
        std::string	makePrivMsg(ClientData *sender, std::string input);
        ClientData& find_ClientData_Nickname(std::string str);
        ClientData	*findUser(std::string str);

    private:
        Server(const Server &other);
        Server	&operator=(const Server &other);
        std::string _port;
        std::string _pass;
        std::string _supass;
        std::vector<ClientData> clients_vec;
        std::vector<pollfd>		_sockets;
        bool _ServerStatus;
        bool _CloseServer;
        

};
#endif
