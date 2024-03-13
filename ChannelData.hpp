#ifndef CHANNELDATA_HPP
# define CHANNELDATA_HPP

#include "Irc.hpp"


class ClientData;
class Server;

class ChannelData 
{
	public:

		ChannelData();
		ChannelData(std::string ChannelName, std::string topic);
		~ChannelData();
		ChannelData(const ChannelData &other);
		ChannelData	&operator=(const ChannelData &other);

		//SETTERS
		void setChannelName(std::string newChannelName);
		void setMode(char newMode);
		void setInviteOnly(bool inviteOnly);
		void setTopicRestrictions(bool topicRestrictions);
		void setPasswordRestrictions(bool passwordRestrictions);
		void setServerLimit(int serverLimit);

		//GETTERS
		std::string getChannelName();
		char getMode();
		bool isInviteOnly();
		bool hasTopicRestrictions();
		bool hasPasswordRestrictions();
		int getServerLimit();
		std::string getTopic();

		//FUNCTIONS
		
		void	sendToChannel(ClientData *client, ChannelData *chanel, std::string message);
		bool	isChanOp(ClientData *client);
		void    makeUserOP(ClientData *OP, ClientData *client);
		void    changeTopic(ClientData *client, std::string newtopic);
		void    addUser(ClientData *client, std::string pass);
        void    printTopic(ClientData *client);
		
	private:

		bool		_inviteOnly;
		bool		_topicRestrictions;
		bool		_passwordRestrictions;
		int			_serverlimit;
		std::string _ChannelName;
		std::string _pass;
		std::string _topic;
		std::vector<ClientData> _clientsVec;
		std::vector<ClientData> _operatorsVec;


};

#endif

