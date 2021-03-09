#include "stdafx.h"
#include "PerformanceChecker.h"

CPerformanceChecker::CPerformanceChecker(CString name, PERFORMANCE_CHECK_SCOPE performance_check_scope /*= PERFORMACE_CHECK_MS*/)
{
	m_name = name;
	m_performance_check_scope = performance_check_scope;

	Start();
}

CPerformanceChecker::~CPerformanceChecker()
{
	if (!m_check)
		Check();
}

void CPerformanceChecker::Start()
{
	m_check = false;
	QueryPerformanceFrequency(&m_freq);
	QueryPerformanceCounter(&m_begin);
}

void CPerformanceChecker::Check()
{
	QueryPerformanceCounter(&m_end);

	__int64 time_elapsed = 0;
	CString scope_name = _T("");

	switch (m_performance_check_scope)
	{
	case CPerformanceChecker::PERFORMACE_CHECK_SEC:
		time_elapsed = (m_end.QuadPart - m_begin.QuadPart) / m_freq.QuadPart;
		scope_name = _T("s");
		break;
	case CPerformanceChecker::PERFORMACE_CHECK_MS:
		time_elapsed = (m_end.QuadPart - m_begin.QuadPart) * 1000 / m_freq.QuadPart;
		scope_name = _T("ms");
		break;
	case CPerformanceChecker::PERFORMACE_CHECK_US:
		time_elapsed = (m_end.QuadPart - m_begin.QuadPart) * 1000000 / m_freq.QuadPart;
		scope_name = _T("us");
		break;
	case CPerformanceChecker::PERFORMACE_CHECK_NS:
		time_elapsed = (m_end.QuadPart - m_begin.QuadPart) * 1000000000 / m_freq.QuadPart;
		scope_name = _T("ns");
		break;
	}

	CString performance_check_string;
	performance_check_string.Format(_T("CPerformanceChecker => [%s]' time_elapsed = %I64d%s\n"), m_name, time_elapsed, scope_name);
	OutputDebugString(performance_check_string);
	m_check = true;
}
