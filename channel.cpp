#include <sstream>
#include "functions.h"
#include "channel.h"

Channel::Channel() :
	m_id(0)
{
}

bool Channel::operator==(int value) const
{
	return m_id == value;
}

bool Channel::operator<(const Channel& other) const
{
	return (m_id < other.Id());
}

std::string Channel::GetXML() const
{
	std::stringstream ss;

	ss << "  <channel id=\"" << m_id << "\">" << std::endl;
	ss << "    <display-name lang=\"nl\">" << m_name << "</display-name>" << std::endl;

	if (!m_iconUrl.empty())
		ss << "    <icon src=\"" << m_iconUrl << "\" />" << std::endl;

	ss << "  </channel>" << std::endl;

	return ss.str();
}

void Channel::Id(int id)
{
	m_id = id;
}

int Channel::Id() const
{
	return m_id;
}

void Channel::InternalId(const std::string& internalId)
{
	m_internalId = internalId;
}

std::string Channel::InternalId() const
{
	return m_internalId;
}

void Channel::Name(const std::string& name)
{
	m_name = FixHTMLAndSpecialCharacters(name);
}

std::string Channel::Name() const
{
	return m_name;
}

void Channel::NameShort(const std::string& nameShort)
{
	m_nameShort = FixHTMLAndSpecialCharacters(nameShort);
}

std::string Channel::NameShort() const
{
	return m_nameShort;
}

void Channel::IconUrl(const std::string& iconUrl)
{
	m_iconUrl = iconUrl;
}

std::string Channel::IconUrl() const
{
	return m_iconUrl;
}

