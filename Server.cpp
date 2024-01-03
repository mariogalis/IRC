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

int Server::start()
{
    std::vector<std::string> UsersVector;
    std::string name;
    //char *port_char;
    std::string port_str;
    char buffername[1024];
    // Crear un socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error al crear el socket." << std::endl;
        return -1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    // Convertir la cadena a un valor uint16_t
    std::istringstream ss(_port);
    uint16_t valorHost;

    if (!(ss >> valorHost)) {
        std::cerr << "Error: No se pudo convertir la cadena a uint16_t." << std::endl;
        exit(0);
    }
    //const char* port_char = _port.c_str();
    //port_str = std::atoi(port_char);
    server_address.sin_port = htons(valorHost);  // Puerto estándar IRC
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Vincular el socket a la dirección del servidor
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error al vincular el socket." << std::endl;
        close(server_socket);
        return -1;
    }

    // Escuchar en el socket
    if (listen(server_socket, 10) == -1) {
        std::cerr << "Error al intentar escuchar en el socket." << std::endl;
        close(server_socket);
        return -1;
    }

    std::cout << "Servidor IRC escuchando en el puerto " << GREEN << _port << "..." << NOCOLOR << std::endl;

    // Aceptar conexiones entrantes
    while (true) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("Error al aceptar la conexión del cliente");
            continue;
        }

        std::cout << "Cliente conectado." << std::endl;
        recv(client_socket, buffername, sizeof(buffername), 0);
        name = buffername;
        if (repeatName(UsersVector, buffername)) 
        {
        std::cout << "Nombre repetido. No se puede agregar." << std::endl;
        continue;
        } 
        else 
        {
        // Agregar el nombre al vector si no está repetido
        UsersVector.push_back(buffername);
        std::cout << "Nombre agregado correctamente." << std::endl;
        }
        // Ciclo para recibir y procesar mensajes del cliente
        while (true) {
            char buffer[1024];
            ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytes_received <= 0) {
                if (bytes_received == 0) 
                {
                    std::cout << "Cliente desconectado." << std::endl;
                } else {
                    perror("Error al recibir datos del cliente");
                }
                UsersVector.erase(std::remove(UsersVector.begin(), UsersVector.end(), buffername), UsersVector.end()); //elimino ek usuario del vector antes de salir
                std::cout << RED << buffername << " desconectado" << NOCOLOR << std::endl;
                break;
            }

            buffer[bytes_received] = '\0';
            std::cout << name << ": " << buffer << std::endl;

            // Envio un mensaje parea decirle que todo salio chido
            const char* response = "Mensaje recibido correctamente.";
            send(client_socket, response, strlen(response), 0);
        }

        // Cerrar el socket del cliente después de todos los mensajes
        close(client_socket);
    }

    // Cerrar el socket del servidor
    close(server_socket);

    return 0;

}

