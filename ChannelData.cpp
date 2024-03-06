#include "ChannelData.hpp"

ChannelData::ChannelData(){}
ChannelData::ChannelData(std::string ChannelName) : _ChannelName(ChannelName){}
ChannelData::ChannelData(const ChannelData &other){*this = other;}
ChannelData::~ChannelData(){}

ChannelData &ChannelData::operator=(const ChannelData &other)
{
    if (this != &other)
    {
        _ChannelName= other._ChannelName;
    }
    return(*this);
}

void ChannelData::setChannelName(std::string newChannelName){this->_ChannelName = newChannelName;}

std::string ChannelData::getChannelName()
{
	return _ChannelName;
}

