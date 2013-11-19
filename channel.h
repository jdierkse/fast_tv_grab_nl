#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>


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
	int m_id;
	std::string m_name;
	std::string m_nameShort;
};

#endif // CHANNEL_H

