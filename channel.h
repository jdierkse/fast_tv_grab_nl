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

	void SetId(int id);
	int GetId() const;
	void SetInternalId(std::string internalId);
	std::string GetInternalId() const;
	void SetName(std::string name);
	std::string GetName() const;
	void SetNameShort(std::string nameShort);
	std::string GetNameShort() const;

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_id;
		ar & m_internalId;
		ar & m_name;
		ar & m_nameShort;
	}

private:
	int m_id;
	std::string m_internalId;
	std::string m_name;
	std::string m_nameShort;
};

#endif // CHANNEL_H

