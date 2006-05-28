#include "StdAfx.h"
#include ".\magnification.h"

Magnification::Magnification(void)
: m_defaultLevel(3)
, m_level(m_defaultLevel)
, m_levelSteps()
{
	m_levelSteps.push_back(0.3);
	m_levelSteps.push_back(0.5);
	m_levelSteps.push_back(0.666);
	m_levelSteps.push_back(1.0);
	m_levelSteps.push_back(1.5);
	m_levelSteps.push_back(2.0);
	m_levelSteps.push_back(3.0);
	m_levelSteps.push_back(4.0);
	m_levelSteps.push_back(5.5);
	m_levelSteps.push_back(8.0);
	m_levelSteps.push_back(11.0);
	m_levelSteps.push_back(16.0);
	m_levelSteps.push_back(23.0);
}

Magnification::~Magnification(void)
{
}

void Magnification::Increase(void)
{
	if ( m_level < (static_cast<double>(m_levelSteps.size()) - 1) ) 
	{
		m_level++;
	}
}

bool Magnification::Decrease(void)
{
	if ( m_level == 0 ) return false;

	m_level--;
	return true;
}


double Magnification::GetValue(void)
{ 
	ASSERT ( m_level < m_levelSteps.size() );
	return m_levelSteps[m_level];
}

void Magnification::Reset(void)
{
	m_level = m_defaultLevel;
}
