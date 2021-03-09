#pragma once
#include <Windows.h>

class CLocalSystemTime
{
public:
	enum DATE_FORMAT { DF_UNKNOWN, DF_YMD, DF_MDY, DF_DMY };
	enum DATE_COMPARE_RESULT { DCR_SAME, DCR_LEFT_BIGGER_THAN_RIGHT, DCR_LEFT_SMALLER_THAN_RIGHT };

	CLocalSystemTime(bool local_time = false);
	CLocalSystemTime(WORD year, WORD month, WORD day, WORD hour = 0, WORD min = 0, WORD sec = 0, WORD ms = 0);
	CLocalSystemTime(CString& time_string, DATE_FORMAT date_format = DF_UNKNOWN);
	CLocalSystemTime(const SYSTEMTIME& r);
	CLocalSystemTime(const CLocalSystemTime& r);
	~CLocalSystemTime();

	CLocalSystemTime& operator=(const SYSTEMTIME& r);
	CLocalSystemTime& operator=(const CLocalSystemTime& r);

	bool operator==(const SYSTEMTIME& r);
	bool operator==(const CLocalSystemTime& r);

	friend bool operator==(const SYSTEMTIME& l, const SYSTEMTIME& r);
	friend bool operator==(const SYSTEMTIME& l, const CLocalSystemTime& r);

	bool operator!=(const SYSTEMTIME& r);
	bool operator!=(const CLocalSystemTime& r);

	friend bool operator!=(const SYSTEMTIME& l, const SYSTEMTIME& r);
	friend bool operator!=(const SYSTEMTIME& l, const CLocalSystemTime& r);

	bool operator>(const SYSTEMTIME& r);
	bool operator>(const CLocalSystemTime& r);

	friend bool operator>(const SYSTEMTIME& l, const SYSTEMTIME& r);
	friend bool operator>(const SYSTEMTIME& l, const CLocalSystemTime& r);

	bool operator<(const SYSTEMTIME& r);
	bool operator<(const CLocalSystemTime& r);

	friend bool operator<(const SYSTEMTIME& l, const SYSTEMTIME& r);
	friend bool operator<(const SYSTEMTIME& l, const CLocalSystemTime& r);

	bool operator>=(const SYSTEMTIME& r);
	bool operator>=(const CLocalSystemTime& r);

	friend bool operator>=(const SYSTEMTIME& l, const SYSTEMTIME& r);
	friend bool operator>=(const SYSTEMTIME& l, const CLocalSystemTime& r);

	bool operator<=(const SYSTEMTIME& r);
	bool operator<=(const CLocalSystemTime& r);

	friend bool operator<=(const SYSTEMTIME& l, const SYSTEMTIME& r);
	friend bool operator<=(const SYSTEMTIME& l, const CLocalSystemTime& r);

	void DebugLog(CString function_name);
	static void DebugLog(CString function_name, SYSTEMTIME& time);

	static DATE_COMPARE_RESULT Compare(SYSTEMTIME& t1, SYSTEMTIME& t2);

	bool IsNullTime();
	static bool IsNullTime(SYSTEMTIME& time);

	bool IsLeapYear();
	static bool IsLeapYear(SYSTEMTIME& time);
	static bool IsLeapYear(WORD year);

	WORD GetEndDay();
	static WORD GetEndDay(SYSTEMTIME& time);
	static WORD GetEndDay(WORD year, WORD month);

	ULONGLONG DiffereceMin(SYSTEMTIME& r);
	ULONGLONG DiffereceMin(CLocalSystemTime& r);
	static ULONGLONG DiffereceMin(SYSTEMTIME& t1, SYSTEMTIME& t2);

	ULONGLONG DiffereceSec(SYSTEMTIME& r);
	ULONGLONG DiffereceSec(CLocalSystemTime& r);
	static ULONGLONG DiffereceSec(SYSTEMTIME& t1, SYSTEMTIME& t2);

	ULONGLONG DiffereceMSec(SYSTEMTIME& r);
	ULONGLONG DiffereceMSec(CLocalSystemTime& r);
	static ULONGLONG DiffereceMSec(SYSTEMTIME& t1, SYSTEMTIME& t2);

	SYSTEMTIME SumMin(LONGLONG minute);
	static SYSTEMTIME SumMin(SYSTEMTIME& time, LONGLONG minute);

	SYSTEMTIME SumSec(LONGLONG second);
	static SYSTEMTIME SumSec(SYSTEMTIME& time, LONGLONG second);

	SYSTEMTIME SumMSec(LONGLONG millisecond);
	static SYSTEMTIME SumMSec(SYSTEMTIME& time, LONGLONG millisecond);

	CString ToString(bool date = true, bool time = true, bool millisecond_view = false, DATE_FORMAT date_format = DF_UNKNOWN);
	static CString SysTimeToStr(SYSTEMTIME& sys_time, bool date = true, bool time = true, bool millisecond_view = false, DATE_FORMAT date_format = DF_UNKNOWN);

	void FromString(CString& time_string, DATE_FORMAT date_format = DF_UNKNOWN);
	static SYSTEMTIME StrToSysTime(CString& time_string, DATE_FORMAT date_format = DF_UNKNOWN);

	SYSTEMTIME ToUTC();
	void FromLocal(SYSTEMTIME& local_time);
	static SYSTEMTIME LocalToUTC(SYSTEMTIME& local_time);

	SYSTEMTIME ToLocal();
	void FromUTC(SYSTEMTIME& utc_time);
	static SYSTEMTIME UTCToLocal(SYSTEMTIME& utc_time);

	void SetYear(WORD year) { m_time.wYear = year; }
	WORD GetYear() const { return m_time.wYear; }

	void SetMonth(WORD month) { m_time.wMonth = month; }
	WORD GetMonth() const { return m_time.wMonth; }

	void SetDay(WORD day) { m_time.wDay = day; }
	WORD GetDay() const { return m_time.wDay; }

	void SetHour(WORD hour) { m_time.wHour = hour; }
	WORD GetHour() const { return m_time.wHour; }

	void SetMinute(WORD minute) { m_time.wMinute = minute; }
	WORD GetMinute() const { return m_time.wMinute; }

	void SetSecond(WORD second) { m_time.wSecond = second; }
	WORD GetSecond() const { return m_time.wSecond; }

	void SetMillisecond(WORD millisecond) { m_time.wMilliseconds = millisecond; }
	WORD GetMillisecond() const { return m_time.wMilliseconds; }

	void SetDayOfWeek(WORD dayofweek) { m_time.wDayOfWeek = dayofweek; }
	WORD GetDayOfWeek() const { return m_time.wDayOfWeek; }

	operator SYSTEMTIME() const { return m_time; }
	SYSTEMTIME Get() const { return m_time; }
	
private:
	DATE_FORMAT SystemDateFormat();
	CString DateStringFormat(DATE_FORMAT date_format);
	CString TimeStringFormat(bool millisecond_view = true);

	SYSTEMTIME m_time;
};

bool operator==(const SYSTEMTIME& l, const SYSTEMTIME& r);
bool operator==(const SYSTEMTIME& l, const CLocalSystemTime& r);

bool operator!=(const SYSTEMTIME& l, const SYSTEMTIME& r);
bool operator!=(const SYSTEMTIME& l, const CLocalSystemTime& r);

bool operator>(const SYSTEMTIME& l, const SYSTEMTIME& r);
bool operator>(const SYSTEMTIME& l, const CLocalSystemTime& r);

bool operator<(const SYSTEMTIME& l, const SYSTEMTIME& r);
bool operator<(const SYSTEMTIME& l, const CLocalSystemTime& r);

bool operator>=(const SYSTEMTIME& l, const SYSTEMTIME& r);
bool operator>=(const SYSTEMTIME& l, const CLocalSystemTime& r);

bool operator<=(const SYSTEMTIME& l, const SYSTEMTIME& r);
bool operator<=(const SYSTEMTIME& l, const CLocalSystemTime& r);