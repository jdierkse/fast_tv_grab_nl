#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <boost/serialization/access.hpp>


class Channel
{
public:
	std::string GetXML();

	void SetId(int id);
	int GetId();
	void SetName(std::string name);
	std::string GetName();
	void SetNameShort(std::string nameShort);
	std::string GetNameShort();

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_id;
		ar & m_name;
		ar & m_nameShort;
	}

private:
	int m_id;
	std::string m_name;
	std::string m_nameShort;
};

#endif // CHANNEL_H

