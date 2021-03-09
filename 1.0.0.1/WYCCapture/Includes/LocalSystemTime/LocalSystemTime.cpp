#include "stdafx.h"
#include "LocalSystemTime.h"

CLocalSystemTime::CLocalSystemTime(bool local_time /*= false*/)
{
	if (local_time)
		GetLocalTime(&m_time);
	else
		ZeroMemory(&m_time, sizeof(SYSTEMTIME));
}

CLocalSystemTime::CLocalSystemTime(CString& time_string, DATE_FORMAT date_format /*= DF_UNKNOWN*/)
{
	ZeroMemory(&m_time, sizeof(SYSTEMTIME));
	FromString(time_string, date_format);
}

CLocalSystemTime::CLocalSystemTime(const SYSTEMTIME& r)
{
	m_time = r;
}

CLocalSystemTime::CLocalSystemTime(const CLocalSystemTime& r)
{
	m_time = r.m_time;
}

CLocalSystemTime::~CLocalSystemTime()
{
}

CLocalSystemTime& CLocalSystemTime::operator=(const SYSTEMTIME& r)
{
	m_time = r;

	return *this;
}

CLocalSystemTime& CLocalSystemTime::operator=(const CLocalSystemTime& r)
{
	if (this == &r)
		return *this;

	m_time = r.m_time;

	return *this;
}

bool CLocalSystemTime::operator==(const SYSTEMTIME& r)
{
	if (m_time.wYear == r.wYear)
	{
		if (m_time.wMonth == r.wMonth)
		{
			if (m_time.wDay == r.wDay)
			{
				if (m_time.wHour == r.wHour)
				{
					if (m_time.wMinute == r.wMinute)
					{
						if (m_time.wSecond == r.wSecond)
						{
							if (m_time.wMilliseconds == r.wMilliseconds)
								return true;
							else
								return false;
						}
						else
							return false;
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool CLocalSystemTime::operator==(const CLocalSystemTime& r)
{
	return operator==(r.m_time);
}

bool CLocalSystemTime::operator>(const SYSTEMTIME& r)
{
	if (m_time.wYear > r.wYear)
		return true;
	else if (m_time.wYear < r.wYear)
		return false;
	else
	{
		if (m_time.wMonth > r.wMonth)
			return true;
		else if (m_time.wMonth < r.wMonth)
			return false;
		else
		{
			if (m_time.wDay > r.wDay)
				return true;
			else if (m_time.wDay < r.wDay)
				return false;
			else
			{
				if (m_time.wHour > r.wHour)
					return true;
				else if (m_time.wHour < r.wHour)
					return false;
				else
				{
					if (m_time.wMinute > r.wMinute)
						return true;
					else if (m_time.wMinute < r.wMinute)
						return false;
					else
					{
						if (m_time.wSecond > r.wSecond)
							return true;
						else if (m_time.wSecond < r.wSecond)
							return false;
						else
						{
							if (m_time.wMilliseconds > r.wMilliseconds)
								return true;
							else
								return false;
						}
					}
				}
			}
		}
	}
}

bool CLocalSystemTime::operator>(const CLocalSystemTime& r)
{
	return operator>(r.m_time);
}

bool CLocalSystemTime::operator<(const SYSTEMTIME& r)
{
	if (m_time.wYear < r.wYear)
		return true;
	else if (m_time.wYear > r.wYear)
		return false;
	else
	{
		if (m_time.wMonth < r.wMonth)
			return true;
		else if (m_time.wMonth > r.wMonth)
			return false;
		else
		{
			if (m_time.wDay < r.wDay)
				return true;
			else if (m_time.wDay > r.wDay)
				return false;
			else
			{
				if (m_time.wHour < r.wHour)
					return true;
				else if (m_time.wHour > r.wHour)
					return false;
				else
				{
					if (m_time.wMinute < r.wMinute)
						return true;
					else if (m_time.wMinute > r.wMinute)
						return false;
					else
					{
						if (m_time.wSecond < r.wSecond)
							return true;
						else if (m_time.wSecond > r.wSecond)
							return false;
						else
						{
							if (m_time.wMilliseconds < r.wMilliseconds)
								return true;
							else
								return false;
						}
					}
				}
			}
		}
	}
}

bool CLocalSystemTime::operator<(const CLocalSystemTime& r)
{
	return operator<(r.m_time);
}

bool CLocalSystemTime::operator>=(const SYSTEMTIME& r)
{
	if (m_time.wYear > r.wYear)
		return true;
	else if (m_time.wYear < r.wYear)
		return false;
	else
	{
		if (m_time.wMonth > r.wMonth)
			return true;
		else if (m_time.wMonth < r.wMonth)
			return false;
		else
		{
			if (m_time.wDay > r.wDay)
				return true;
			else if (m_time.wDay < r.wDay)
				return false;
			else
			{
				if (m_time.wHour > r.wHour)
					return true;
				else if (m_time.wHour < r.wHour)
					return false;
				else
				{
					if (m_time.wMinute > r.wMinute)
						return true;
					else if (m_time.wMinute < r.wMinute)
						return false;
					else
					{
						if (m_time.wSecond > r.wSecond)
							return true;
						else if (m_time.wSecond < r.wSecond)
							return false;
						else
						{
							if (m_time.wMilliseconds >= r.wMilliseconds)
								return true;
							else
								return false;
						}
					}
				}
			}
		}
	}
}

bool CLocalSystemTime::operator>=(const CLocalSystemTime& r)
{
	return operator>=(r.m_time);
}

bool CLocalSystemTime::operator<=(const SYSTEMTIME& r)
{
	if (m_time.wYear < r.wYear)
		return true;
	else if (m_time.wYear > r.wYear)
		return false;
	else
	{
		if (m_time.wMonth < r.wMonth)
			return true;
		else if (m_time.wMonth > r.wMonth)
			return false;
		else
		{
			if (m_time.wDay < r.wDay)
				return true;
			else if (m_time.wDay > r.wDay)
				return false;
			else
			{
				if (m_time.wHour < r.wHour)
					return true;
				else if (m_time.wHour > r.wHour)
					return false;
				else
				{
					if (m_time.wMinute < r.wMinute)
						return true;
					else if (m_time.wMinute > r.wMinute)
						return false;
					else
					{
						if (m_time.wSecond < r.wSecond)
							return true;
						else if (m_time.wSecond > r.wSecond)
							return false;
						else
						{
							if (m_time.wMilliseconds <= r.wMilliseconds)
								return true;
							else
								return false;
						}
					}
				}
			}
		}
	}
}

bool CLocalSystemTime::operator<=(const CLocalSystemTime& r)
{
	return operator<=(r.m_time);
}


void CLocalSystemTime::DebugLog(CString function_name)
{
	CString str = ToString();
	str = _T("[") + function_name + _T("] ") + str + _T("\n");
	OutputDebugString(str);
}

void CLocalSystemTime::DebugLog(CString function_name, SYSTEMTIME& time)
{
	CLocalSystemTime local_time(time);
	local_time.DebugLog(function_name);
}

CLocalSystemTime::DATE_COMPARE_RESULT CLocalSystemTime::Compare(SYSTEMTIME& t1, SYSTEMTIME& t2)
{
	CLocalSystemTime lt1(t1), lt2(t2);
	
	if (lt1 > lt2)
		return DCR_LEFT_BIGGER_THAN_RIGHT;
	else if (lt1 < lt2)
		return DCR_LEFT_SMALLER_THAN_RIGHT;

	return DCR_SAME;
}

bool CLocalSystemTime::IsNullTime()
{
	CLocalSystemTime null_time;
	return (*this == null_time);
}

bool CLocalSystemTime::IsNullTime(SYSTEMTIME& time)
{
	CLocalSystemTime target_time(time);
	return target_time.IsNullTime();
}

bool CLocalSystemTime::IsLeapYear()
{
	WORD year = m_time.wYear;
	return IsLeapYear(year);
}

bool CLocalSystemTime::IsLeapYear(SYSTEMTIME& time)
{
	CLocalSystemTime local_time(time);
	return local_time.IsLeapYear();
}

bool CLocalSystemTime::IsLeapYear(WORD year)
{
	if( !(year % 400) )
		return true;
	if( !(year % 100) )
		return false;
	if( !(year % 4) )
		return true;
	return false;
}

WORD CLocalSystemTime::GetEndDay()
{
	WORD days = 0;
	if (!IsNullTime())
	{
		WORD month = m_time.wMonth;
		WORD year = m_time.wYear;
		days = GetEndDay(year, month);
	}
	return days;
}

WORD CLocalSystemTime::GetEndDay(SYSTEMTIME& time)
{
	CLocalSystemTime local_time(time);
	return local_time.GetEndDay();
}

WORD CLocalSystemTime::GetEndDay(WORD year, WORD month)
{
	WORD days = 0;
	if (month > 0 && year > 0)
	{
		const WORD remainder[12] = { 3, 0, 3, 2, 3, 2, 3, 3, 2, 3, 2, 3 };
		days = remainder[month - 1] + 28;
		if( month == 2 && IsLeapYear(year) )
			days++;
	}
	return days;
}

ULONGLONG CLocalSystemTime::DiffereceMin(SYSTEMTIME& r)
{
	return DiffereceSec(r) / 60;
}

ULONGLONG CLocalSystemTime::DiffereceMin(CLocalSystemTime& r)
{
	return DiffereceMin(r.m_time);
}

ULONGLONG CLocalSystemTime::DiffereceMin(SYSTEMTIME& t1, SYSTEMTIME& t2)
{
	return DiffereceSec(t1, t2) / 60;
}

ULONGLONG CLocalSystemTime::DiffereceSec(SYSTEMTIME& r)
{
	return DiffereceMSec(r) / 1000;
}

ULONGLONG CLocalSystemTime::DiffereceSec(CLocalSystemTime& r)
{
	return DiffereceSec(r.m_time);
}

ULONGLONG CLocalSystemTime::DiffereceSec(SYSTEMTIME& t1, SYSTEMTIME& t2)
{
	return DiffereceMSec(t1, t2) / 1000;
}

ULONGLONG CLocalSystemTime::DiffereceMSec(SYSTEMTIME& r)
{
	FILETIME filetime;
	ULARGE_INTEGER uliTime1, uliTime2;
	ULONGLONG ullTime1, ullTime2;

	SystemTimeToFileTime(&m_time, &filetime);
	uliTime1.LowPart = filetime.dwLowDateTime;
	uliTime1.HighPart = filetime.dwHighDateTime;
	ullTime1 = uliTime1.QuadPart;

	SystemTimeToFileTime(&r, &filetime);
	uliTime2.LowPart = filetime.dwLowDateTime;
	uliTime2.HighPart = filetime.dwHighDateTime;
	ullTime2 = uliTime2.QuadPart;

	ULONGLONG diff = ullTime1 - ullTime2;

	return diff / 10000;
}

ULONGLONG CLocalSystemTime::DiffereceMSec(CLocalSystemTime& r)
{
	return DiffereceMSec(r.m_time);
}

ULONGLONG CLocalSystemTime::DiffereceMSec(SYSTEMTIME& t1, SYSTEMTIME& t2)
{
	CLocalSystemTime lt1(t1);
	return lt1.DiffereceMSec(t2);
}

SYSTEMTIME CLocalSystemTime::SumMin(LONGLONG minute)
{
	return SumSec(minute * 60);
}

SYSTEMTIME CLocalSystemTime::SumMin(SYSTEMTIME& time, LONGLONG minute)
{
	CLocalSystemTime local_time(time);
	return local_time.SumMin(minute);
}

SYSTEMTIME CLocalSystemTime::SumSec(LONGLONG second)
{
	return SumMSec(second * 1000);
}

SYSTEMTIME CLocalSystemTime::SumSec(SYSTEMTIME& time, LONGLONG second)
{
	CLocalSystemTime local_time(time);
	return local_time.SumSec(second);
}

SYSTEMTIME CLocalSystemTime::SumMSec(LONGLONG millisecond)
{
	CLocalSystemTime local_time(m_time);
	SYSTEMTIME system_time = local_time;
	FILETIME file_time;
	LARGE_INTEGER large_integer;
	SystemTimeToFileTime(&system_time, &file_time);
	large_integer.HighPart = file_time.dwHighDateTime;
	large_integer.LowPart = file_time.dwLowDateTime;

	large_integer.QuadPart += (10000 * millisecond);

	file_time.dwHighDateTime = (DWORD)large_integer.HighPart;
	file_time.dwLowDateTime = (DWORD)large_integer.LowPart;
	FileTimeToSystemTime(&file_time, &system_time);

	return system_time;
}

SYSTEMTIME CLocalSystemTime::SumMSec(SYSTEMTIME& time, LONGLONG millisecond)
{
	CLocalSystemTime local_time(time);
	return local_time.SumMSec(millisecond);
}

CString CLocalSystemTime::ToString(bool date /*= true*/, bool time /*= true*/, bool millisecond_view /*= false*/, DATE_FORMAT date_format /*= DF_UNKNOWN*/)
{
	CString time_string;
	if (date)
		time_string += DateStringFormat(date_format);
	if (date && time)
		time_string += _T(" ");
	if (time)
		time_string += TimeStringFormat(millisecond_view);
	return time_string;
}

CString CLocalSystemTime::SysTimeToStr(SYSTEMTIME& sys_time, bool date /*= true*/, bool time /*= true*/, bool millisecond_view /*= false*/, DATE_FORMAT date_format /*= DF_UNKNOWN*/)
{
	CLocalSystemTime local_time(sys_time);
	return local_time.ToString(date, time, millisecond_view, date_format);
}

void CLocalSystemTime::FromString(CString& time_string, DATE_FORMAT date_format)
{
	time_string.Replace(_T("-"), _T(""));
	time_string.Replace(_T(":"), _T(""));
	time_string.Replace(_T(" "), _T(""));
	switch (date_format)
	{
	case CLocalSystemTime::DF_UNKNOWN:
		FromString(time_string, SystemDateFormat());
		break;
	case CLocalSystemTime::DF_YMD:
		m_time.wYear = _ttoi(time_string.Mid(0, 4));
		m_time.wMonth = _tstoi(time_string.Mid(4, 2));
		m_time.wDay = _tstoi(time_string.Mid(6, 2));
		m_time.wHour = _tstoi(time_string.Mid(8, 2));
		m_time.wMinute = _tstoi(time_string.Mid(10, 2));
		m_time.wSecond = _tstoi(time_string.Mid(12, 2));
		break;
	case CLocalSystemTime::DF_MDY:
		m_time.wMonth = _tstoi(time_string.Mid(0, 2));
		m_time.wDay = _tstoi(time_string.Mid(2, 2));
		m_time.wYear = _ttoi(time_string.Mid(4, 4));
		m_time.wHour = _tstoi(time_string.Mid(8, 2));
		m_time.wMinute = _tstoi(time_string.Mid(10, 2));
		m_time.wSecond = _tstoi(time_string.Mid(12, 2));
		break;
	case CLocalSystemTime::DF_DMY:
		m_time.wDay = _tstoi(time_string.Mid(0, 2));
		m_time.wMonth = _tstoi(time_string.Mid(2, 2));
		m_time.wYear = _ttoi(time_string.Mid(4, 4));
		m_time.wHour = _tstoi(time_string.Mid(8, 2));
		m_time.wMinute = _tstoi(time_string.Mid(10, 2));
		m_time.wSecond = _tstoi(time_string.Mid(12, 2));
		break;
	}
	if (time_string.GetLength() == 17)
		m_time.wMilliseconds = _ttoi(time_string.Mid(14, 3));
	else
		m_time.wMilliseconds = 0;
}
SYSTEMTIME CLocalSystemTime::StrToSysTime(CString& time_string, DATE_FORMAT date_format /*= DF_UNKNOWN*/)
{
	CLocalSystemTime local_time;
	local_time.FromString(time_string, date_format);
	return local_time.Get();
}

SYSTEMTIME CLocalSystemTime::ToUTC()
{
	FILETIME utc_file_time, local_file_time;
	SYSTEMTIME utc_time;
	ZeroMemory(&utc_time, sizeof(SYSTEMTIME));
	SystemTimeToFileTime(&m_time, &local_file_time);
	LocalFileTimeToFileTime(&local_file_time, &utc_file_time);
	FileTimeToSystemTime(&utc_file_time, &utc_time);

	return utc_time;
}

SYSTEMTIME CLocalSystemTime::LocalToUTC(SYSTEMTIME& local_time)
{
	CLocalSystemTime time(local_time);
	return time.ToUTC();
}

void CLocalSystemTime::FromUTC(SYSTEMTIME& utc_time)
{
	FILETIME utc_file_time, local_file_time;
	SystemTimeToFileTime(&utc_time, &utc_file_time);
	FileTimeToLocalFileTime(&utc_file_time, &local_file_time);
	FileTimeToSystemTime(&local_file_time, &m_time);
}

SYSTEMTIME CLocalSystemTime::UTCToLocal(SYSTEMTIME& utc_time)
{
	CLocalSystemTime time(utc_time);
	time.FromUTC(utc_time);
	return time.Get();
}

CLocalSystemTime::DATE_FORMAT CLocalSystemTime::SystemDateFormat()
{
	TCHAR tmp[256];
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SSHORTDATE, tmp, sizeof(tmp));

	switch (tmp[0])
	{
	case 'y':
		return DF_YMD;
	case 'M':
		return DF_MDY;
	case 'd':
		return DF_DMY;
	}
	return DF_YMD;
}

CString CLocalSystemTime::DateStringFormat(DATE_FORMAT date_format)
{
	CString date_format_string;

	switch (date_format)
	{
	case CLocalSystemTime::DF_UNKNOWN:
		date_format_string = DateStringFormat(SystemDateFormat());
		break;
	case CLocalSystemTime::DF_YMD:
		date_format_string.Format(_T("%04d-%02d-%02d"), m_time.wYear, m_time.wMonth, m_time.wDay);
		break;
	case CLocalSystemTime::DF_MDY:
		date_format_string.Format(_T("%02d-%02d-%04d"), m_time.wMonth, m_time.wDay, m_time.wYear);
		break;
	case CLocalSystemTime::DF_DMY:
		date_format_string.Format(_T("%02d-%02d-%04d"), m_time.wDay, m_time.wMonth, m_time.wYear);
		break;
	}
	return date_format_string;
}

CString CLocalSystemTime::TimeStringFormat(bool millisecond_view /*= true*/)
{
	CString time_format_string;
	if (millisecond_view)
		time_format_string.Format(_T("%02d:%02d:%02d:%03d"), m_time.wHour, m_time.wMinute, m_time.wSecond, m_time.wMilliseconds);
	else
		time_format_string.Format(_T("%02d:%02d:%02d"), m_time.wHour, m_time.wMinute, m_time.wSecond);
	return time_format_string;
}
