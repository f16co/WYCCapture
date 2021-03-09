#include "stdafx.h"
#include "ScopeLock.h"

CScopeLock::CScopeLock(CRITICAL_SECTION* critical_section)
{
	m_critical_section = critical_section;
	if (m_critical_section)
		EnterCriticalSection(m_critical_section);
}

CScopeLock::~CScopeLock()
{
	if (m_critical_section)
	{
		LeaveCriticalSection(m_critical_section);
		m_critical_section = NULL;
	}
}

void CScopeLock::InitCS(CRITICAL_SECTION* critical_section)
{
	if (critical_section)
		InitializeCriticalSectionEx(critical_section, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
}

void CScopeLock::DeleteCS(CRITICAL_SECTION* critical_section)
{
	if (critical_section)
		DeleteCriticalSection(critical_section);
}
