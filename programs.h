#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <string>
#include <vector>
#include "channel.h"
#include "program.h"


class Programs
{
public:
	void LoadFromJSON(Channel channel, std::string json);
	std::string GetXML();

	typedef std::vector<Program>::iterator iterator;
	typedef std::vector<Program>::reverse_iterator reverse_iterator;

	iterator begin();
	iterator end();
	reverse_iterator rbegin();
	reverse_iterator rend();
	size_t size();

private:
	std::vector<Program> m_programs;
};

#endif // PROGRAMS_h

