#include "Server.hpp"

bool RunServer = true;

void UseCtrl(int signal)
{
    signal = 0;
    RunServer = false;
}

Server::Server(std::string const port, std::string const pass) : _port(port), _pass(pass){_supass = "supermario";}
Server::Server(const Server &other){*this = other;}
Server::~Server(){}
Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _port = other._port;
        _pass = other._pass;
    }
    return(*this);
}

int Server::CreateNewUser(struct sockaddr_storage client_addr, int server_socket, char *buffer)
{
    std::string token;
    std::vector<std::string> tokens;
    int client_socket;
    socklen_t client_len;

    client_len = sizeof(client_addr);
    client_socket = accept(server_socket,(struct sockaddr *) &client_addr, &client_len);
    if(client_socket == -1)
            std::cerr << RED << "Error client socket" << NOCOLOR << std::endl;
    _sockets.push_back(pollfd());
    _sockets.back().fd = client_socket;
    _sockets.back().events = POLLIN;
    ClientData client(client_socket);
    for(int i = 0; i < 3; i++)
    {
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        buffer[bytes_received] = '\0';
        if (bytes_received <= 0) 
        {
            std::cerr << "Error al recibir el nombre del cliente" << std::endl;
            _sockets.erase(_sockets.begin() + i);
            close(client_socket);
            continue;
        }
        std::istringstream iss(buffer);
        while (std::getline(iss, token, '\n'))
            tokens.push_back(token);
        for (size_t j = 0; j < tokens.size(); ++j)
        {
            if(firstCommand(tokens[j], &client) != 0)
            {
                _sockets.erase(_sockets.end() - 1);
                close(client_socket);
                std::cerr << "CERRADO!" << std::endl;
                return 1;
            }
        }
        tokens.clear();
    }
    std::cout << GREEN << "New user connected :)" << NOCOLOR << std::endl;
    clients_vec.push_back(client);
    sendToUser(&client, makeUserMsg(&client, RPL_WELCOME, "Hola caracola!"));
    return 0;
}

int Server::ReceiveDataClient(size_t socket_num, char *buffer)
{
    int bytes;
    bytes = recv(_sockets[socket_num].fd , buffer, BUFFER_SIZE, 0);
    std::vector<ClientData>::iterator it_client = find_ClientData_Socket(_sockets[socket_num].fd);
    if(it_client == clients_vec.end())
    {
        std::cerr << "ERROR not in socket list" << std::endl;
        return(1);
    }
    else if(bytes <= 0)
    {
        deleteClient(socket_num, it_client);
        return(1);
    }
    else
    {
        buffer[bytes] = '\0';
        std::cout << it_client->getNickName() << " : " << buffer << std::endl;
        if(processCommand(buffer, *it_client, socket_num, it_client) != 0)
            return(2);
        return(0);
    }
}

void Server::CloseServer()
{
    for(size_t socket_num = 0; socket_num < _sockets.size(); socket_num++)
        close(_sockets[socket_num].fd);
    _sockets.clear();
    for(size_t client_num = 0; client_num < clients_vec.size(); client_num++)
        clients_vec[client_num].~ClientData();
    clients_vec.clear();
    std::cout << RED << "||Servidor cerrado||" << NOCOLOR << std::endl;
}

int Server::Start()
{
    std::string input;
    int server_socket;
    struct sockaddr_storage client_addr;
    char buffer[BUFFER_SIZE];
    server_socket = create_serversocket();
    _sockets.push_back(pollfd());
    _sockets[0].fd = server_socket;
    _sockets[0].events = POLLIN;
    std::cout << "Servidor IRC escuchando en el puerto " << _port << std::endl;
    signal(SIGINT, UseCtrl);
    while (RunServer)
    {
        if(poll(&_sockets[0], _sockets.size(), 1000) == -1 && RunServer)
            std::cerr << RED << "Error poll" << NOCOLOR << std::endl;
        for(size_t socket_num = 0; socket_num < _sockets.size(); socket_num++)
        {
            if(_sockets[socket_num].revents & POLLIN)
            {
                if(_sockets[socket_num].fd == server_socket)
                {
                    if(CreateNewUser(client_addr, server_socket, buffer) != 0)
                        break;
                }
                else
                {
                    int i = ReceiveDataClient(socket_num, buffer);
                    if(i == 1)
                        break;
                    else if(i == 2)
                        return(0);
                }
            }
        }
    }
    CloseServer();
    return 0;
}


