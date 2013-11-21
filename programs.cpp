#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "programs.h"


void Programs::LoadFromJSON(Channel channel, std::string json)
{
	std::stringstream jsonStream;
	jsonStream << json;

	std::stringstream channelIdStream;
	channelIdStream << channel.GetId();

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child(channelIdStream.str().c_str()))
	{
		Program program(channel);

		program.SetId(val.second.get<int>("db_id"));

		if (std::find(m_programs.begin(), m_programs.end(), program.GetId()) != m_programs.end())
			continue;

		program.SetTitle(val.second.get<std::string>("titel"));
		program.SetType(val.second.get<std::string>("soort"));
		program.SetGenre(val.second.get<std::string>("genre"));
		program.SetRating(val.second.get<std::string>("kijkwijzer"));
		program.SetDateStart(val.second.get<std::string>("datum_start"));
		program.SetDateEnd(val.second.get<std::string>("datum_end"));

		boost::optional<int> articleId = val.second.get_optional<int>("artikel_id");
		if (articleId.is_initialized())
			program.SetArticleId(articleId.get());
			
		boost::optional<std::string> articleTitle = val.second.get_optional<std::string>("artikel_titel");
		if (articleTitle.is_initialized())
			program.SetArticleTitle(articleTitle.get());

		m_programs.push_back(program);
	}
}

std::string Programs::GetXML() const
{
	std::stringstream ss;
	for (const_iterator it = begin(); it != end(); ++it)
		ss << it->GetXML();

	return ss.str();
}

bool Programs::Remove(Program program)
{
	Programs::iterator it = std::find(m_programs.begin(), m_programs.end(), program.GetId());
	if (it != m_programs.end())
	{
		m_programs.erase(it);
		return true;
	}
	return false;
}

Programs::iterator Programs::begin()
{
	return m_programs.begin();
}

Programs::const_iterator Programs::begin() const
{
	return m_programs.begin();
}

Programs::iterator Programs::end()
{
	return m_programs.end();
}

Programs::const_iterator Programs::end() const
{
	return m_programs.end();
}

Programs::reverse_iterator Programs::rbegin()
{
	return m_programs.rbegin();
}

Programs::const_reverse_iterator Programs::rbegin() const
{
	return m_programs.rbegin();
}

Programs::reverse_iterator Programs::rend()
{
	return m_programs.rend();
}

Programs::const_reverse_iterator Programs::rend() const
{
	return m_programs.rend();
}

size_t Programs::size() const
{
	return m_programs.size();
}

