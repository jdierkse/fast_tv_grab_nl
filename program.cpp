#include <vector>
#include "functions.h"
#include "program.h"


Program::Program() :
	m_detailsLoaded(false),
	m_id(0),
	m_articleId(0)
{
}

Program::Program(const ::Channel& channel) :
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
	return (m_id < other.Id());
}

std::string Program::GetXML() const
{
	std::stringstream ss;
	ss << "  <programme start=\"" << m_dateStart << "\" stop=\"" << m_dateEnd << "\" channel=\"" << m_channel.Id() << "\">" << std::endl;

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

::Channel Program::Channel() const
{
	return m_channel;
}

void Program::Id(int id)
{
	m_id = id;
}

int Program::Id() const
{
	return m_id;
}

void Program::InternalId(const std::string& internalId)
{
	m_internalId = internalId;
}

std::string Program::InternalId() const
{
	return m_internalId;
}

void Program::Title(const std::string& title)
{
	m_title = FixHTMLAndSpecialCharacters(title);
}

std::string Program::Title() const
{
	return m_title;
}

void Program::SubTitle(const std::string& subTitle)
{
	m_subTitle = subTitle;
}

std::string Program::SubTitle() const
{
	return m_subTitle;
}

void Program::Genre(const std::string& genre)
{
	m_genre = genre;
}

std::string Program::Genre() const
{
	return m_genre;
}

void Program::Type(const std::string& type)
{
	m_type = FixHTMLAndSpecialCharacters(type);
}

std::string Program::Type() const
{
	return m_type;
}

void Program::Rating(const std::string& rating)
{
	m_rating = FixHTMLAndSpecialCharacters(rating);
}

std::string Program::Rating() const
{
	return m_rating;
}

void Program::ArticleId(int articleId)
{
	m_articleId = articleId;
}

int Program::ArticleId() const
{
	return m_articleId;
}

void Program::ArticleTitle(const std::string& articleTitle)
{
	m_articleTitle = FixHTMLAndSpecialCharacters(articleTitle);
}

std::string Program::ArticleTitle() const
{
	return m_articleTitle;
}

void Program::DateStart(const std::string& dateStart)
{
	m_dateStart = dateStart;
}

std::string Program::DateStart() const
{
	return m_dateStart;
}

void Program::DateEnd(const std::string& dateEnd)
{
	m_dateEnd = dateEnd;
}

std::string Program::DateEnd() const
{
	return m_dateEnd;
}

void Program::Country(const std::string& country)
{
	m_country = country;
}

std::string Program::Country() const
{
	return m_country;
}

void Program::Synopsis(const std::string& synopsis)
{
	m_synopsis = FixHTMLAndSpecialCharacters(synopsis);
}

std::string Program::Synopsis() const
{
	return m_synopsis;
}

void Program::Hosts(const std::string& hosts)
{
	m_hosts = FixHTMLAndSpecialCharacters(hosts);
}

std::string Program::Hosts() const
{
	return m_hosts;
}

void Program::Actors(const std::string& actors)
{
	m_actors = FixHTMLAndSpecialCharacters(actors);
}

std::string Program::Actors() const
{
	return m_actors;
}

void Program::Director(const std::string& director)
{
	m_director = FixHTMLAndSpecialCharacters(director);
}

std::string Program::Director() const
{
	return m_director;
}

void Program::DetailsLoaded(bool detailsLoaded)
{
	m_detailsLoaded = detailsLoaded;
}

bool Program::DetailsLoaded() const
{
	return m_detailsLoaded;
}

