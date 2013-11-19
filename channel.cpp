#include <sstream>
#include "channel.h"


std::string Channel::GetXML()
{
	std::stringstream ss;

	ss << "  <channel id=\"" << m_id << "\">" << std::endl;
	ss << "    <display-name lang=\"nl\">" << m_name << "</display-name>" << std::endl;
	ss << "  </channel>" << std::endl;

	return ss.str();
}

void Channel::SetId(int id)
{
	m_id = id;
}

int Channel::GetId()
{
	return m_id;
}

void Channel::SetName(std::string name)
{
	m_name = name;
}

std::string Channel::GetName()
{
	return m_name;
}

void Channel::SetNameShort(std::string nameShort)
{
	m_nameShort = nameShort;
}

std::string Channel::GetNameShort()
{
	return m_nameShort;
}

