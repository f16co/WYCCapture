#pragma once
#include <Windows.h>

#ifndef PERFORMANCE_CHECK_INIT
#define PERFORMANCE_CHECK_INIT(scope) CPerformanceChecker checker(_T(__FUNCTION__), scope);
#endif

#ifndef PERFORMANCE_CHECK_CHANGE_SCOPE
#define PERFORMANCE_CHECK_CHANGE_SCOPE(scope) checker.ChangeScope(scope);
#endif

#ifndef PERFORMANCE_CHECK_START
#define PERFORMANCE_CHECK_START(scope) checker.Start();
#endif

#ifndef PERFORMANCE_CHECK_CHECK
#define PERFORMANCE_CHECK_CHECK() checker.Check();
#endif

class CPerformanceChecker
{
public:
	enum PERFORMANCE_CHECK_SCOPE { PERFORMACE_CHECK_SEC, PERFORMACE_CHECK_MS, PERFORMACE_CHECK_US, PERFORMACE_CHECK_NS };

	CPerformanceChecker(CString name, PERFORMANCE_CHECK_SCOPE performance_check_scope = PERFORMACE_CHECK_MS);
	~CPerformanceChecker();

	void ChangeScope(PERFORMANCE_CHECK_SCOPE performance_check_scope) { m_performance_check_scope = performance_check_scope; }
	void Start();
	void Check();

private:
	bool m_check;
	CString m_name;

	PERFORMANCE_CHECK_SCOPE m_performance_check_scope;
	LARGE_INTEGER m_freq, m_begin, m_end;
};