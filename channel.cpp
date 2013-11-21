#include <sstream>
#include "functions.h"
#include "channel.h"

bool Channel::operator==(int value) const
{
	return m_id == value;
}

bool Channel::operator<(const Channel& other) const
{
	return (m_id < other.GetId());
}

std::string Channel::GetXML() const
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

int Channel::GetId() const
{
	return m_id;
}

void Channel::SetName(std::string name)
{
	m_name = FixHTMLAndSpecialCharacters(name);
}

std::string Channel::GetName() const
{
	return m_name;
}

void Channel::SetNameShort(std::string nameShort)
{
	m_nameShort = FixHTMLAndSpecialCharacters(nameShort);
}

std::string Channel::GetNameShort() const
{
	return m_nameShort;
}

