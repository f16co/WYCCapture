#pragma once
#include <Windows.h>
#include "Monitors.h"
#include "MultiMonitor.h"

class CGlobalFunction
{
public:
	static CGlobalFunction* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CGlobalFunction();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	bool CheckWindow(CWnd* wnd);
	bool CheckWindow(CWnd& wnd);
	bool CheckWindow(HWND wnd);
	HDC GetDesktopDC();
	void SafeDelete(void*& p);
	void SafeDeleteArr(void*& p);

	UINT64 PointsToUint64(CPoint pt1, CPoint pt2);
	void Uint64ToPoints(UINT64 value, CPoint* pt1, CPoint* pt2);

	UINT64 RectToUint64(CRect rect);
	CRect Uint64ToRect(UINT64 value);

	CSize GetTotalMonitorSize();

	void ModifyPointFromMonitorScope(int current_monitor_index, CPoint* point);

	int GetMonitorIndexFromPoint(CPoint point);
	int GetQuadrantIndexFromMonitorIndexAndPoint(int monitor_index, CPoint point);

	CRect GetMonitorRectFromMonitorIndex(int monitor_index);

	bool MonitorIndexIsValid(int monitor_index);
	bool QuadrantIndexIsValid(int quadrant_index);

	CRect GetScreenRect(CWnd* scope_wnd, CWnd* source_wnd);
	CRect GetScreenRect(CWnd* scope_wnd, CWnd& source_wnd);
	CRect GetScreenRect(CWnd& scope_wnd, CWnd* source_wnd);
	CRect GetScreenRect(CWnd& scope_wnd, CWnd& source_wnd);
	CRect GetScreenRect(HWND scope_hwnd, HWND source_hwnd);

	bool CheckVirtualKey(int virtual_key);
	bool CheckFileExist(CString file);

private:
	CGlobalFunction();
	CGlobalFunction(const CGlobalFunction& r) {}
	~CGlobalFunction();

	static void ReleaseSingletone()
	{
		if (m_singletone)
			delete m_singletone;
	}

	static CGlobalFunction* m_singletone;
};

#ifndef GF
#define GF() CGlobalFunction::GetSingletone()
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) CGlobalFunction::GetSingletone()->SafeDelete((void*&)p)
#endif

#ifndef SAFE_DELETE_ARR
#define SAFE_DELETE_ARR(p) CGlobalFunction::GetSingletone()->SafeDeleteArr((void*&)p)
#endif
