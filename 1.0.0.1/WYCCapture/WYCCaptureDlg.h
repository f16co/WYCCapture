#pragma once

#include "RegionCaptureThread.h"
#include "WindowCaptureThread.h"
#include "CapturedRegionManager.h"
#include "ShortcutTarget.h"
#include "WMTarget.h"

class CWYCCaptureDlg : public CDialogEx, CShortcutTarget, CWMTarget
{
public:
	CWYCCaptureDlg(CWnd* pParent = nullptr);
	virtual ~CWYCCaptureDlg();

protected:
	enum { IDD = IDD_WYCCAPTURE_DIALOG };

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnShortcutGenerated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShortcutChanged(WPARAM wParam, LPARAM lParam);

	HICON m_hIcon;

	virtual void InitShortcutTarget();
	virtual void InitShortcuts();
	virtual void DeleteShortcuts();
	virtual void RegisterUserMessages();

	void OnTrayIconRegionCapture();
	void OnTrayIconWindowCapture();
	void OnTrayIconProductInfo();
	void OnTrayIconSetting();
	void OnTrayIconExit();

private:
	static void OnShortcutRegionCapture(HWND hwnd);
	static void OnShortcutWindowCapture(HWND hwnd);
	static void OnShortcutSetting(HWND hwnd);
	static void OnShortcutProductInfo(HWND hwnd);
	static void OnShortcutExit(HWND hwnd);
	static void OnShortcutESC(HWND hwnd); // ShortcutList.xml에는 따로 등록하지 않고 사용

	void MakePopupMenu();
	void Pause();
	void Close() { AfxGetMainWnd()->PostMessage(WM_CLOSE); }

	CRegionCaptureThread m_region_capture_thread;
	CWindowCaptureThread m_window_capture_thread;

	CCapturedRegionManager m_captured_region_manager;

	HMENU m_context_menu;

	int m_last_selected_shortcut;
};
