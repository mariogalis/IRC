#include "Server.hpp"

ClientData& Server::find_ClientData_Nickname(std::string str)
{
    for (std::vector<ClientData>::iterator it = clients_vec.begin(); it != clients_vec.end(); ++it)
    {
        if (it->getNickName() == str)
            return *it; // Devuelve una referencia a la clase ClientData encontrada
    }
    // Si no se encuentra ninguna coincidencia, devuelve una referencia al último elemento (equivalente a end())
    return clients_vec.back();
}

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
            std::cerr << "Error getting host IP address" << std::endl;
            return nullptr;
        }
    } 
    else 
    {
        std::cerr << "Error getting hostname" << std::endl;
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
            std::cerr << "Error: Could not convert string to uint16_t" << std::endl;
            CloseServer();
        } 
        const int reuse = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
        {
            std::cerr << "Error at setsocketopt(): " << std::endl;
            CloseServer();
        }
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(valorHost);
        addr.sin_addr.s_addr = inet_addr(getIP());
        std::cout << BLUE << "Local IP: " << getIP() << NOCOLOR << std::endl;
        int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
        if (bindResult == -1)
            perror("bindResult");
        int listenResult = listen(server_socket, 5);
        if (listenResult == -1)
            perror("listenResult");
        std::cout << GREEN << "Server start" << NOCOLOR << std::endl;
        return server_socket;
}

std::string	makeUserMsg(ClientData *user, std::string code, std::string input)
{
	std::ostringstream 	message;
	message << ":" << user->getHostname() << " " << code << " " << user->getNickName() << " :" << input << "\r\n";
	return (message.str());
}

std::string	makeUserMsg01(ClientData *user, std::string input)
{
	std::ostringstream 	message;
	message << ":" << user->getHostname() << " " << user->getNickName() << " :" << input << "\r\n";
	return (message.str());
}

void	sendToUser(ClientData *targetUser, std::string message)
{
	std::ostringstream debug;
	std::cerr << "OUTGOING USER_MSG TO : " << targetUser->getNickName() << " :\n" << message; 
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

std::string	Server::makePrivMsg(ClientData *sender, ClientData *receiver , std::string input)
{
	std::ostringstream 	message;
    std::cerr << GREEN << input << NOCOLOR << std::endl;
    if(input[0] != ':')
	    message << ":" << sender->getNickName() << " PRIVMSG " <<  receiver->getNickName() << " :" << input << "\r\n";
    else
        message << ":" << sender->getNickName() << " PRIVMSG " <<  receiver->getNickName() << " " << input << "\r\n";
    std::cerr << RED << message.str() << NOCOLOR << std::endl;
	return (message.str());
}

void Server::send_PersonalMessage(std::vector<std::string> args, ClientData *sender)
{
    std::string name = args[1];
    std::string message;
    for (size_t i = 2; i < args.size(); ++i) 
    {
        message += args[i];
        if (i < args.size() - 1) {
            message += " ";
        }
    }

    for (std::vector<ClientData>::iterator it = clients_vec.begin(); it != clients_vec.end(); ++it)
    {
        if (it->getNickName() == name)
        {
            ClientData &receiver = *it; // Devuelve una referencia a la clase ClientData encontrada
            sendToUser(&receiver, makePrivMsg(sender, &receiver, message));
            return ;
        } 
    }
    sendToUser(sender, makeUserMsg(sender, ERR_ERRONEUSNICKNAME, "The client you want to send the message to does not exist"));
    std::cerr << RED << "A client tried to contact a non-existent client" << NOCOLOR << std::endl;
    return ;
}

std::vector<std::string>	Server::splitString(std::string str, const char *dlmtrs)
{
	std::vector<std::string> args;


	char	*ptr = strtok((char *)str.c_str(), dlmtrs);

//	NOTE : strtok works iteratively, so it needs to be called once per token
	while (ptr != nullptr && !std::string(ptr).empty())
	{
		args.push_back(std::string(ptr));
		ptr = strtok(NULL, dlmtrs);
	}

	return args;
}
