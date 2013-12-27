#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <boost/serialization/access.hpp>


class Channel
{
public:
	Channel();

	bool operator==(int value) const;
	bool operator<(const Channel& other) const;

	std::string GetXML() const;

	void Id(int id);
	int Id() const;
	void InternalId(const std::string& internalId);
	std::string InternalId() const;
	void Name(const std::string& name);
	std::string Name() const;
	void NameShort(const std::string& nameShort);
	std::string NameShort() const;
	void IconUrl(const std::string& iconUrl);
	std::string IconUrl() const;

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & m_id;
		ar & m_internalId;
		ar & m_name;
		ar & m_nameShort;
		ar & m_iconUrl;
	}

private:
	int m_id;
	std::string m_internalId;
	std::string m_name;
	std::string m_nameShort;
	std::string m_iconUrl;
};

#endif // CHANNEL_H

