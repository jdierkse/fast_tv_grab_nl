#include <sstream>
#include "channels.h"


std::string Channels::GetXML() const
{
	std::stringstream ss;
	for (const_iterator it = begin(); it != end(); ++it)
		ss << it->GetXML();

	return ss.str();
}

Channels::iterator Channels::begin()
{
	return m_channels.begin();
}

Channels::const_iterator Channels::begin() const
{
	return m_channels.begin();
}

Channels::iterator Channels::end()
{
	return m_channels.end();
}

Channels::const_iterator Channels::end() const
{
	return m_channels.end();
}

Channels::reverse_iterator Channels::rbegin()
{
	return m_channels.rbegin();
}

Channels::const_reverse_iterator Channels::rbegin() const
{
	return m_channels.rbegin();
}

Channels::reverse_iterator Channels::rend()
{
	return m_channels.rend();
}

Channels::const_reverse_iterator Channels::rend() const
{
	return m_channels.rend();
}

size_t Channels::size() const
{
	return m_channels.size();
}

void Channels::push_back(Channel channel)
{
	m_channels.push_back(channel);
}

