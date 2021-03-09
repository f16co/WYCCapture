#include "stdafx.h"
#include "WndManager.h"

CWndManager* CWndManager::m_singletone = NULL;

BOOL CWndManager::GetEnabledWindowList(HWND hWnd, LPARAM lParam)
{
	CWndManager* main = reinterpret_cast<CWndManager*>(lParam);
	if (main)
	{
		if (hWnd)
		{
			CWnd* wnd = CWnd::FromHandle(hWnd);
			if (!wnd->IsWindowVisible())
				return TRUE;

			WINDOWPLACEMENT window_placement = { 0, };
			wnd->GetWindowPlacement(&window_placement);
			if (window_placement.showCmd == SW_HIDE)
				return TRUE;

			if (::GetWindowTextLength(hWnd) <= 0)
				return TRUE;

			if (::GetParent(hWnd))
				return TRUE;

			TCHAR name[MAX_PATH] = { 0, };
			TCHAR title[MAX_PATH] = { 0, };

			::GetClassName(wnd->GetSafeHwnd(), name, MAX_PATH);
			::GetWindowText(wnd->GetSafeHwnd(), title, MAX_PATH);

			if (_tcscmp(title, _T("Program Manager")) == 0)
				return TRUE;

			EnterCriticalSection(&main->m_cs_exception_list);
			std::vector<CString> exception_list = main->m_exception_list;
			LeaveCriticalSection(&main->m_cs_exception_list);
			for (int i = 0; i < (int)exception_list.size(); i++)
			{
				if (exception_list[i].CompareNoCase(title) == 0)
					return TRUE;
			}

			CRect window_rect = { 0, };
			wnd->GetWindowRect(window_rect);

			CRect frame_rect;
			DwmGetWindowAttribute(wnd->GetSafeHwnd(), DWMWA_EXTENDED_FRAME_BOUNDS, frame_rect, sizeof(CRect));

			//TRACE(_T("name = %s | title = %s | window_rect = (%d, %d, %d, %d) | frame_rect = (%d, %d, %d, %d)\n"),
			//	name, title,
			//	window_rect.left, window_rect.top, window_rect.Width(), window_rect.Height(),
			//	frame_rect.left, frame_rect.top, frame_rect.Width(), frame_rect.Height());

			WINDOWINFO window_info = { 0, };
			window_info.cbSize = sizeof(WINDOWINFO);
			wnd->GetWindowInfo(&window_info);
			if (frame_rect == window_rect && wnd->IsZoomed())
				frame_rect.DeflateRect(window_info.cxWindowBorders, window_info.cyWindowBorders);

			CDC* pdc = wnd->GetDC();
			//TRACE_DC2(*pdc, frame_rect.Width(), frame_rect.Height(), CMyString(_T("%s_%s.jpeg"), name, title));

			EnterCriticalSection(&main->m_cs_enabled_wnd_list);
			main->m_enabled_wnd_list.push_back(WND_INFO(hWnd, name, title, frame_rect));
			LeaveCriticalSection(&main->m_cs_enabled_wnd_list);
		}
	}
	return TRUE;
}

BOOL CWndManager::GetAllWindowList(HWND hWnd, LPARAM lParam)
{
	CWndManager* main = reinterpret_cast<CWndManager*>(lParam);
	if (main)
	{
		if (hWnd)
		{
			CWnd* wnd = CWnd::FromHandle(hWnd);
			if (!wnd->IsWindowVisible())
				return TRUE;
			
			//WINDOWPLACEMENT window_placement = { 0, };
			//wnd->GetWindowPlacement(&window_placement);
			//if (window_placement.showCmd == SW_HIDE)
			//	return TRUE;

			//if (::GetWindowTextLength(hWnd) <= 0)
			//	return TRUE;

			//if (::GetParent(hWnd))
			//	return TRUE;

			TCHAR name[MAX_PATH] = { 0, };
			TCHAR title[MAX_PATH] = { 0, };

			::GetClassName(wnd->GetSafeHwnd(), name, MAX_PATH);
			::GetWindowText(wnd->GetSafeHwnd(), title, MAX_PATH);

			if (_tcscmp(title, _T("Program Manager")) == 0)
				return TRUE;

			EnterCriticalSection(&main->m_cs_exception_list);
			std::vector<CString> exception_list = main->m_exception_list;
			LeaveCriticalSection(&main->m_cs_exception_list);
			for (int i = 0; i < (int)exception_list.size(); i++)
			{
				if (exception_list[i].CompareNoCase(title) == 0)
					return TRUE;
			}

			CRect window_rect = { 0, };
			wnd->GetWindowRect(window_rect);

			CRect frame_rect;
			DwmGetWindowAttribute(wnd->GetSafeHwnd(), DWMWA_EXTENDED_FRAME_BOUNDS, frame_rect, sizeof(CRect));

			//int accessible_child_count = wnd->GetAccessibleChildCount();
			//int windowed_child_count = wnd->GetWindowedChildCount();
			//
			//TRACE(_T("name = %s | title = %s | accessible_child_count = %d | windowed_child_count = %d\n"), name, title, accessible_child_count, windowed_child_count);

			//TRACE(_T("name = %s | title = %s | window_rect = (%d, %d, %d, %d) | frame_rect = (%d, %d, %d, %d)\n"),
			//	name, title,
			//	window_rect.left, window_rect.top, window_rect.Width(), window_rect.Height(),
			//	frame_rect.left, frame_rect.top, frame_rect.Width(), frame_rect.Height());

			WINDOWINFO window_info = { 0, };
			window_info.cbSize = sizeof(WINDOWINFO);
			wnd->GetWindowInfo(&window_info);
			if (frame_rect == window_rect && wnd->IsZoomed())
				frame_rect.DeflateRect(window_info.cxWindowBorders, window_info.cyWindowBorders);

			CDC* pdc = wnd->GetDC();
			//TRACE_DC2(*pdc, frame_rect.Width(), frame_rect.Height(), CMyString(_T("%s_%s.jpeg"), name, title));

			EnterCriticalSection(&main->m_cs_all_wnd_list);
			main->m_all_wnd_list.push_back(WND_INFO(hWnd, name, title, frame_rect));
			LeaveCriticalSection(&main->m_cs_all_wnd_list);
		}
	}
	return TRUE;
}

CWndManager::CWndManager()
{
	InitializeCriticalSectionEx(&m_cs_enabled_wnd_list, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
	InitializeCriticalSectionEx(&m_cs_all_wnd_list, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
	InitializeCriticalSectionEx(&m_cs_exception_list, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
}

CWndManager::~CWndManager()
{
	DeleteCriticalSection(&m_cs_enabled_wnd_list);
	DeleteCriticalSection(&m_cs_all_wnd_list);
	DeleteCriticalSection(&m_cs_exception_list);
}

void CWndManager::CheckWindowStyle(HWND hWnd)
{
	WINDOWINFO window_info = { 0, };
	::GetWindowInfo(hWnd, &window_info);

	TCHAR name[MAX_PATH] = { 0, };
	TCHAR title[MAX_PATH] = { 0, };

	::GetClassName(hWnd, name, MAX_PATH);
	::GetWindowText(hWnd, title, MAX_PATH);

	CString trace_data;
	trace_data.Format(_T("name = %s | title = %s | "), name, title);
	if (window_info.dwStyle & WS_OVERLAPPED)
		trace_data += _T("'WS_OVERLAPPED' ");
	if (window_info.dwStyle & WS_POPUP)
		trace_data += _T("'WS_POPUP' ");
	if (window_info.dwStyle & WS_CHILD)
		trace_data += _T("'WS_CHILD' ");
	if (window_info.dwStyle & WS_MINIMIZE)
		trace_data += _T("'WS_MINIMIZE' ");
	if (window_info.dwStyle & WS_VISIBLE)
		trace_data += _T("'WS_VISIBLE' ");
	if (window_info.dwStyle & WS_DISABLED)
		trace_data += _T("'WS_DISABLED' ");
	if (window_info.dwStyle & WS_CLIPSIBLINGS)
		trace_data += _T("'WS_CLIPSIBLINGS' ");
	if (window_info.dwStyle & WS_CLIPCHILDREN)
		trace_data += _T("'WS_CLIPCHILDREN' ");
	if (window_info.dwStyle & WS_MAXIMIZE)
		trace_data += _T("'WS_MAXIMIZE' ");
	if (window_info.dwStyle & WS_CAPTION)
		trace_data += _T("'WS_CAPTION' ");
	if (window_info.dwStyle & WS_BORDER)
		trace_data += _T("'WS_BORDER' ");
	if (window_info.dwStyle & WS_DLGFRAME)
		trace_data += _T("'WS_DLGFRAME' ");
	if (window_info.dwStyle & WS_VSCROLL)
		trace_data += _T("'WS_VSCROLL' ");
	if (window_info.dwStyle & WS_HSCROLL)
		trace_data += _T("'WS_HSCROLL' ");
	if (window_info.dwStyle & WS_SYSMENU)
		trace_data += _T("'WS_SYSMENU' ");
	if (window_info.dwStyle & WS_THICKFRAME)
		trace_data += _T("'WS_THICKFRAME' ");
	if (window_info.dwStyle & WS_GROUP)
		trace_data += _T("'WS_GROUP' ");
	if (window_info.dwStyle & WS_TABSTOP)
		trace_data += _T("'WS_TABSTOP' ");
	if (window_info.dwStyle & WS_MINIMIZEBOX)
		trace_data += _T("'WS_MINIMIZEBOX' ");
	if (window_info.dwStyle & WS_MAXIMIZEBOX)
		trace_data += _T("'WS_MAXIMIZEBOX' ");

	if (window_info.dwExStyle & WS_EX_DLGMODALFRAME)
		trace_data += _T("'WS_EX_DLGMODALFRAME' ");
	if (window_info.dwExStyle & WS_EX_NOPARENTNOTIFY)
		trace_data += _T("'WS_EX_NOPARENTNOTIFY' ");
	if (window_info.dwExStyle & WS_EX_TOPMOST)
		trace_data += _T("'WS_EX_TOPMOST' ");
	if (window_info.dwExStyle & WS_EX_ACCEPTFILES)
		trace_data += _T("'WS_EX_ACCEPTFILES' ");
	if (window_info.dwExStyle & WS_EX_TRANSPARENT)
		trace_data += _T("'WS_EX_TRANSPARENT' ");
	if (window_info.dwExStyle & WS_EX_MDICHILD)
		trace_data += _T("'WS_EX_MDICHILD' ");
	if (window_info.dwExStyle & WS_EX_TOOLWINDOW)
		trace_data += _T("'WS_EX_TOOLWINDOW' ");
	if (window_info.dwExStyle & WS_EX_WINDOWEDGE)
		trace_data += _T("'WS_EX_WINDOWEDGE' ");
	if (window_info.dwExStyle & WS_EX_CLIENTEDGE)
		trace_data += _T("'WS_EX_CLIENTEDGE' ");
	if (window_info.dwExStyle & WS_EX_CONTEXTHELP)
		trace_data += _T("'WS_EX_CONTEXTHELP' ");

#if(WINVER >= 0x0400)
	if (window_info.dwExStyle & WS_EX_RIGHT)
		trace_data += _T("'WS_EX_RIGHT' ");
	if (window_info.dwExStyle & WS_EX_LEFT)
		trace_data += _T("'WS_EX_LEFT' ");
	if (window_info.dwExStyle & WS_EX_RTLREADING)
		trace_data += _T("'WS_EX_RTLREADING' ");
	if (window_info.dwExStyle & WS_EX_LTRREADING)
		trace_data += _T("'WS_EX_LTRREADING' ");
	if (window_info.dwExStyle & WS_EX_LEFTSCROLLBAR)
		trace_data += _T("'WS_EX_LEFTSCROLLBAR' ");
	if (window_info.dwExStyle & WS_EX_RIGHTSCROLLBAR)
		trace_data += _T("'WS_EX_RIGHTSCROLLBAR' ");
	if (window_info.dwExStyle & WS_EX_CONTROLPARENT)
		trace_data += _T("'WS_EX_CONTROLPARENT' ");
	if (window_info.dwExStyle & WS_EX_STATICEDGE)
		trace_data += _T("'WS_EX_STATICEDGE' ");
	if (window_info.dwExStyle & WS_EX_APPWINDOW)
		trace_data += _T("'WS_EX_APPWINDOW' ");
#endif

#if(_WIN32_WINNT >= 0x0500)
	if (window_info.dwExStyle & WS_EX_LAYERED)
		trace_data += _T("'WS_EX_LAYERED' ");
	if (window_info.dwExStyle & WS_EX_NOINHERITLAYOUT)
		trace_data += _T("'WS_EX_NOINHERITLAYOUT' ");
	if (window_info.dwExStyle & WS_EX_LAYOUTRTL)
		trace_data += _T("'WS_EX_LAYOUTRTL' ");
	if (window_info.dwExStyle & WS_EX_NOACTIVATE)
		trace_data += _T("'WS_EX_NOACTIVATE' ");
#endif

#if(WINVER >= 0x0602)
	if (window_info.dwExStyle & WS_EX_NOREDIRECTIONBITMAP)
		trace_data += _T("'WS_EX_NOREDIRECTIONBITMAP' ");
#endif

#if(_WIN32_WINNT >= 0x0501)
	if (window_info.dwExStyle & WS_EX_COMPOSITED)
		trace_data += _T("'WS_EX_COMPOSITED' ");
#endif
	trace_data += _T("\n");

	TRACE_STR(trace_data);
}

bool CWndManager::LoadEnabledWindows()
{
	EnterCriticalSection(&m_cs_enabled_wnd_list);
	m_enabled_wnd_list.clear();
	LeaveCriticalSection(&m_cs_enabled_wnd_list);

	if (!EnumWindows(GetEnabledWindowList, (LPARAM)this))
		return false;

	CalcZOrderOfEnabledWindows();

	return true;
}

bool CWndManager::LoadAllWindows()
{
	EnterCriticalSection(&m_cs_all_wnd_list);
	m_all_wnd_list.clear();
	LeaveCriticalSection(&m_cs_all_wnd_list);

	if (!EnumWindows(GetAllWindowList, (LPARAM)this))
		return false;

	CalcZOrderOfAllWindows();

	return true;
}

std::vector<WND_INFO> CWndManager::GetEnabledWndList()
{
	std::vector<WND_INFO> wnd_list;
	EnterCriticalSection(&m_cs_enabled_wnd_list);
	if (m_enabled_wnd_list.size() <= 0)
	{
		LeaveCriticalSection(&m_cs_enabled_wnd_list);
		LoadEnabledWindows();
	}
	else
		LeaveCriticalSection(&m_cs_enabled_wnd_list);
	EnterCriticalSection(&m_cs_enabled_wnd_list);
	wnd_list = m_enabled_wnd_list;
	LeaveCriticalSection(&m_cs_enabled_wnd_list);

	return wnd_list;
}

std::vector<WND_INFO> CWndManager::GetAllWndList()
{
	std::vector<WND_INFO> wnd_list;
	EnterCriticalSection(&m_cs_all_wnd_list);
	if (m_all_wnd_list.size() <= 0)
	{
		LeaveCriticalSection(&m_cs_all_wnd_list);
		LoadAllWindows();
	}
	else
		LeaveCriticalSection(&m_cs_all_wnd_list);
	EnterCriticalSection(&m_cs_all_wnd_list);
	wnd_list = m_all_wnd_list;
	LeaveCriticalSection(&m_cs_all_wnd_list);

	return wnd_list;
}

WND_INFO CWndManager::GetTopEnabledWndFromPoint(CPoint& point)
{
	EnterCriticalSection(&m_cs_enabled_wnd_list);
	std::vector<WND_INFO> wnd_list = m_enabled_wnd_list;
	LeaveCriticalSection(&m_cs_enabled_wnd_list);

	WND_INFO top_wnd;
	int size = (int)wnd_list.size();
	for (int i = 0; i < size; i++)
	{
		if (wnd_list[i].position.PtInRect(point))
		{
			if (top_wnd.z_order > wnd_list[i].z_order)
				top_wnd = wnd_list[i];
		}
	}

	return top_wnd;
}

WND_INFO CWndManager::GetTopAllWndFromFromPoint(CPoint & point)
{
	EnterCriticalSection(&m_cs_all_wnd_list);
	std::vector<WND_INFO> wnd_list = m_all_wnd_list;
	LeaveCriticalSection(&m_cs_all_wnd_list);

	WND_INFO top_wnd;
	int size = (int)wnd_list.size();
	for (int i = 0; i < size; i++)
	{
		if (wnd_list[i].position.PtInRect(point))
		{
			if (top_wnd.z_order > wnd_list[i].z_order)
				top_wnd = wnd_list[i];
		}
	}

	return top_wnd;
}

void CWndManager::AddExceptionList(CString exception)
{
	EnterCriticalSection(&m_cs_exception_list);
	m_exception_list.push_back(exception);
	LeaveCriticalSection(&m_cs_exception_list);
}

void CWndManager::CalcZOrderOfEnabledWindows()
{
	HWND hwnd = GetNextWindow(GetTopWindow(NULL), GW_HWNDNEXT);

	EnterCriticalSection(&m_cs_enabled_wnd_list);
	UINT z_order = 0;
	int size = (int)m_enabled_wnd_list.size();
	while (hwnd)
	{
		for (int i = 0; i < size; i++)
		{
			if (m_enabled_wnd_list[i].hwnd == hwnd)
				m_enabled_wnd_list[i].z_order = z_order;
		}
		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
		z_order++;
	}
	LeaveCriticalSection(&m_cs_enabled_wnd_list);
}

void CWndManager::CalcZOrderOfAllWindows()
{
	HWND hwnd = GetNextWindow(GetTopWindow(NULL), GW_HWNDNEXT);

	EnterCriticalSection(&m_cs_all_wnd_list);
	UINT z_order = 0;
	int size = (int)m_all_wnd_list.size();
	while (hwnd)
	{
		for (int i = 0; i < size; i++)
		{
			if (m_all_wnd_list[i].hwnd == hwnd)
				m_all_wnd_list[i].z_order = z_order;
		}
		hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
		z_order++;
	}
	LeaveCriticalSection(&m_cs_all_wnd_list);

	//for (int i = 0; i < size; i++)
	//	TRACE(_T("name = %s | title = %s | z_order = %d\n"), m_all_wnd_list[i].name, m_all_wnd_list[i].title, m_all_wnd_list[i].z_order);
}
