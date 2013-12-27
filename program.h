#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include "channel.h"


class Program
{
public:
	Program(const ::Channel& channel);

	bool operator==(const int& value) const;
	bool operator==(const std::string& value) const;
	bool operator<(const Program& other) const;

	std::string GetXML() const;

	::Channel Channel() const;

	void Id(int id);
	int Id() const;
	void InternalId(const std::string& internalId);
	std::string InternalId() const;
	void Title(const std::string& title);
	std::string Title() const;
	void SubTitle(const std::string& subTitle);
	std::string SubTitle() const;
	void Genre(const std::string& genre);
	std::string Genre() const;
	void Type(const std::string& type);
	std::string Type() const;
	void Rating(const std::string& rating);
	std::string Rating() const;

	void ArticleId(int articleId);
	int ArticleId() const;
	void ArticleTitle(const std::string& articleTitle);
	std::string ArticleTitle() const;

	void DateStart(const std::string& dateStart);
	std::string DateStart() const;
	void DateEnd(const std::string& dateEnd);
	std::string DateEnd() const;

	void Country(const std::string& country);
	std::string Country() const;
	void Synopsis(const std::string& synopsis);
	std::string Synopsis() const;
	void Hosts(const std::string& hosts);
	std::string Hosts() const;
	void Actors(const std::string& actors);
	std::string Actors() const;
	void Director(const std::string& director);
	std::string Director() const;

	void DetailsLoaded(bool detailsLoaded);
	bool DetailsLoaded() const;

private:
	Program();

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & m_channel;
		ar & m_detailsLoaded;
		ar & m_id;
		ar & m_internalId;
		ar & m_title;
		ar & m_genre;
		ar & m_type;
		ar & m_rating;
		ar & m_articleId;
		ar & m_articleTitle;
		ar & m_dateStart;
		ar & m_dateEnd;
		ar & m_synopsis;
		ar & m_hosts;
		ar & m_actors;
		ar & m_director;
	}

private:
	::Channel m_channel;
	bool m_detailsLoaded;

	int m_id;
	std::string m_internalId;
	std::string m_title;
	std::string m_subTitle;
	std::string m_genre;
	std::string m_type;
	std::string m_rating;

	int m_articleId;
	std::string m_articleTitle;

	std::string m_dateStart;
	std::string m_dateEnd;

	std::string m_country;
	std::string m_synopsis;
	std::string m_hosts;
	std::string m_actors;
	std::string m_director;
};

#endif // PROGRAM_H

