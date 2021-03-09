#pragma once
#include <afxwin.h>
#include "WMTarget.h"
#include "WindowCaptureDetailRegion.h"
#include "WndManager.h"
#include "SafeQueue.h"
#include "ShortcutTarget.h"

class CWindowCaptureDrawRegion : public CWnd, CShortcutTarget, CWMTarget
{
	DECLARE_DYNAMIC(CWindowCaptureDrawRegion)

public:
	CWindowCaptureDrawRegion();
	virtual ~CWindowCaptureDrawRegion();

	bool Create();
	void OnDraw(CPoint current_pos);

	void Show();
	void Hide();

	void Pause() { m_is_pause = true; }
	void Resume() { m_is_pause = false; }

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnShortcutGenerated(WPARAM wParam, LPARAM lParam);

	virtual void InitShortcutTarget();
	virtual void InitShortcuts();
	virtual void DeleteShortcuts();
	virtual void RegisterUserMessages();

private:
	static void OnShortcutLeft(HWND hwnd);
	static void OnShortcutUp(HWND hwnd);
	static void OnShortcutRight(HWND hwnd);
	static void OnShortcutDown(HWND hwnd);

	void OnLeft();
	void OnRight();
	void OnUp();
	void OnDown();

	void DrawCrossLine(CDC& mem_dc, CPoint pos);
	void DrawDesktopBackground(CDC& mem_dc, CRect& client_rect);
	void DrawSelectedRegion(CDC& mem_dc);
	bool DrawFirstTime(CDC& this_dc, CDC& monitor_dc, CRect& client_rect, CRect& monitor_rect);
	void DrawToThis(CDC& this_dc, CDC& mem_dc, CRect& client_rect);
	void DrawBackToMonitorDC(CDC& monitor_dc, CRect& monitor_rect);

	CPoint m_current_pos;
	WND_INFO m_captured_window_info;

	COLORREF m_selected_window_color;
	int m_selected_window_color_increments;

	bool m_is_shown;
	bool m_is_first_time;
	bool m_is_cliecked;
	bool m_is_pause;

	HBITMAP m_temp_memory_bitmap;
	HDC m_temp_memory_dc;

	//CWindowCaptureDetailRegion m_window_capture_detail_region;

	CSafeQueue m_signaling_data_queue;
};