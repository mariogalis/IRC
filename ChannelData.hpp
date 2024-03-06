#ifndef CHANNELDATA_HPP
# define CHANNELDATA_HPP

#include "Irc.hpp"

class ClientData;

class ChannelData 
{
public:
    ChannelData();
    ChannelData(std::string channel_name);
    ~ChannelData();
    ChannelData(const ChannelData &other);
    ChannelData	&operator=(const ChannelData &other);

    //SETTERS
    void setChannelName(std::string newNickName);
    //GETTERS
    std::string getChannelName();
private:
    std::string _ChannelName;
    std::vector<ClientData> clients_vec_channel;
};

#endif
