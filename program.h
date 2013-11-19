#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "channel.h"


class Program
{
public:
	Program(Channel channel);

	void LoadFromJSON(std::string json);
	std::string GetXML();

	Channel GetChannel();

	void SetId(int id);
	int GetId();
	void SetTitle(std::string title);
	std::string GetTitle();
	void SetGenre(std::string genre);
	std::string GetGenre();
	void SetType(std::string type);
	std::string GetType();
	void SetRating(std::string rating);
	std::string GetRating();

	void SetArticleId(int articleId);
	int GetArticleId();
	void SetArticleTitle(std::string articleTitle);
	std::string GetArticleTitle();

	void SetDateStart(std::string dateStart);
	std::string GetDateStart();
	void SetDateEnd(std::string dateEnd);
	std::string GetDateEnd();

	void SetSynopsis(std::string synopsis);
	std::string GetSynopsis();
	void SetHosts(std::string hosts);
	std::string GetHosts();
	void SetActors(std::string actors);
	std::string GetActors();
	void SetDirector(std::string director);
	std::string GetDirector();

private:
	std::string ConvertGenre(std::string genre);
	std::string ConvertDate(std::string date);

private:
	Channel m_channel;

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

