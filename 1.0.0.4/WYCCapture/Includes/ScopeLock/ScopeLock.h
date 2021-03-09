#pragma once
#include <Windows.h>

class CScopeLock
{
public:
	CScopeLock(CRITICAL_SECTION* m_critical_section);
	~CScopeLock();

	static void InitCS(CRITICAL_SECTION* m_critical_section);
	static void DeleteCS(CRITICAL_SECTION* m_critical_section);

private:
	CRITICAL_SECTION* m_critical_section;
};

