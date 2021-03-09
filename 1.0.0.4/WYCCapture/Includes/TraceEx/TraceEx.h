#pragma once
#include <afxwin.h>

class CLoggerEx
{
public:
	CLoggerEx(CString log_path = _T(""), bool is_show_time = true, bool is_write_file = true);
	~CLoggerEx();

	void SetLogPath(CString log_path);
	CString GetLogPath();

	void SetShowTime(bool is_show_time);
	bool GetShowTime();

	void SetWriteFile(bool is_write_file);
	bool GetWriteFile();

	bool HaveLogPath();

	bool Write(CString file_name, CString module_name, int line_no, CString format, va_list arguments);

private:
	void AppendTime(CString& log);
	void AppendFileName(CString& log, CString file_name);
	void AppendModuleName(CString& log, CString module_name);
	void AppendLineNo(CString& log, int line_no);
	bool WriteFile(CString& log);

	CString m_log_path;
	bool m_is_show_time;
	bool m_is_write_file;
};

class CTraceEx
{
public: 
	// 프로그램 초기화 시에 한 번만 호출
	static void Init(bool is_write_file = false, CString log_path = _T(""));
	static void Exit(bool is_write_file = false);

	CTraceEx(CString file_name, CString module_name, int line_no, bool is_show_time = true, bool is_write_file = true, CString log_path = _T(""));

	void SetFileName(CString file_name);
	void SetModuleName(CString module_name);
	void SetLineNo(int line_no);
	void SetShowTime(bool is_show_time);
	void SetWriteFile(bool is_write_file);
	void SetLogPath(CString log_path);

	void operator()(CString format, ...);

	static void TraceDC(HDC hdc, CString file_name);
	static void TraceDC(HDC hdc, int width, int height, CString file_name);

private:
	friend class CLoggerEx;

	static bool CreateFolder(CString folder_name);
	static bool RemoveFolder(CString folder_name);
	static bool RemoveFile(CString file_name);

	static bool IsExistFolder(CString folder_name);
	static bool IsExistFile(CString file_name);

	CString m_file_name;
	CString m_module_name;
	int m_line_no;
	bool m_is_show_time;
	bool m_is_write_file;
	CString m_log_path;
};