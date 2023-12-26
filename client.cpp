#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char **argv) {
    
    std::string name = "Alberto";
    int port = atoi(argv[1]);
    // Crear un socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error al crear el socket." << std::endl;
        return -1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);  // Puerto IRC
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // Dirección IP del servidor IRC (local)

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error al conectar al servidor." << std::endl;
        close(client_socket);
        return -1;
    }

    std::cout << "Conectado al servidor IRC." << std::endl;
    send(client_socket, name.c_str(), name.size(), 0);
    while (true) {
        // Leer una línea de la entrada estándar
        std::string message;
        std::cout << "Ingrese un mensaje (o escriba 'exit' para salir): ";
        std::getline(std::cin, message);

        // Salir si el usuario escribe "exit"
        if (message == "exit") {
            break;
        }

        // Enviar el mensaje al servidor
        ssize_t bytes_sent = send(client_socket, message.c_str(), message.size(), 0);
        if (bytes_sent == -1) {
            perror("Error al enviar mensaje al servidor");
            break;
        }

        std::cout << "Mensaje enviado al servidor." << std::endl;
    }
    // Cerrar el socket del cliente
    close(client_socket);

    return 0;
}
