#include "stdafx.h"
#include "TraceEx.h"

CLoggerEx::CLoggerEx(CString log_path /*= _T("")*/, bool is_show_time /*= true*/, bool is_write_file /*= true*/)
{
	SetShowTime(is_show_time);
	SetWriteFile(is_write_file);
	if (m_is_write_file)
		SetLogPath(log_path);
}

CLoggerEx::~CLoggerEx()
{
}

void CLoggerEx::SetLogPath(CString log_path)
{
	CString exe_path = log_path;
	if (exe_path.IsEmpty())
	{
		TCHAR full_path[MAX_PATH] = { 0, };
		::GetModuleFileName(NULL, full_path, MAX_PATH);
		exe_path = full_path;

		exe_path = exe_path.Left(exe_path.ReverseFind(_T('\\')) + 1);
		exe_path += _T("Log\\");
		if (!CTraceEx::IsExistFolder(exe_path))
			CTraceEx::CreateFolder(exe_path);

		SYSTEMTIME time;
		GetLocalTime(&time);
		CString time_string;
		time_string.Format(_T("%04d-%02d-%02d.txt"), time.wYear, time.wMonth, time.wDay);
		exe_path += time_string;
	}

	m_log_path = exe_path;
}

CString CLoggerEx::GetLogPath()
{
	return m_log_path;
}

void CLoggerEx::SetShowTime(bool is_show_time)
{
	m_is_show_time = is_show_time;
}

bool CLoggerEx::GetShowTime()
{
	return m_is_show_time;
}

void CLoggerEx::SetWriteFile(bool is_write_file)
{
	m_is_write_file = is_write_file;
}

bool CLoggerEx::GetWriteFile()
{
	return m_is_write_file;
}

bool CLoggerEx::HaveLogPath()
{
	return !m_log_path.IsEmpty();
}

bool CLoggerEx::Write(CString file_name, CString module_name, int line_no, CString format, va_list arguments)
{
	CString info_string;
	if (GetShowTime())
		AppendTime(info_string);

	if (!file_name.IsEmpty())
		AppendFileName(info_string, file_name);

	if (line_no >= 0)
		AppendLineNo(info_string, line_no);

	if (!module_name.IsEmpty())
		AppendModuleName(info_string, module_name);

	CString data_string;
	data_string.FormatV(format, arguments);

	CString log;
	log.Format(_T("%s %s"), info_string, data_string);

	OutputDebugString(log);
	if (GetWriteFile())
		if (!WriteFile(log))
			return false;

	return true;
}

void CLoggerEx::AppendTime(CString& log)
{
	CString time_string;
	SYSTEMTIME local_time;
	GetLocalTime(&local_time);

	time_string.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d:%03d] "), local_time.wYear, local_time.wMonth, local_time.wDay, local_time.wHour, local_time.wMinute, local_time.wSecond, local_time.wMilliseconds);

	log += time_string;
}

void CLoggerEx::AppendFileName(CString& log, CString file_name)
{
	int index = file_name.ReverseFind('\\');
	if (index >= 0)
		file_name = file_name.Right(file_name.GetLength() - index - 1);

	log += _T("\'");
	log += file_name;
	log += _T("\' ");
}

void CLoggerEx::AppendModuleName(CString& log, CString module_name)
{
	log += _T("[");
	log += module_name;
	log += _T("] ");
}

void CLoggerEx::AppendLineNo(CString& log, int line_no)
{
	CString lone_no_string;
	lone_no_string.Format(_T("(%d) "), line_no);
	log += lone_no_string;
}

bool CLoggerEx::WriteFile(CString& log)
{
	FILE* fp = fopen(CStringA(m_log_path), "a");
	if (!fp)
		return false;

	fseek(fp, 0, SEEK_END);
	int length = log.GetLength();
	for (int i = 0; i < length; i++)
		fputc(log[i], fp);
	fclose(fp);

	return true;
}

void CTraceEx::Init(bool is_write_file, CString log_path /*= _T("")*/)
{
	CString exe_path = log_path;
	if (is_write_file)
	{
		if (exe_path.IsEmpty())
		{
			TCHAR full_path[MAX_PATH] = { 0, };
			::GetModuleFileName(NULL, full_path, MAX_PATH);
			exe_path = full_path;

			exe_path = exe_path.Left(exe_path.ReverseFind(_T('\\')) + 1);
			exe_path += _T("Log\\");
			if (!IsExistFolder(exe_path))
				CreateFolder(exe_path);

			SYSTEMTIME time;
			GetLocalTime(&time);
			CString time_string;
			time_string.Format(_T("%04d-%02d-%02d.txt"), time.wYear, time.wMonth, time.wDay);
			exe_path += time_string;
		}
	}

	if (IsExistFile(exe_path))
		RemoveFile(exe_path);

	CTraceEx(_T(""), _T(""), -1, true, is_write_file)(_T("---------- Start Program ----------\n"));
}

void CTraceEx::Exit(bool is_write_file)
{
	CTraceEx(_T(""), _T(""), -1, true, is_write_file)(_T("---------- Exit Program ----------\n"));
}

CTraceEx::CTraceEx(CString file_name, CString module_name, int line_no, bool is_show_time, bool is_write_file, CString log_path)
{
	m_file_name = file_name;
	m_module_name = module_name;
	m_line_no = line_no;
	m_is_show_time = is_show_time;
	m_is_write_file = is_write_file;
	m_log_path = log_path;
}

CTraceEx::~CTraceEx()
{
}

void CTraceEx::SetFileName(CString file_name)
{
	m_file_name = file_name;
}

void CTraceEx::SetModuleName(CString module_name)
{
	m_module_name = module_name;
}

void CTraceEx::SetLineNo(int line_no)
{
	m_line_no = line_no;
}

void CTraceEx::SetShowTime(bool is_show_time)
{
	m_is_show_time = is_show_time;
}

void CTraceEx::SetWriteFile(bool is_write_file)
{
	m_is_write_file = is_write_file;
}

void CTraceEx::SetLogPath(CString log_path)
{
	m_log_path = log_path;
}

void CTraceEx::operator()(CString format, ...)
{
	va_list arg;
	va_start(arg, format);
	CLoggerEx logger(m_log_path, m_is_show_time, m_is_write_file);
	logger.Write(m_file_name, m_module_name, m_line_no, format, arg);
	va_end(arg);
}

void CTraceEx::TraceDC(HDC hdc, CString file_name)
{
	HWND hwnd = ::WindowFromDC(hdc);
	RECT rect;
	::GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	TraceDC(hdc, width, height, file_name);
}

void CTraceEx::TraceDC(HDC hdc, int width, int height, CString file_name)
{
	CImage image;
	if (!image.Create(width, height, GetDeviceCaps(hdc, BITSPIXEL)))
		return;

	::BitBlt(
		image.GetDC(),
		0, 0, width, height,
		hdc,
		0, 0,
		SRCCOPY);
	image.ReleaseDC();
	image.Save(file_name);
}

bool CTraceEx::CreateFolder(CString folder_name)
{
	return CreateDirectory(folder_name, NULL) ? true : false;
}

bool CTraceEx::RemoveFolder(CString folder_name)
{
	return RemoveDirectory(folder_name) ? true : false;
}

bool CTraceEx::RemoveFile(CString file_name)
{
	return DeleteFile(file_name) ? true : false;
}

bool CTraceEx::IsExistFolder(CString folder_name)
{
	bool ret = false;
	DWORD dwResult;
	dwResult = GetFileAttributes(folder_name);

	return (dwResult != -1) ? true : false;
}

bool CTraceEx::IsExistFile(CString file_name)
{
	CFileStatus status;
	return CFile::GetStatus(file_name, status) ? true : false;
}