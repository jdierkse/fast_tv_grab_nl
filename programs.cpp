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

		try
		{
			program.SetId(val.second.get<int>("db_id"));

			if (std::find(m_programs.begin(), m_programs.end(), program.GetId()) != m_programs.end())
				continue;

			program.SetTitle(val.second.get<std::string>("titel"));
			program.SetType(val.second.get<std::string>("soort"));
			program.SetGenre(val.second.get<std::string>("genre"));
			program.SetRating(val.second.get<std::string>("kijkwijzer"));
			program.SetDateStart(val.second.get<std::string>("datum_start"));
			program.SetDateEnd(val.second.get<std::string>("datum_end"));

			try
			{
				program.SetArticleId(val.second.get<int>("artikel_id"));
			}
			catch (const boost::property_tree::ptree_bad_data &e)
			{
			}
			catch (const boost::property_tree::ptree_bad_path &e)
			{
			}
			
			try
			{
				program.SetArticleTitle(val.second.get<std::string>("artikel_titel"));
			}
			catch (const boost::property_tree::ptree_bad_data &e)
			{
			}
			catch (const boost::property_tree::ptree_bad_path &e)
			{
			}

			m_programs.push_back(program);
		}
		catch (...)
		{
		}
	}
}

std::string Programs::GetXML()
{
	std::stringstream ss;
	for (iterator it = begin(); it != end(); ++it)
		ss << it->GetXML();

	return ss.str();
}

Programs::iterator Programs::begin()
{
	return m_programs.begin();
}

Programs::iterator Programs::end()
{
	return m_programs.end();
}

Programs::reverse_iterator Programs::rbegin()
{
	return m_programs.rbegin();
}

Programs::reverse_iterator Programs::rend()
{
	return m_programs.rend();
}

size_t Programs::size()
{
	return m_programs.size();
}

