#include "StdAfx.h"
#include ".\magnification.h"

Magnification::Magnification(void)
: m_level(3)
{
	m_levelSteps.push_back(0.3);
	m_levelSteps.push_back(0.5);
	m_levelSteps.push_back(0.666);
	m_levelSteps.push_back(1);
	m_levelSteps.push_back(1.5);
	m_levelSteps.push_back(2);
	m_levelSteps.push_back(3);
	m_levelSteps.push_back(4);
	m_levelSteps.push_back(5.5);
	m_levelSteps.push_back(8);
	m_levelSteps.push_back(11);
	m_levelSteps.push_back(16);
	m_levelSteps.push_back(23);
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

void Magnification::Decrease(void)
{
	if ( m_level > 0 )
	{
		m_level--;
	}
}


double Magnification::GetValue(void)
{ 
	ASSERT ( m_level >= 0 );
	ASSERT ( m_level < static_cast<int>( m_levelSteps.size() ) );
	return m_levelSteps[m_level];
}
