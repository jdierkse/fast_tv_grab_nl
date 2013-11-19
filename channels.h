#ifndef CHANNELS_H
#define CHANNELS_H

#include <string>
#include <vector>
#include "channel.h"


class Channels
{
public:
	void LoadFromJSON(std::string json);
	std::string GetXML();

	typedef std::vector<Channel>::iterator iterator;
	typedef std::vector<Channel>::reverse_iterator reverse_iterator;

	iterator begin();
	iterator end();
	reverse_iterator rbegin();
	reverse_iterator rend();
	size_t size();

	void push_back(Channel channel);
	void clear();

private:
	std::vector<Channel> m_channels;
};

#endif // CHANNELS_h

