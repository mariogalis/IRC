#include "ClientData.hpp"

ClientData::ClientData(int socket, const std::string& NickName) : _socket(socket), _NickName(NickName)
{

}
ClientData::ClientData(int socket) : _socket(socket)
{

}

ClientData::ClientData(const ClientData &other)
{
    *this = other;
}

ClientData::~ClientData(){}

ClientData &ClientData::operator=(const ClientData &other)
{
    if (this != &other)
    {
        _socket = other._socket;
        _NickName = other._NickName;
    }
    return(*this);
}

void	ClientData::setHost(std::string host) { _host = host; }

void	ClientData::setService(std::string service) { _service = service; }

int	ClientData::CreateClientData(int fd, struct sockaddr * addr, socklen_t addrlen, std::vector<ClientData *> *clientes)
{
	ClientData		*temp = new ClientData(fd);
	char		host[NI_MAXHOST];
	char		service[NI_MAXSERV];
	std::string	hoststring;

	if (getnameinfo(addr, addrlen, host, sizeof(host), service, sizeof(service), NI_NUMERICHOST) != 0)
	{
		std::cerr << RED << "Error. Couldn't retrieve hostname." << NOCOLOR << std::endl;
		close(fd);
		delete temp;
		return (0) ;
	}
	hoststring = host;
	if (hoststring.size() > HOSTLEN)
			hoststring.erase(HOSTLEN, std::string::npos);
	temp->setHost(hoststring);
	temp->setService(std::string (service));
	clientes->push_back(temp);
	return (0);
}

void ClientData::setNickName(std::string newNickName)
{
	this->_NickName = newNickName;
}

void ClientData::setLoginName(std::string newLoginName)
{
	this->_LoginName = newLoginName;
}

void ClientData::setRealName(std::string newRealName)
{
	this->_RealName = newRealName;
}

int ClientData::getSocket()
{
	return _socket;
}

std::string ClientData::getNickName()
{
	return _NickName;
}

std::string ClientData::getLoginName()
{
	return _LoginName;
}

std::string ClientData::getRealName()
{
	return _RealName;
}
