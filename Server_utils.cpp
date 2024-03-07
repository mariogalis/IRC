#include "Server.hpp"

std::vector<ClientData>::iterator	Server::find_ClientData_Socket(int fd)
{
    for (std::vector<ClientData>::iterator it = clients_vec.begin(); it != clients_vec.end(); ++it)
    {
		if ((it)->getSocket() == fd)
			return (it);
	}
	return (clients_vec.end());

}

char *getIP()
{
    char hostname[256];
    char *ip = new char[100];

    if (gethostname(hostname, sizeof(hostname)) == 0) 
    {
        struct hostent *hostinfo;

        if ((hostinfo = gethostbyname(hostname)) != NULL) 
            strcpy(ip, inet_ntoa(*(struct in_addr *)hostinfo->h_addr));
        else 
        {
            std::cerr << "Error al obtener la dirección IP del host." << std::endl;
            return nullptr;
        }
    } 
    else 
    {
        std::cerr << "Error al obtener el nombre del host." << std::endl;
        return nullptr;
    }

    return ip;
}


int Server::create_serversocket()
{
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        std::istringstream ss(_port);
        uint16_t valorHost;

        if (!(ss >> valorHost)) 
        {
            std::cerr << "Error: No se pudo convertir la cadena a uint16_t." << std::endl;
            exit(0);
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(valorHost);

        addr.sin_addr.s_addr = inet_addr(getIP());
        std::cout << BLUE << "IP local: " << getIP() << NOCOLOR << std::endl;
        int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
        if (bindResult == -1)
            perror("bindResult");
        int listenResult = listen(server_socket, 5);
        if (listenResult == -1)
            perror("listenResult");
        std::cerr << "server start" << std::endl;
        return server_socket;
}

std::string	Server::makeUserMsg(ClientData *user, std::string code, std::string input)
{
	std::ostringstream 	message;
	message << ":" << user->getHostname() << " " << code << " " << user->getNickName() << " :" << input << "\r\n";
	return (message.str());
}

void	Server::sendToUser(ClientData *targetUser, std::string message)
{
	std::ostringstream debug;
	debug << "OUTGOING USER_MSG TO : " << targetUser->getNickName() << " :\n" << message; 
	if (send(targetUser->getFd(), message.c_str(), message.size(), 0) < 0)
		throw std::invalid_argument(" > Error at sendToUser() ");
}

void	Server::deleteClient(size_t socket_num, std::vector<ClientData>::iterator it_client)
{
    std::cerr << RED << "Client disconnected" << NOCOLOR << std::endl;
    close(_sockets[socket_num].fd);
    _sockets.erase(_sockets.begin() + socket_num);
    clients_vec.erase(it_client);
    it_client->~ClientData();
}


