#ifndef CHANNELS_H
#define CHANNELS_H

#include <string>
#include <vector>
#include "channel.h"


class Channels
{
public:
	void LoadFromJSON(std::string json);
	std::string GetXML() const;

	typedef std::vector<Channel>::iterator iterator;
	typedef std::vector<Channel>::const_iterator const_iterator;
	typedef std::vector<Channel>::reverse_iterator reverse_iterator;
	typedef std::vector<Channel>::const_reverse_iterator const_reverse_iterator;

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;
	size_t size() const;

	void push_back(Channel channel);
	void clear();

private:
	std::vector<Channel> m_channels;
};

#endif // CHANNELS_h

