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
        int processCommand(std::vector<std::string> args, ClientData &client, size_t socket_num, std::vector<ClientData>::iterator it_client);
        int firstCommand(std::vector<std::string> args, ClientData *client);
        int suCommand(const std::string& command, std::vector<ClientData>::iterator it_client);
        //int inChannelCmds(const std::string& command, ClientData &client, size_t socket_num, std::vector<ClientData>::iterator it_client);
        std::vector<ClientData>::iterator find_ClientData_Socket(int fd);
        //void sendToUser(ClientData *targetUser, std::string message);
        int CreateNewUser(struct sockaddr_storage client_addr, int server_socket, char *buffer);
        int ReceiveDataClient(size_t socket_num, char *buffer);
        void deleteClient(size_t socket_num, std::vector<ClientData>::iterator it_client);
        void CloseServer();
        static void CloseServer01();
        //std::string makeChanMsg(ClientData *user, std::string input);
        //std::string Server::makeChanMsg(ClientData *user, std::string code, std::string input);
        void send_PersonalMessage(std::vector<std::string> args, ClientData *sender);
        std::string	makePrivMsg(ClientData *sender, ClientData *receiver , std::string input);
        ClientData& find_ClientData_Nickname(std::string str);
        ClientData	*findUser(std::string str);
        std::vector<std::string> splitString(std::string str, const char *dlmtrs);
        void findchannel(std::vector<std::string> args, ClientData *client);
    private:
        Server(const Server &other);
        Server	&operator=(const Server &other);
        std::string _port;
        std::string _pass;
        std::string _supass;
        std::vector<ClientData> clients_vec;
        std::vector<ChannelData> _channel_vec;
        std::vector<pollfd>		_sockets;
        bool _ServerStatus;
        bool _CloseServer;
        

};

void sendToUser(ClientData *targetUser, std::string message);
std::string makeUserMsg(ClientData *user, std::string code, std::string input);
std::string	makeUserMsg01(ClientData *user, std::string input);
#endif
