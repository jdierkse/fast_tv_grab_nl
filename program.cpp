#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include "functions.h"
#include "program.h"


Program::Program() :
	m_detailsLoaded(false)
{
}

Program::Program(Channel channel) :
	m_channel(channel),
	m_detailsLoaded(false)
{
}

bool Program::operator==(const int& value) const
{
	return m_id == value;
}

bool Program::operator<(const Program& other) const
{
	return (m_id < other.GetId());
}

void Program::LoadDetailsFromJSON(std::string json)
{
	std::stringstream jsonStream;
	jsonStream << "{ \"root\": [" << json << "] }";

	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json(jsonStream, pt);

	BOOST_FOREACH(boost::property_tree::ptree::value_type &val, pt.get_child("root"))
	{
		try
		{
			SetSynopsis(val.second.get<std::string>("synop"));
		}
		catch (const boost::property_tree::ptree_bad_data &e)
		{
		}
		catch (const boost::property_tree::ptree_bad_path &e)
		{
		}

		try
		{
			SetHosts(val.second.get<std::string>("presentatie"));
		}
		catch (const boost::property_tree::ptree_bad_data &e)
		{
		}
		catch (const boost::property_tree::ptree_bad_path &e)
		{
		}

		try
		{
			SetActors(val.second.get<std::string>("acteursnamen_rolverdeling"));
		}
		catch (const boost::property_tree::ptree_bad_data &e)
		{
		}
		catch (const boost::property_tree::ptree_bad_path &e)
		{
		}

		try
		{
			SetDirector(val.second.get<std::string>("regisseur"));
		}
		catch (const boost::property_tree::ptree_bad_data &e)
		{
		}
		catch (const boost::property_tree::ptree_bad_path &e)
		{
		}
	}

	m_detailsLoaded = true;
}

std::string Program::GetXML() const
{
	std::stringstream ss;
	ss << "  <programme start=\"" << m_dateStart << "\" stop=\"" << m_dateEnd << "\" channel=\"" << m_channel.GetId() << "\">" << std::endl;

	bool subTitle = false;
	std::vector<std::string> titles = SplitString(m_title, ": ");
	for (std::vector<std::string>::iterator it = titles.begin(); it != titles.end(); ++it)
	{
		if (!subTitle)
		{
			ss << "    <title lang=\"nl\">" << *it << "</title>" << std::endl;
			subTitle = true;
		}
		else
		{
			ss << "    <sub-title lang=\"nl\">" << *it << "</sub-title>" << std::endl;
		}
	}

	if (!m_synopsis.empty())
		ss << "    <desc lang=\"nl\">" << m_synopsis << "</desc>" << std::endl;

	ss << "    <category lang=\"en\">" << m_genre << "</category>" << std::endl;

	if (!m_director.empty() || !m_actors.empty() || !m_hosts.empty())
	{
		ss << "    <credits>" << std::endl;
		if (!m_director.empty())
			ss << "      <director>" << m_director << "</director>" << std::endl;
		if (!m_actors.empty())
		{
			std::vector<std::string> actors = SplitString(m_actors, ", ");
			for (std::vector<std::string>::iterator it = actors.begin(); it != actors.end(); ++it)
				ss << "      <actor>" << *it << "</actor>" << std::endl;
		}
		if (!m_hosts.empty())
		{
			std::vector<std::string> presenters = SplitString(m_hosts, ", ");
			for (std::vector<std::string>::iterator it = presenters.begin(); it != presenters.end(); ++it)
				ss << "      <presenter>" << *it << "</presenter>" << std::endl;
		}

		ss << "    </credits>" << std::endl;
	}

	//ss << "    <video>" << std::endl;
	//ss << "      <aspect>16:9</aspect>" << std::endl;
	//ss << "    </video" << std::endl;
	//ss << "    <subtitles type=\"teletext\" />" << std::endl;
	
	ss << "  </programme>" << std::endl;

	return ss.str();
}

Channel Program::GetChannel() const
{
	return m_channel;
}

bool Program::GetDetailsLoaded() const
{
	return m_detailsLoaded;
}

void Program::SetId(int id)
{
	m_id = id;
}

int Program::GetId() const
{
	return m_id;
}

void Program::SetTitle(std::string title)
{
	m_title = RemoveSpecialCharacters(title);
}

std::string Program::GetTitle() const
{
	return m_title;
}

void Program::SetGenre(std::string genre)
{
	m_genre = ConvertGenre(genre);
}

std::string Program::GetGenre() const
{
	return m_genre;
}

void Program::SetType(std::string type)
{
	m_type = RemoveSpecialCharacters(type);
}

std::string Program::GetType() const
{
	return m_type;
}

void Program::SetRating(std::string rating)
{
	m_rating = RemoveSpecialCharacters(rating);
}

std::string Program::GetRating() const
{
	return m_rating;
}

void Program::SetArticleId(int articleId)
{
	m_articleId = articleId;
}

int Program::GetArticleId() const
{
	return m_articleId;
}

void Program::SetArticleTitle(std::string articleTitle)
{
	m_articleTitle = RemoveSpecialCharacters(articleTitle);
}

std::string Program::GetArticleTitle() const
{
	return m_articleTitle;
}

void Program::SetDateStart(std::string dateStart)
{
	m_dateStart = ConvertDate(dateStart);
}

std::string Program::GetDateStart() const
{
	return m_dateStart;
}

void Program::SetDateEnd(std::string dateEnd)
{
	m_dateEnd = ConvertDate(dateEnd);
}

std::string Program::GetDateEnd() const
{
	return m_dateEnd;
}

void Program::SetSynopsis(std::string synopsis)
{
	m_synopsis = RemoveSpecialCharacters(synopsis);
}

std::string Program::GetSynopsis() const
{
	return m_synopsis;
}

void Program::SetHosts(std::string hosts)
{
	m_hosts = RemoveSpecialCharacters(hosts);
}

std::string Program::GetHosts() const
{
	return m_hosts;
}

void Program::SetActors(std::string actors)
{
	m_actors = RemoveSpecialCharacters(actors);
}

std::string Program::GetActors() const
{
	return m_actors;
}

void Program::SetDirector(std::string director)
{
	m_director = RemoveSpecialCharacters(director);
}

std::string Program::GetDirector() const
{
	return m_director;
}

std::string Program::ConvertGenre(std::string genre)
{
	if (genre == "Nieuws/actualiteiten" || genre == "Actualiteit")
		return "News / Current affairs";
	if (genre == "Amusement" || genre == "Comedy")
		return "Show / Game show";
	if (genre == "Animatie" || genre == "Jeugd")
		return "Children's / Youth programmes";
	if (genre == "Serie/Soap" || genre == "Film" ||
	    genre == "Komedie" || genre == "Actiefilm" ||
	    genre == "Tekenfilm" || genre == "Animatiekomedie" ||
	    genre == "Thriller" || genre == "Familiekomedie" ||
	    genre == "Romantische Komedie" || genre == "Serie" ||
	    genre == "Misdaad")
		return "Movie / Drama";
	if (genre == "Info" || genre == "Informatief" ||
	    genre == "Documentaire" || genre == "Wetenschap" ||
	    genre == "Natuur" || genre == "Educatief")
		return "Education / Science / Factual topics";
	if (genre == "Sport")
		return "Sports";
	if (genre == "Erotiek")
		return "Leisure hobbies";
	if (genre == "Muziek")
		return "Music / Ballet / Dance";
	if (genre == "Religieus")
		return "Social / Political issues / Economics";
	if (genre == "Kunst/Cultuur" || genre == "Cultuur")
		return "Art / Culture";
	if (genre == "Overig")
		return "Other";
	return genre;
}

std::string Program::ConvertDate(std::string date)
{
	std::stringstream ss;
	ss << boost::regex_replace(date, boost::regex("[-: ]"), "") << " +0100";

	return ss.str();
}

std::string Program::RemoveSpecialCharacters(std::string string)
{
	string = boost::regex_replace(string, boost::regex("<[a-zA-Z1-9\\/ ]*>"), "");
	string = boost::regex_replace(string, boost::regex("\r"), "");
	string = boost::regex_replace(string, boost::regex("&"), "&amp;");
	return string;
}

