#include "stdafx.h"
#include "LockEx.h"

CLockEx::CLockEx()
{
	InitializeCriticalSectionEx(&m_critical_section, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
}

CLockEx::~CLockEx()
{
	DeleteCriticalSection(&m_critical_section);
}

void CLockEx::Lock()
{
	EnterCriticalSection(&m_critical_section);
}

void CLockEx::Unlock()
{
	LeaveCriticalSection(&m_critical_section);
}
