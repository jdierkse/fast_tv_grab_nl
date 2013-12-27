#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <string>
#include <vector>
#include <boost/serialization/vector.hpp>
#include "channel.h"
#include "program.h"


class Programs
{
public:
	std::string GetXML() const;
	bool Remove(const Program& program);

	typedef std::vector<Program>::iterator iterator;
	typedef std::vector<Program>::const_iterator const_iterator;
	typedef std::vector<Program>::reverse_iterator reverse_iterator;
	typedef std::vector<Program>::const_reverse_iterator const_reverse_iterator;

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	reverse_iterator rend();
	const_reverse_iterator rend() const;
	size_t size() const;

	void push_back(const Program& program);

private:
        friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & m_programs;
	}

private:
	std::vector<Program> m_programs;
};

#endif // PROGRAMS_h

