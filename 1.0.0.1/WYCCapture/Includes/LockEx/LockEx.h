#pragma once
#include <Windows.h>

class CLockEx
{
public:
	CLockEx();
	~CLockEx();

	void Lock();
	void Unlock();

private:
	CRITICAL_SECTION m_critical_section;
};

