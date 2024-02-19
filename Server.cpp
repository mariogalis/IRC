#include "Server.hpp"

Server::Server(std::string const port, std::string const pass) : _port(port), _pass(pass), clientes()
{

}

Server::Server(const Server &other)
{
    *this = other;
}

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


int Server::firstCommand(const std::string& command, int client_socket) 
{
    std::istringstream iss(command);
    std::string token;
    std::vector<std::string> tokens;
    std::vector<ClientData*>::iterator it;

    it = find_ClientData_Socket(client_socket);
    if(it == clientes.end())
    {
        std::cout << "Error en comandos iniciales: " << "no se encontró el socket del cliente" << std::endl;
        return 1;
    }
    while (std::getline(iss, token, ' '))
        tokens.push_back(token);
    // Verificar si es un comando válido
    if (!tokens.empty()) 
    {
        // El primer token es el comando
        std::string ircCommand = tokens[0];

        if(ircCommand == "PASS")
        {
            std::string tryPass = tokens[1];
            std::string myPass = _pass;
            tryPass.pop_back(); //elimino el ultimo caracter porque yo que se que hay al final que no funciona si no
            std::cerr << "The client tried |"<< tryPass << "|" << std::endl;
            std::cerr << "The password is |"<< myPass << "|" << std::endl;
            if(myPass.compare(tryPass) != 0)
            {
                //write(client_socket, "Wrong password", 15); ESTO DEBERIA ENVIAR UN MENSAJE AL USUARIO PERO NO LO HACE XD
                std::cerr << "The client tried to log in with an incorrect password" << std::endl;
                return 1;
                close(client_socket);
            }
            else
            {
                std::cerr << "password correct!" << std::endl;
                return 0;
            }
        }
        else if(ircCommand == "NICK")
        {
            std::string newNickName = tokens[1];
            newNickName.pop_back(); //elimino el ultimo caracter porque yo que se que hay al final que no funciona si no
            ((*it)->setNickName(newNickName));
            std::cout << "Su nickname es: |" << (*it)->getNickName() << "|" << std::endl;
            return 0;
        }
        else if(ircCommand == "USER")
        {
            std::string newLogin = tokens[1];
            std::string newReal = tokens[4];
            //newLogin.pop_back();
            newReal.pop_back();
            (*it)->setLoginName(newLogin);
            std::cout << "Su LoginName es: |" << (*it)->getLoginName() << "|" << std::endl;
            (*it)->setRealName(newReal);
            std::cout << "Su RealName es: |" << (*it)->getRealName() << "|" << std::endl;
            return 0;
        }
        else 
        {
            std::cout << "Error en comandos iniciales" << ircCommand << std::endl;
            std::cerr << "Cliente desconectado" << std::endl;
            return 0;
        }
    }
    return 1;
}

void Server::processCommand(const std::string& command) 
{
    std::istringstream iss(command);
    std::string token;
    std::vector<std::string> tokens;

    // Separar el comando en tokens
    while (std::getline(iss, token, ' '))
        tokens.push_back(token);

    // Verificar si es un comando válido
    if (!tokens.empty()) 
    {
        // El primer token es el comando
        std::string ircCommand = tokens[0];

        // Dependiendo del comando, realiza la acción correspondiente
        if (ircCommand == "JOIN") 
        {
            // Procesar el comando JOIN
            // tokens[1] contendría el nombre del canal al que el usuario quiere unirse
            std::cout << "Usuario desea unirse al canal: " << tokens[1] << std::endl;
        } 
        else if (ircCommand == "PRIVMSG") 
        {
            // Procesar el comando PRIVMSG
            // tokens[1] contendría el destinatario del mensaje
            // tokens[2] contendría el mensaje en sí
            std::cout << "Mensaje para " << tokens[1] << ": " << tokens[2] << std::endl;
        }
        else 
        {
            std::cout << "Comando no reconocido: " << ircCommand << std::endl;
        }
    }
}

bool repeatName(const std::vector<std::string>& vec, const std::string& nombre) 
{
    return std::find(vec.begin(), vec.end(), nombre) != vec.end();
}

// static int	get_listener(int const queue_size, char const *port)
// {
// 	int				listenfd;
// 	int				yes = 1;
// 	struct addrinfo	hints;
// 	struct addrinfo	*info;
// 	struct addrinfo	*temp;

// 	memset(&hints, 0, sizeof(hints));
// 	hints.ai_family = AF_UNSPEC;
// 	hints.ai_socktype = SOCK_STREAM;
// 	hints.ai_flags = AI_PASSIVE;
// 	if (getaddrinfo(NULL, port, &hints, &info) != 0)
// 	{
// 		std::cerr << RED << "Error. Unable to get network address." << NOCOLOR << std::endl;
// 		freeaddrinfo(info);
// 		exit (1);
// 	}
// 	for (temp = info; temp != NULL; temp = temp->ai_next)
// 	{
// 		listenfd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
// 		if (listenfd < 0)
// 			continue ;
// 		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
// 		if (bind(listenfd, temp->ai_addr, temp->ai_addrlen) != 0)
// 		{
// 			close(listenfd);
// 			continue ;
// 		}
// 		break ;
// 	}
// 	freeaddrinfo(info);
// 	if (!temp)
// 	{
// 		std::cerr << RED << "Error. Unable to bind socket." << NOCOLOR << std::endl;
// 		close(listenfd);
// 		exit (1);
// 	}
// 	if (listen(listenfd, queue_size) != 0)
// 	{
// 		std::cerr << RED << "Error. Unable to setup socket." << NOCOLOR << std::endl;
// 		close(listenfd);
// 		exit (1);
// 	}
// 	return (listenfd);
// }

int Server::create_socket()
{
    // Crear un socket del lado del servidor
    //memset(buffer, 0, BUFFER_SIZE);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Convertir la cadena a un valor uint16_t
    std::istringstream ss(_port);
    uint16_t valorHost;

    if (!(ss >> valorHost)) 
    {
        std::cerr << "Error: No se pudo convertir la cadena a uint16_t." << std::endl;
        exit(0);
    }
    // Configurar la dirección y el puerto en el que el servidor escuchará
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(valorHost);  // Especificar el puerto (debe estar en el formato de red)
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Escuchar en todas las interfaces de red

    // Vincular el socket a la dirección y puerto especificados
    int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (bindResult == -1)
    {
        perror("bindResult");  // Imprimir un mensaje de error si la vinculación falla
    }

    // Configurar el socket para escuchar conexiones entrantes, con una cola de hasta 5 conexiones pendientes
    int listenResult = listen(server_socket, 5);
    if (listenResult == -1)
    {
        perror("listenResult");  // Imprimir un mensaje de error si la configuración para escuchar falla
    }

    // Imprimir un mensaje indicando que el servidor ha comenzado exitosamente
    std::cerr << "server start" << std::endl;

    // Devolver el descriptor de archivo del socket del servidor
    return server_socket;
}


std::vector<ClientData*>::iterator	Server::find_ClientData_Socket(int fd)
{
    for (std::vector<ClientData*>::iterator it = clientes.begin(); it != clientes.end(); ++it)
    {
		if ((*it)->getSocket() == fd)
			return (it);
	}
	return (clientes.end());

}


int Server::Try_01()
{
    int server_socket, client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    std::string token;
    std::vector<std::string> tokens;
    //int  event_client;
    //memset(buffer, 0, BUFFER_SIZE);
    server_socket = create_socket();
    //server_socket = get_listener(10, _port.c_str());
    _sockets.push_back(pollfd());
    _sockets[0].fd = server_socket;
    _sockets[0].events = POLLIN;
    std::cout << "Servidor IRC escuchando en el puerto " << _port << std::endl;

    for (size_t i = 0; i < clientes.size(); ++i) 
    {
        _sockets[i + 1].fd = clientes[i]->getSocket();
        _sockets[i + 1].events = POLLIN;
    }
    while (true) 
    {

        memset(buffer, 0, BUFFER_SIZE);
        int num_ready = poll(&_sockets[0], _sockets.size(), -1);
        if (num_ready < 0) 
        {
            std::cerr << "Error en poll()" << std::endl;
            break;
        }
        for (size_t i = 0; i < _sockets.size(); i++)
        {
            std::cerr << "bucle" << std::endl;
            if (_sockets[i].revents & POLLIN) 
            {
                std::cerr << "aqui01" << std::endl;
                if (_sockets[i].fd == server_socket)
                {
                    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
                    if(find_ClientData_Socket(client_socket) != clientes.end())
                    {
                        std::cerr << "cliente repetido" << client_socket << std::endl;
                    }
                    std::cerr << "client socket " << client_socket << std::endl;
                    if (client_socket < 0) 
                    {
                        std::cerr << "Error al aceptar la conexión" << std::endl;
                        continue;
                    }
                    if (ClientData::CreateClientData(client_socket, (struct sockaddr *)&client_addr, client_len, &clientes) != 0)
                        continue ;
                    std::cout << "Cliente conectado" << std::endl;
                    
                    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
                    _sockets.push_back(pollfd());
					_sockets.back().fd = client_socket;
					_sockets.back().events = POLLIN;
                    std::cout << buffer << std::endl;
                    if (bytes_received <= 0) 
                    {
                        std::cerr << "Error al recibir el nombre del cliente" << std::endl;
                        close(client_socket);
                        continue;
                    }
                    std::istringstream iss(buffer);
                    while (std::getline(iss, token, '\n'))
                        tokens.push_back(token);
                    for (size_t i = 0; i < tokens.size(); ++i)
                    {
                        if(firstCommand(tokens[i], client_socket) != 0)
                        {
                            clientes.erase(find_ClientData_Socket(client_socket));
                            close(client_socket);
                            std::cerr << "CERRADO!" << std::endl;
                            break;
                        }
                    }
                }
                else
                {
                    std::cerr << "aqui02" << std::endl;
                    for (size_t i = 0; i < clientes.size(); ++i) 
                    {
                        std::cerr << "Cliente size = " << clientes.size() << std::endl;
                        if (_sockets[i + 1].revents & POLLIN) 
                        {
                                int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
                                if (find_ClientData_Socket(client_socket) != clientes.end())
                                {
                                    if (bytes_received <= 0) 
                                    {
                                        std::cerr << "Cliente desconectado" << std::endl;
                                        clientes.erase(find_ClientData_Socket(client_socket)); // Eliminar el elemento del vector
                                        close(client_socket);
                                        break;
                                    }
                                    std::cout <<  (*(find_ClientData_Socket(client_socket)))->getNickName() << " : " << buffer << std::endl;
                                    processCommand(buffer);
                                }
                        }
                    }
                }

            }
        }
    }

    // Cerrar el socket del servidor
    close(server_socket);

    return 0;
}

int Server::Start()
{
    if(Try_01() != 1)
        return 1;
    // int server_socket = create_socket();

    // int client_socket = WaitClient(server_socket);

    // std::cerr << "server end" << std::endl;

    // close(client_socket);
    // close(server_socket);

    return 0;
}

