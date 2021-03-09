#pragma once
#include <Windows.h>
#include <vector>
#include <afxwin.h>

#pragma comment (lib, "dwmapi.lib")

typedef struct _WND_INFO
{
	HWND hwnd;
	CString name;
	CString title;
	CRect position;
	UINT z_order;

	_WND_INFO()
	{
		hwnd = NULL;
		z_order = UINT_MAX;
	}
	~_WND_INFO()
	{

	}
	_WND_INFO(HWND hwnd, CString name, CString title, CRect position, UINT z_order = UINT_MAX)
	{
		this->hwnd = hwnd;
		this->name = name;
		this->title = title;
		this->position = position;
		this->z_order = z_order;
	}
	_WND_INFO(const _WND_INFO& r)
	{
		this->hwnd = r.hwnd;
		this->name = r.name;
		this->title = r.title;
		this->position = r.position;
		this->z_order = r.z_order;
	}
	_WND_INFO& operator=(const _WND_INFO& r)
	{
		if (this == &r)
			return *this;

		this->hwnd = r.hwnd;
		this->name = r.name;
		this->title = r.title;
		this->position = r.position;
		this->z_order = r.z_order;

		return *this;
	}
	bool operator==(const _WND_INFO& r)
	{
		return (this->hwnd == r.hwnd &&
				this->name == r.name &&
				this->title == r.title &&
				this->position == r.position &&
				this->z_order == r.z_order);
	}
	bool operator!=(const _WND_INFO& r)
	{
		return !(*this == r);
	}
} WND_INFO, *LPWND_INFO;

class CWndManager
{
public:
	static CWndManager* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CWndManager();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	bool LoadEnabledWindows();
	bool LoadAllWindows();
	void CalcZOrderOfEnabledWindows();
	void CalcZOrderOfAllWindows();

	std::vector<WND_INFO> GetEnabledWndList();
	std::vector<WND_INFO> GetAllWndList();
	WND_INFO GetTopEnabledWndFromPoint(CPoint& point);
	WND_INFO GetTopAllWndFromFromPoint(CPoint& point);

	void AddExceptionList(CString exception);
	
private:
	CWndManager();
	CWndManager(const CWndManager& r) {}
	~CWndManager();

	void CheckWindowStyle(HWND hWnd);

	static void ReleaseSingletone()
	{
		if (m_singletone)
			delete m_singletone;
	}

	static CWndManager* m_singletone;

	static BOOL GetEnabledWindowList(HWND hWnd, LPARAM lParam);
	static BOOL GetAllWindowList(HWND hWnd, LPARAM lParam);

	CRITICAL_SECTION m_cs_enabled_wnd_list;
	std::vector<WND_INFO> m_enabled_wnd_list;

	CRITICAL_SECTION m_cs_all_wnd_list;
	std::vector<WND_INFO> m_all_wnd_list;

	CRITICAL_SECTION m_cs_exception_list;
	std::vector<CString> m_exception_list;
};

