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
    struct sockaddr_storage client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    server_socket = create_socket();
    int bytes;
    _sockets.push_back(pollfd());
    _sockets[0].fd = server_socket;
    _sockets[0].events = POLLIN;
    std::cout << "Servidor IRC escuchando en el puerto " << _port << std::endl;

    while (true) 
    {
        if(poll(&_sockets[0], _sockets.size(), 1000) == -1)
            std::cerr << RED << "Error poll" << NOCOLOR << std::endl;
        for(size_t i = 0; i < _sockets.size(); i ++)
        {
            if(_sockets[i].revents & POLLIN)
            {
                if(_sockets[i].fd == server_socket) //nuevo cliente
                {
                    client_len = sizeof(client_addr);
                    client_socket = accept(server_socket,(struct sockaddr *) &client_addr, &client_len);
                    if(client_socket == -1)
                    {
                         std::cerr << RED << "Error client socket" << NOCOLOR << std::endl;
                    }
                    std::cout << GREEN << "New user connected :)" << NOCOLOR << std::endl;
                    _sockets.push_back(pollfd());
                    _sockets.back().fd = client_socket;
                    _sockets.back(). events = POLLIN;
                }
                else
                {
                    bytes = recv(_sockets[i].fd , buffer, BUFFER_SIZE, 0);
                    if(bytes <= 0)
                    {
                        std::cerr << RED << "Client disconnected" << NOCOLOR << std::endl;
                        for (std::vector<pollfd>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
                        {
                            if (client_socket == (*it).fd)
                            {
                                close((*it).fd);
                                _sockets.erase(it);
                                break ;
                            }
                        }
                        break;
                    }
                    else
                    {
                        buffer[bytes] = '\0';
                        std::cout << "user : " << buffer << std::endl;
                        processCommand(buffer);
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

