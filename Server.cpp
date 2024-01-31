#include "Server.hpp"

Server::Server(std::string const port, std::string const pass) : _port(port), _pass(pass)
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

bool repeatName(const std::vector<std::string>& vec, const std::string& nombre) 
{
    return std::find(vec.begin(), vec.end(), nombre) != vec.end();
}

int Server::create_socket()
{
    // Crear un socket del lado del servidor
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Convertir la cadena a un valor uint16_t
    std::istringstream ss(_port);
    uint16_t valorHost;

    if (!(ss >> valorHost)) {
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

std::string Separate(std::string input, int pos)
{
    std::istringstream iss(input); // Stream de entrada asociado a la cadena RECORDAR QUE NO TOMO EN CUENTA LOS ESPACIOS NI EN CONTRASEÑA NI USUARIOS
    int i = 0;
    std::string word;
    if(pos == 0)
    {
        iss >> word;
        return word;
    }
    while (iss >> word && i <= pos) 
    {
        //std::cout << "Palabra: " << word << std::endl;
    }
    return word;
}

bool Server::CheckPassword(std::string buffer)
{
    if(Separate(buffer, 0) != "PASS")
        return false;
    buffer = Separate(buffer, 1);
    if(_pass == buffer)
        return true;
    else
        return false;
}

bool Server::CheckNickName(char * buffer)
{
    if(Separate(buffer, 0) == "NICK")
        return true;
    else 
        return false;
}

int Server::Try_01()
{
    int server_socket, client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    //int  event_client;

    server_socket =  create_socket();
    std::cout << "Servidor IRC escuchando en el puerto " << _port << std::endl;


    while (true) {
        struct pollfd fds[MAX_CLIENTS + 1];
        fds[0].fd = server_socket;
        fds[0].events = POLLIN;

        for (size_t i = 0; i < clientes.size(); ++i) 
        {
            fds[i + 1].fd = clientes[i].getSocket();
            fds[i + 1].events = POLLIN;
        }

        int num_ready = poll(fds, clientes.size() + 1, -1);
        if (num_ready < 0) 
        {
            std::cerr << "Error en poll()" << std::endl;
            break;
        }

        if (fds[0].revents & POLLIN) {
            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
            if (client_socket < 0) 
            {
                std::cerr << "Error al aceptar la conexión" << std::endl;
                continue;
            }
            std::cout << "Cliente conectado" << std::endl;

            // Recibir la contraseña
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) 
            {
                std::cerr << "Error al recibir el nombre del cliente" << std::endl;
                close(client_socket);
                continue;
            }

            if(CheckPassword(buffer) == false)
            {
                //write(client_socket, "Wrong password", 15); ESTO DEBERIA ENVIAR UN MENSAJE AL USUARIO
                std::cerr << "The client tried to log in with an incorrect password" << std::endl;
                close(client_socket);
                continue;
            }
            recv(client_socket, buffer, BUFFER_SIZE, 0);
            if(CheckNickName(buffer) == false)
            {
                std::cerr << "The client tries to log with wrong nickname" << std::endl;
                close(client_socket);
                continue;
            }
            else
            {
                std::string nickname = Separate(buffer, 1);
                std::cout << "<" << nickname << ">" << " acaba de entrar al servidor!" << std::endl;
                recv(client_socket, buffer, BUFFER_SIZE, 0);
                clientes.push_back(ClientData(client_socket, nickname));
            }

            // std::cerr << "client_socket :\n" << client_socket << std::endl;
            // std::cerr << "buffer :\n" << buffer << std::endl;
            // Agregar el cliente al vector de clientes
        }

        for (size_t i = 0; i < clientes.size(); ++i) 
        {
            if (fds[i + 1].revents & POLLIN) 
            {
            int bytes_received = read(client_socket, buffer, BUFFER_SIZE);
            for (std::vector<ClientData>::iterator it = clientes.begin(); it != clientes.end(); ++it) //que cliente ha sido
            {
                if (it->getSocket() == client_socket) 
                {
                    if (bytes_received <= 0) 
                    {
                        std::cerr << "Cliente desconectado" << std::endl;
                        clientes.erase(it); // Eliminar el elemento del vector
                        close(client_socket);
                        break;
                    }
                    std::cout <<  it->getNickName() << " : " << buffer << std::endl;

                    //std::cout << "Mensaje de " << FindSocket(clientes, client_socket).getName()  << " :" << buffer << std::endl;

                    // procesar los comandos IRC
                    // std::string response = "Recibido: ";
                    // response += buffer;
                    // write(client_socket, response.c_str(), response.length());
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

