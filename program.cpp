#include <vector>
#include "functions.h"
#include "program.h"


Program::Program() :
	m_detailsLoaded(false),
	m_id(0),
	m_articleId(0)
{
}

Program::Program(Channel channel) :
	m_channel(channel),
	m_detailsLoaded(false),
	m_id(0),
	m_articleId(0)
{
}

bool Program::operator==(const int& value) const
{
	return m_id == value;
}

bool Program::operator==(const std::string& value) const
{
	return m_internalId == value;
}

bool Program::operator<(const Program& other) const
{
	return (m_id < other.GetId());
}

std::string Program::GetXML() const
{
	std::stringstream ss;
	ss << "  <programme start=\"" << m_dateStart << "\" stop=\"" << m_dateEnd << "\" channel=\"" << m_channel.GetId() << "\">" << std::endl;

	if (m_subTitle.empty())
	{
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
	}
	else
	{
		ss << "    <title lang=\"nl\">" << m_title << "</title>" << std::endl;
		ss << "    <sub-title lang=\"nl\">" << m_subTitle << "</sub-title>" << std::endl;
	}

	if (!m_country.empty())
		ss << "    <country>" << m_country << "</country>" << std::endl;

	if (!m_synopsis.empty())
		ss << "    <desc lang=\"nl\">" << m_synopsis << "</desc>" << std::endl;

	if (!m_genre.empty())
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

void Program::SetId(int id)
{
	m_id = id;
}

int Program::GetId() const
{
	return m_id;
}

void Program::SetInternalId(std::string internalId)
{
	m_internalId = internalId;
}

std::string Program::GetInternalId() const
{
	return m_internalId;
}

void Program::SetTitle(std::string title)
{
	m_title = FixHTMLAndSpecialCharacters(title);
}

std::string Program::GetTitle() const
{
	return m_title;
}

void Program::SetSubTitle(std::string subTitle)
{
	m_subTitle = subTitle;
}

std::string Program::GetSubTitle() const
{
	return m_subTitle;
}

void Program::SetGenre(std::string genre)
{
	m_genre = genre;
}

std::string Program::GetGenre() const
{
	return m_genre;
}

void Program::SetType(std::string type)
{
	m_type = FixHTMLAndSpecialCharacters(type);
}

std::string Program::GetType() const
{
	return m_type;
}

void Program::SetRating(std::string rating)
{
	m_rating = FixHTMLAndSpecialCharacters(rating);
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
	m_articleTitle = FixHTMLAndSpecialCharacters(articleTitle);
}

std::string Program::GetArticleTitle() const
{
	return m_articleTitle;
}

void Program::SetDateStart(std::string dateStart)
{
	m_dateStart = dateStart;
}

std::string Program::GetDateStart() const
{
	return m_dateStart;
}

void Program::SetDateEnd(std::string dateEnd)
{
	m_dateEnd = dateEnd;
}

std::string Program::GetDateEnd() const
{
	return m_dateEnd;
}

void Program::SetCountry(std::string country)
{
	m_country = country;
}

std::string Program::GetCountry() const
{
	return m_country;
}

void Program::SetSynopsis(std::string synopsis)
{
	m_synopsis = FixHTMLAndSpecialCharacters(synopsis);
}

std::string Program::GetSynopsis() const
{
	return m_synopsis;
}

void Program::SetHosts(std::string hosts)
{
	m_hosts = FixHTMLAndSpecialCharacters(hosts);
}

std::string Program::GetHosts() const
{
	return m_hosts;
}

void Program::SetActors(std::string actors)
{
	m_actors = FixHTMLAndSpecialCharacters(actors);
}

std::string Program::GetActors() const
{
	return m_actors;
}

void Program::SetDirector(std::string director)
{
	m_director = FixHTMLAndSpecialCharacters(director);
}

std::string Program::GetDirector() const
{
	return m_director;
}

void Program::SetDetailsLoaded(bool detailsLoaded)
{
	m_detailsLoaded = detailsLoaded;
}

bool Program::GetDetailsLoaded() const
{
	return m_detailsLoaded;
}

