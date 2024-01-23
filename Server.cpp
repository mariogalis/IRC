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
    printf("server start\n");

    // Devolver el descriptor de archivo del socket del servidor
    return server_socket;
}

int Server::WaitClient(int server_socket)
{
    // Declarar un arreglo de estructuras pollfd para monitorear eventos en los sockets del servidor y clientes
    struct pollfd pollfds[MAX_CLIENTS + 1];
    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN | POLLPRI;  // Monitorear eventos de lectura en el socket del servidor
    int useClient = 0;

    while (1)
    {
        // Esperar eventos en los sockets usando poll
        int pollResult = poll(pollfds, useClient + 1, 5000); //monitorear eventos en múltiples descriptores de archivos de manera no bloqueante
        //pollfds: contiene la configuración de los descriptores de archivos que se deben monitorear
        //useClient + 1: Representa el número total de elementos en el arreglo pollfds. +1, se garantiza que la función poll revisará todos los elementos del arreglo, desde pollfds[0] hasta pollfds[useClient]
        //5000: Es el tiempo máximo de espera en milisegundos para que ocurra un evento en algún descriptor de archivo 5 seg
        //pollResult: almacena el resultado de la llamada a poll. Después de la llamada, pollResult contendrá la cantidad de descriptores de archivos que tienen eventos pendientes, o un valor específico indicando el resultado de la operación
        //Si pollResult es igual a 0, no se detectaron eventos antes de que expirara el tiempo de espera
        //Si pollResult es mayor que 0, indica cuántos descriptores de archivos tienen eventos pendientes
        //Si pollResult es igual a -1, indica que se produjo un error durante la llamada a poll
        if (pollResult > 0) // Si hay eventos disponibles
        {
            // Verificar si hay una conexión entrante en el socket del servidor
            if (pollfds[0].revents & POLLIN)
            {
                // Aceptar la conexión entrante
                struct sockaddr_in cliaddr;
                //int addrlen = sizeof(cliaddr);
                int client_socket = accept(server_socket, (struct sockaddr *)&cliaddr, NULL); //NULL debeia ser &addrlen
                printf("accept success %s\n", inet_ntoa(cliaddr.sin_addr));

                // Buscar un espacio libre en el arreglo de estructuras pollfd para almacenar el nuevo descriptor de archivo del cliente
                for (int i = 1; i < MAX_CLIENTS; i++)
                {
                    if (pollfds[i].fd == 0)
                    {
                        // Asignar el descriptor de archivo del cliente al arreglo de estructuras pollfd
                        pollfds[i].fd = client_socket;
                        pollfds[i].events = POLLIN | POLLPRI;
                        useClient++;
                        break;
                    }
                }
            }

            // Verificar eventos de lectura en los sockets de los clientes existentes
            for (int i = 1; i < MAX_CLIENTS; i++)
            {
                if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN)
                {
                    char buf[SIZE];
                    int bufSize = read(pollfds[i].fd, buf, SIZE - 1);
                    if (bufSize == -1)
                    {
                        // Error de lectura, cerrar el socket del cliente y actualizar el estado
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        useClient--;
                    }
                    else if (bufSize == 0)
                    {
                        // Conexión cerrada por el cliente, cerrar el socket del cliente y actualizar el estado
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        useClient--;
                    }
                    else
                    {
                        // Datos recibidos del cliente, imprimir
                        buf[bufSize] = '\0';
                        printf("From client: %s\n", buf);
                    }
                }
            }
        }
    }
}

int Server::Start()
{
    int server_socket = create_socket();

    int client_socket = WaitClient(server_socket);

    printf("server end\n");

    close(client_socket);
    close(server_socket);

    return 0;
}

