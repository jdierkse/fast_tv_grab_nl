#include "programs.h"


std::string Programs::GetXML() const
{
	std::stringstream ss;
	for (const_iterator it = begin(); it != end(); ++it)
		ss << it->GetXML();

	return ss.str();
}

bool Programs::Remove(const Program& program)
{
	Programs::iterator it = std::find(m_programs.begin(), m_programs.end(), program.Id());
	if (it != m_programs.end())
	{
		m_programs.erase(it);
		return true;
	}
	return false;
}

Programs::iterator Programs::begin()
{
	return m_programs.begin();
}

Programs::const_iterator Programs::begin() const
{
	return m_programs.begin();
}

Programs::iterator Programs::end()
{
	return m_programs.end();
}

Programs::const_iterator Programs::end() const
{
	return m_programs.end();
}

Programs::reverse_iterator Programs::rbegin()
{
	return m_programs.rbegin();
}

Programs::const_reverse_iterator Programs::rbegin() const
{
	return m_programs.rbegin();
}

Programs::reverse_iterator Programs::rend()
{
	return m_programs.rend();
}

Programs::const_reverse_iterator Programs::rend() const
{
	return m_programs.rend();
}

size_t Programs::size() const
{
	return m_programs.size();
}

void Programs::push_back(const Program& program)
{
	m_programs.push_back(program);
}
