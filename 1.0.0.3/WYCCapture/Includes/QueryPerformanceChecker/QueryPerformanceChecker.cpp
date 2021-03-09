#include "stdafx.h"
#include "QueryPerformanceChecker.h"

CQueryPerformanceChecker::CQueryPerformanceChecker(CString name, PERFORMANCE_CHECK_SCOPE performance_check_scope /*= PERFORMACE_CHECK_MS*/, bool auto_check /*= false*/)
{
	m_name = name;
	m_performance_check_scope = performance_check_scope;
	m_auto_check = auto_check;

	Start();
}

CQueryPerformanceChecker::~CQueryPerformanceChecker()
{
	if (!m_check && m_auto_check)
		Check();
}

void CQueryPerformanceChecker::Start()
{
	m_check = false;
	QueryPerformanceFrequency(&m_freq);
	QueryPerformanceCounter(&m_begin);
}

void CQueryPerformanceChecker::Check()
{
	__int64 time_elapsed = GetElapsed();
	CString scope_name = _T("");

	switch (m_performance_check_scope)
	{
	case CQueryPerformanceChecker::PERFORMACE_CHECK_SEC:
		scope_name = _T("s");
		break;
	case CQueryPerformanceChecker::PERFORMACE_CHECK_MS:
		scope_name = _T("ms");
		break;
	case CQueryPerformanceChecker::PERFORMACE_CHECK_US:
		scope_name = _T("us");
		break;
	case CQueryPerformanceChecker::PERFORMACE_CHECK_NS:
		scope_name = _T("ns");
		break;
	}

	CString performance_check_string;
	performance_check_string.Format(_T("[%s] performance = %I64d%s\n"), m_name, time_elapsed, scope_name);
	TRACE(performance_check_string);
	m_check = true;
}

__int64 CQueryPerformanceChecker::GetElapsed()
{
	QueryPerformanceCounter(&m_end);
	__int64 time_elapsed = 0;

	switch (m_performance_check_scope)
	{
	case CQueryPerformanceChecker::PERFORMACE_CHECK_SEC:
		time_elapsed = (m_end.QuadPart - m_begin.QuadPart) / m_freq.QuadPart;
		break;
	case CQueryPerformanceChecker::PERFORMACE_CHECK_MS:
		time_elapsed = (m_end.QuadPart - m_begin.QuadPart) * 1000 / m_freq.QuadPart;
		break;
	case CQueryPerformanceChecker::PERFORMACE_CHECK_US:
		time_elapsed = (m_end.QuadPart - m_begin.QuadPart) * 1000000 / m_freq.QuadPart;
		break;
	case CQueryPerformanceChecker::PERFORMACE_CHECK_NS:
		time_elapsed = (m_end.QuadPart - m_begin.QuadPart) * 1000000000 / m_freq.QuadPart;
		break;
	}

	return time_elapsed;
}
