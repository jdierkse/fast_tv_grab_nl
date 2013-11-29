#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/access.hpp>
#include "channel.h"


class Program
{
public:
	Program(Channel channel);

	bool operator==(const int& value) const;
	bool operator<(const Program& other) const;

	std::string GetXML() const;

	Channel GetChannel() const;

	void SetId(int id);
	int GetId() const;
	void SetTitle(std::string title);
	std::string GetTitle() const;
	void SetGenre(std::string genre);
	std::string GetGenre() const;
	void SetType(std::string type);
	std::string GetType() const;
	void SetRating(std::string rating);
	std::string GetRating() const;

	void SetArticleId(int articleId);
	int GetArticleId() const;
	void SetArticleTitle(std::string articleTitle);
	std::string GetArticleTitle() const;

	void SetDateStart(std::string dateStart);
	std::string GetDateStart() const;
	void SetDateEnd(std::string dateEnd);
	std::string GetDateEnd() const;

	void SetSynopsis(std::string synopsis);
	std::string GetSynopsis() const;
	void SetHosts(std::string hosts);
	std::string GetHosts() const;
	void SetActors(std::string actors);
	std::string GetActors() const;
	void SetDirector(std::string director);
	std::string GetDirector() const;

	void SetDetailsLoaded(bool detailsLoaded);
	bool GetDetailsLoaded() const;

private:
	Program();

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_channel;
		ar & m_detailsLoaded;
		ar & m_id;
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
	std::string ConvertGenre(std::string genre);
	std::string ConvertDate(std::string date);

private:
	Channel m_channel;
	bool m_detailsLoaded;

	int m_id;
	std::string m_title;
	std::string m_genre;
	std::string m_type;
	std::string m_rating;

	int m_articleId;
	std::string m_articleTitle;

	std::string m_dateStart;
	std::string m_dateEnd;

	std::string m_synopsis;
	std::string m_hosts;
	std::string m_actors;
	std::string m_director;
};

#endif // PROGRAM_H

