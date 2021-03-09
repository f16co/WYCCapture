#include "stdafx.h"
#include "GlobalFunction.h"
#include "Monitors.h"
#include "MultiMonitor.h"
#include <Shlwapi.h>

#pragma comment (lib, "version.lib")
#pragma comment (lib, "shlwapi.lib")

CGlobalFunction* CGlobalFunction::m_singletone = NULL;

CGlobalFunction::CGlobalFunction()
{
}

CGlobalFunction::~CGlobalFunction()
{
}

CString CGlobalFunction::GetVersion()
{
	TCHAR module_name[512];
	GetModuleFileName(NULL, module_name, sizeof(module_name));

	DWORD	dwValue = 0;
	DWORD	dwSize = 0;
	BYTE	*pBuffer = NULL;
	DWORD	dwVerLen = 0;
	unsigned __int64	ui64Version = 0;
	VS_FIXEDFILEINFO *pVer = NULL;

	dwSize = GetFileVersionInfoSize(module_name, &dwValue);
	if (!dwSize)
		return 0;

	pBuffer = new BYTE[dwSize];
	if (!pBuffer)
		return 0;

	if (!GetFileVersionInfo(module_name, 0, dwSize, pBuffer))
	{
		delete[] pBuffer;
		return 0;
	}
	if (!VerQueryValue(pBuffer, _T("\\"), (void**)&pVer, (UINT*)&dwVerLen))
	{
		delete[] pBuffer;
		return 0;
	}
	if (dwVerLen != sizeof(VS_FIXEDFILEINFO))
	{
		delete[] pBuffer;
		return 0;
	}

	ui64Version = (((unsigned __int64)pVer->dwFileVersionMS) << 32) | ((unsigned __int64)pVer->dwFileVersionLS);
	delete[] pBuffer;

	unsigned __int64 ver = ui64Version;
	CMyString str(_T("%u.%u.%u.%u"),
		(WORD)((ver >> 48) & 0xFFFF),
		(WORD)((ver >> 32) & 0xFFFF),
		(WORD)((ver >> 16) & 0xFFFF),
		(WORD)(ver & 0xFFFF));

	return str;
}

bool CGlobalFunction::CheckWindow(CWnd* wnd)
{
	return (wnd && CheckWindow(wnd->GetSafeHwnd()));
}

bool CGlobalFunction::CheckWindow(CWnd& wnd)
{
	return CheckWindow(wnd.GetSafeHwnd());
}

bool CGlobalFunction::CheckWindow(HWND wnd)
{
	return (wnd && ::IsWindow(wnd));
}

HDC CGlobalFunction::GetDesktopDC()
{
	HWND hwnd_desktop = ::GetDesktopWindow();
	HDC hdc_desktop = ::GetDC(hwnd_desktop);
	return hdc_desktop;
}

void CGlobalFunction::SafeDelete(void*& p)
{
	if (p)
	{
		delete p;
		p = NULL;
	}
}

void CGlobalFunction::SafeDeleteArr(void*& p)
{
	if (p)
	{
		delete[] p;
		p = NULL;
	}
}

UINT64 CGlobalFunction::PointsToUint64(CPoint pt1, CPoint pt2)
{
	UINT64 value = 0;
	UINT64 x1 = (UINT64)pt1.x;
	UINT64 y1 = (UINT64)pt1.y;
	UINT64 x2 = (UINT64)pt2.x;
	UINT64 y2 = (UINT64)pt2.y;

	x1 = x1 << 48;
	y1 = y1 << 32;
	x2 = x2 << 16;
	y2 = y2 << 0;

	value = x1 | y1 | x2 | y2;

	return value;
}

void CGlobalFunction::Uint64ToPoints(UINT64 value, CPoint* pt1, CPoint* pt2)
{
	UINT64 x1 = value & 0xFFFF000000000000;
	UINT64 y1 = value & 0x0000FFFF00000000;
	UINT64 x2 = value & 0x00000000FFFF0000;
	UINT64 y2 = value & 0x000000000000FFFF;

	x1 = x1 >> 48;
	y1 = y1 >> 32;
	x2 = x2 >> 16;
	y2 = y2 >> 0;

	pt1->x = (int)x1;
	pt1->y = (int)y1;
	pt2->x = (int)x2;
	pt2->y = (int)y2;
}

UINT64 CGlobalFunction::RectToUint64(CRect rect)
{
	UINT64 value = 0;
	UINT64 x1 = (UINT64)rect.left;
	UINT64 y1 = (UINT64)rect.top;
	UINT64 x2 = (UINT64)rect.right;
	UINT64 y2 = (UINT64)rect.bottom;

	x1 = x1 << 48;
	y1 = y1 << 32;
	x2 = x2 << 16;
	y2 = y2 << 0;

	value = x1 | y1 | x2 | y2;

	return value;
}

CRect CGlobalFunction::Uint64ToRect(UINT64 value)
{
	UINT64 left = value & 0xFFFF000000000000;
	UINT64 top = value & 0x0000FFFF00000000;
	UINT64 right = value & 0x00000000FFFF0000;
	UINT64 bottom = value & 0x000000000000FFFF;

	left = left >> 48;
	top = top >> 32;
	right = right >> 16;
	bottom = bottom >> 0;

	CRect rect((int)left, (int)top, (int)right, (int)bottom);
	return rect;
}

CSize CGlobalFunction::GetTotalMonitorSize()
{
	int total_monitor_width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int total_monitor_height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	CSize total_monitor_size(total_monitor_width, total_monitor_height);

	return total_monitor_size;
}

void CGlobalFunction::ModifyPointFromMonitorScope(int current_monitor_index, CPoint* point)
{
	CMonitors monitors;
	if (!MonitorIndexIsValid(current_monitor_index))
		return;

	CRect monitor_rect = GetMonitorRectFromMonitorIndex(current_monitor_index);

	point->x -= monitor_rect.left;
	point->y -= monitor_rect.top;
}

int CGlobalFunction::GetMonitorIndexFromPoint(CPoint point)
{
	int monitor_index = -1;
	CMonitors monitors;
	int monitor_count = monitors.GetCount();
	for (int i = 0; i < monitor_count; i++)
	{
		CMonitor monitor = monitors.GetMonitor(i);
		CRect monitor_rect;
		monitor.GetMonitorRect(monitor_rect);
		if (monitor_rect.PtInRect(point))
		{
			monitor_index = i;
			break;
		}
	}
	return monitor_index;
}

int CGlobalFunction::GetQuadrantIndexFromMonitorIndexAndPoint(int monitor_index, CPoint point)
{
	if (!MonitorIndexIsValid(monitor_index))
		return -1;

	CRect monitor_rect = GetMonitorRectFromMonitorIndex(monitor_index);

	int quadrand_index = -1; // 1사분면: 0, 2사분면: 1, 3사분면: 2, 4사분면: 3

	CRect quadrant_rect[4];
	
	quadrant_rect[0].left = monitor_rect.left + monitor_rect.Width() / 2;
	quadrant_rect[0].top = monitor_rect.top;
	quadrant_rect[0].right = monitor_rect.right;
	quadrant_rect[0].bottom = monitor_rect.top + monitor_rect.Height() / 2;

	quadrant_rect[1].left = monitor_rect.left;
	quadrant_rect[1].top = monitor_rect.top;
	quadrant_rect[1].right = monitor_rect.left + monitor_rect.Width() / 2;
	quadrant_rect[1].bottom = monitor_rect.top + monitor_rect.Height() / 2;

	quadrant_rect[2].left = monitor_rect.left;
	quadrant_rect[2].top = monitor_rect.top + monitor_rect.Height() / 2;
	quadrant_rect[2].right = monitor_rect.left + monitor_rect.Width() / 2;
	quadrant_rect[2].bottom = monitor_rect.bottom;

	quadrant_rect[3].left = monitor_rect.left + monitor_rect.Width() / 2;
	quadrant_rect[3].top = monitor_rect.top + monitor_rect.Height() / 2;
	quadrant_rect[3].right = monitor_rect.right;
	quadrant_rect[3].bottom = monitor_rect.bottom;

	for (int i = 0; i < 4; i++)
	{
		if (quadrant_rect[i].PtInRect(point))
		{
			quadrand_index = i;
			break;
		}
	}

	return quadrand_index;
}

CRect CGlobalFunction::GetMonitorRectFromMonitorIndex(int monitor_index)
{
	CRect monitor_rect;
	if (MonitorIndexIsValid(monitor_index))
	{
		CMonitors monitors;
		CMonitor monitor = monitors.GetMonitor(monitor_index);
		monitor.GetMonitorRect(monitor_rect);
	}

	return monitor_rect;
}

bool CGlobalFunction::MonitorIndexIsValid(int monitor_index)
{
	if (monitor_index < 0)
		return false;
	if (monitor_index >= CMonitors::GetMonitorCount())
		return false;
	return true;
}

bool CGlobalFunction::QuadrantIndexIsValid(int quadrant_index)
{
	if (quadrant_index < 0)
		return false;
	if (quadrant_index >= 4)
		return false;
	return true;
}

CRect CGlobalFunction::GetScreenRect(CWnd* scope_wnd, CWnd* source_wnd)
{
	CRect source_screen_rect(0, 0, 0, 0);
	if (scope_wnd && source_wnd)
		source_screen_rect = GetScreenRect(scope_wnd->GetSafeHwnd(), source_wnd->GetSafeHwnd());

	return source_screen_rect;
}

CRect CGlobalFunction::GetScreenRect(CWnd* scope_wnd, CWnd& source_wnd)
{
	CRect source_screen_rect(0, 0, 0, 0);
	if (scope_wnd)
		source_screen_rect = GetScreenRect(scope_wnd->GetSafeHwnd(), source_wnd.GetSafeHwnd());

	return source_screen_rect;
}

CRect CGlobalFunction::GetScreenRect(CWnd& scope_wnd, CWnd* source_wnd)
{
	CRect source_screen_rect(0, 0, 0, 0);
	if (source_wnd)
		source_screen_rect = GetScreenRect(scope_wnd.GetSafeHwnd(), source_wnd->GetSafeHwnd());

	return source_screen_rect;
}

CRect CGlobalFunction::GetScreenRect(CWnd& scope_wnd, CWnd& source_wnd)
{
	return GetScreenRect(scope_wnd.GetSafeHwnd(), source_wnd.GetSafeHwnd());
}

CRect CGlobalFunction::GetScreenRect(HWND scope_hwnd, HWND source_hwnd)
{
	CRect source_screen_rect(0, 0, 0, 0);
	if (CheckWindow(scope_hwnd) && CheckWindow(source_hwnd))
	{
		CWnd* scope_wnd = CWnd::FromHandle(scope_hwnd);
		CWnd* source_wnd = CWnd::FromHandle(source_hwnd);

		CRect scope_rect;
		CRect source_rect;
		
		scope_wnd->GetClientRect(scope_rect);
		scope_wnd->ClientToScreen(scope_rect);
		source_wnd->GetClientRect(source_rect);
		source_wnd->ClientToScreen(source_rect);

		source_screen_rect.left = source_rect.left - scope_rect.left;
		source_screen_rect.top = source_rect.top - scope_rect.top;
		source_screen_rect.right = source_rect.right - scope_rect.left;
		source_screen_rect.bottom = source_rect.bottom - scope_rect.top;
	}

	return source_screen_rect;
}

bool CGlobalFunction::CheckVirtualKey(int virtual_key)
{
	return (GetAsyncKeyState(virtual_key) < 0);
}

bool CGlobalFunction::CheckFileExist(CString file)
{
	return PathFileExists(file) ? true : false;
}