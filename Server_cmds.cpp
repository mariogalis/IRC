#include "Server.hpp"


int Server::firstCommand(const std::string& command, ClientData *client) 
{
    std::istringstream iss(command);
    std::string token;
    std::vector<std::string> tokens;
    std::vector<ClientData*>::iterator it;

    while (std::getline(iss, token, ' '))
        tokens.push_back(token);

    if (!tokens.empty()) 
    {
        std::string ircCommand = tokens[0];
        if(ircCommand == "PASS")
        {
            std::string tryPass = tokens[1];
            std::string myPass = _pass;
            tryPass.pop_back();
            if(myPass.compare(tryPass) != 0)
            {
                std::cerr << "The client tried to log in with an incorrect password" << std::endl;
                return 1;
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
            newNickName.pop_back();
            client->setNickName(newNickName);
            return 0;
        }
        else if(ircCommand == "USER")
        {
            std::string newLogin = tokens[1];
            std::string newReal = tokens[4];
            newReal.pop_back();
            client->setLoginName(newLogin);
            client->setRealName(newReal);
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

int Server::processCommand(const std::string& command, ClientData &client, size_t socket_num, std::vector<ClientData>::iterator it_client) 
{
    std::istringstream iss(command);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(iss, token, ' '))
        tokens.push_back(token);

    if (!tokens.empty()) 
    {
        std::string ircCommand = tokens[0];

        if (ircCommand == "JOIN") 
        {
            std::cout << "Usuario desea unirse al canal: " << tokens[1] << std::endl;
        } 
        else if (ircCommand == "PRIVMSG") 
        {
            // if(tokens[1][0] == '#')
            //     inChannelCmds(command, *it_client, socket_num, it_client);
            // else
            // {
                send_PersonalMessage(tokens[1], tokens[2], &client);
                std::cout << "Mensaje para " << tokens[1] << ": " << tokens[2] << std::endl;
            //}
        }
        else if(ircCommand == "SU")
        {
            std::string su_pass = tokens[1];
            std::string my_pass = _supass;
            su_pass.pop_back();
            su_pass.pop_back();
            if(my_pass.compare(su_pass) != 0)
                sendToUser(&client, makeUserMsg(&client, ERR_NOTREGISTERED, "Contraseña del superusuario erronea"));
            else
            {
                it_client->setSuper(true);
                sendToUser(&client, makeUserMsg(&client, RPL_TRACENEWTYPE, "SUPERUSUARIO"));
                std::cout << BLUE << client.getNickName() << " ahora es superusuario" << NOCOLOR << std::endl;
            }
        }
        else if(ircCommand == "QUIT")
        {
            deleteClient(socket_num, it_client);
        }
        else if(ircCommand == "ENDSERVER")
        {
            CloseServer();
            return (1);
        }
        else 
        {
            sendToUser(&client, makeUserMsg(&client, RPL_NONE, "Comando no reconocido"));
            std::cout << "Comando no reconocido: " << ircCommand << std::endl;
        }
    }
    return (0);
}
