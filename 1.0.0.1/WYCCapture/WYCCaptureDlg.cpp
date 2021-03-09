﻿#include "stdafx.h"
#include "WYCCapture.h"
#include "WYCCaptureDlg.h"
#include "afxdialogex.h"
#include "ShortcutListParser.h"
#include "MonitorDC.h"
#include "Monitors.h"
#include "MultiMonitor.h"
#include "TrayIconManager.h"
#include "CapturedRegionManager.h"
#include "WndManager.h"
#include "DlgProductInfo.h"
#include "DlgSetting.h"

BEGIN_MESSAGE_MAP(CWYCCaptureDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SHORTCUT_GENERATED, OnShortcutGenerated)
	ON_MESSAGE(UM_SHORTCUT_CHANGED, OnShortcutChanged)
	ON_MESSAGE(CTrayIconManager::TRAY_ICON_MESSAGE_RBUTTONDOWN, OnTrayIcon)
	ON_COMMAND(ID_TRAY_ICON_REGION_CAPTURE_MODE, OnTrayIconRegionCapture)
	ON_COMMAND(ID_TRAY_ICON_WINDOW_CAPTURE_MODE, OnTrayIconWindowCapture)
	ON_COMMAND(ID_TRAY_ICON_PRODUCT_INFO, OnTrayIconProductInfo)
	ON_COMMAND(ID_TRAY_ICON_SETTING, OnTrayIconSetting)
	ON_COMMAND(ID_TRAY_ICON_EXIT, OnTrayIconExit)
END_MESSAGE_MAP()

CWYCCaptureDlg::CWYCCaptureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD, pParent)
	, m_context_menu(NULL)
{
	InitializeCriticalSectionEx(&m_cs_shortcut_func, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetShortcutTargetName(_T("TrayIcon"));

	m_last_selected_shortcut = 0;
}

CWYCCaptureDlg::~CWYCCaptureDlg()
{
	DeleteCriticalSection(&m_cs_shortcut_func);
}

void CWYCCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CWYCCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	if (!CTrayIconManager::GetSingletone()->AddTrayIcon(
		GetSafeHwnd(), 
		GI()->GetProductName(), 
		m_hIcon))
		return FALSE;

	RegisterUserMessages();
	MakePopupMenu();
	InitShortcuts();
	InitShortcutTarget();

	CShortcutManager::GetSingletone()->Resume();

	m_captured_region_manager.Create();

	m_region_capture_thread.Start();
	m_window_capture_thread.Start();

	return TRUE;
}

BOOL CWYCCaptureDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) ||
		(pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4))
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CWYCCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CWYCCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CWYCCaptureDlg::OnEraseBkgnd(CDC* pDC)
{
	ShowWindow(SW_HIDE);
	return CDialogEx::OnEraseBkgnd(pDC);
}

void CWYCCaptureDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	DeleteShortcuts();

	if (!CTrayIconManager::GetSingletone()->DeleteTreyIcon())
	{
		TRACE_STR(_T("Failed to delete tray icon!"));
		return;
	}

	WM_UNREGISTERALL();
}

LRESULT CWYCCaptureDlg::OnShortcutGenerated(WPARAM wParam, LPARAM lParam)
{
	DWORD shortcut_code = (DWORD)wParam;
	int shortcut_id = (DWORD)lParam;
	SHORTCUT_DATA shortcut_data = CShortcutManager::GetSingletone()->ParseShortcutCode(shortcut_code);
	bool ctrl = shortcut_data.ctrl;
	bool alt = shortcut_data.alt;
	bool shift = shortcut_data.shift;
	BYTE key = shortcut_data.key;
	
	if (shortcut_id >= 0)
	{
		SHORTCUT_FUNC shortcut_func = NULL;
		EnterCriticalSection(&m_cs_shortcut_func);
		std::map<int, SHORTCUT_FUNC>::iterator pos = m_map_shortcut_func.find(shortcut_id);
		if (pos != m_map_shortcut_func.end())
			shortcut_func = pos->second;
		LeaveCriticalSection(&m_cs_shortcut_func);

		if (shortcut_func)
			shortcut_func(GetSafeHwnd());
	}

	return 0;
}

LRESULT CWYCCaptureDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_RBUTTONDOWN)
		CTrayIconManager::GetSingletone()->ShowPopupMenu();

	return 0;
}

LRESULT CWYCCaptureDlg::OnShortcutChanged(WPARAM wParam, LPARAM lParam)
{
	MakePopupMenu();
	InitShortcuts();
	InitShortcutTarget();

	return 0;
}

void CWYCCaptureDlg::InitShortcutTarget()
{
	EnterCriticalSection(&m_cs_shortcut_func);
	m_map_shortcut_func[0] = OnShortcutRegionCapture;
	m_map_shortcut_func[1] = OnShortcutWindowCapture;
	m_map_shortcut_func[2] = OnShortcutSetting;
	m_map_shortcut_func[3] = OnShortcutProductInfo;
	m_map_shortcut_func[4] = OnShortcutExit;
	m_map_shortcut_func[5] = OnShortcutESC;
	LeaveCriticalSection(&m_cs_shortcut_func);
}

void CWYCCaptureDlg::OnTrayIconRegionCapture()
{
	if (!m_window_capture_thread.IsPause())
	{
		WM_SEND_MESSAGE(UM_WINDOW_CAPTURE_PAUSE);
		Sleep(800);
	}
	m_region_capture_thread.Resume();
	::SetCursor(LoadCursor(NULL, IDC_CROSS));

	//if (!m_window_capture_thread.IsRunning())
	//{
	//	WM_SEND_MESSAGE(UM_WINDOW_CAPTURE_STOP);
	//	Sleep(800);
	//}
	//m_region_capture_thread.Start();
	//::SetCursor(LoadCursor(NULL, IDC_CROSS));
}

void CWYCCaptureDlg::OnTrayIconWindowCapture()
{
	if (!m_region_capture_thread.IsPause())
	{
		WM_SEND_MESSAGE(UM_REGION_CAPTURE_PAUSE);
		Sleep(800);
	}

	if (!CWndManager::GetSingletone()->LoadAllWindows())
		return;

	m_window_capture_thread.Resume();
	::SetCursor(LoadCursor(NULL, IDC_CROSS));

	//if (!m_region_capture_thread.IsRunning())
	//{
	//	WM_SEND_MESSAGE(UM_REGION_CAPTURE_STOP);
	//	Sleep(800);
	//}

	//if (!CWndManager::GetSingletone()->LoadAllWindows())
	//	return;

	//m_window_capture_thread.Start();
	//::SetCursor(LoadCursor(NULL, IDC_CROSS));
}

void CWYCCaptureDlg::OnTrayIconProductInfo()
{
	CShortcutManager::GetSingletone()->Pause();

	CDlgProductInfo dlg_product_info(this);
	dlg_product_info.DoModal();

	CShortcutManager::GetSingletone()->Resume();
}

void CWYCCaptureDlg::OnTrayIconSetting()
{
	CShortcutManager::GetSingletone()->Pause();

	CDlgSetting dlg_setting(this);
	dlg_setting.SetLastSelectedItem(m_last_selected_shortcut);
	if (dlg_setting.DoModal() == IDOK)
		m_last_selected_shortcut = dlg_setting.GetLastSelectedItem();

	CShortcutManager::GetSingletone()->Resume();
}

void CWYCCaptureDlg::OnTrayIconExit()
{
	Close();
}

void CWYCCaptureDlg::InitShortcuts()
{
	DeleteShortcuts();

	std::vector<DWORD> shortcut_code_list;
	std::vector<UINT64> shortcut_id_list;
	CShortcutListParser::GetSingletone()->GetShortcutList(GetShortcutTargetName(), &shortcut_code_list, &shortcut_id_list);

	for (int i = 0; i < (int)shortcut_id_list.size(); i++)
		CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(shortcut_id_list[i], GetSafeHwnd()), shortcut_code_list[i]);

	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(shortcut_id_list[shortcut_id_list.size() - 1] + 1, GetSafeHwnd()), false, false, false, VK_ESCAPE);
}

void CWYCCaptureDlg::DeleteShortcuts()
{
	std::vector<DWORD> shortcut_code_list;
	std::vector<UINT64> shortcut_id_list;
	CShortcutListParser::GetSingletone()->GetShortcutList(GetShortcutTargetName(), NULL, &shortcut_id_list);

	for (int i = 0; i < (int)shortcut_id_list.size(); i++)
		CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(shortcut_id_list[i], GetSafeHwnd()));

	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(shortcut_id_list[shortcut_id_list.size() - 1] + 1, GetSafeHwnd()), false, false, false, VK_ESCAPE);
}

void CWYCCaptureDlg::RegisterUserMessages()
{
	WM_REGISTER(UM_SHORTCUT_CHANGED);
}

void CWYCCaptureDlg::MakePopupMenu()
{
	if (m_context_menu)
	{
		::DestroyMenu(m_context_menu);
		m_context_menu = NULL;
	}
	m_context_menu = ::CreatePopupMenu();

	std::vector<DWORD> shortcut_code_list;
	std::vector<CString> shorcut_command_name_list;
	CShortcutListParser::GetSingletone()->GetShortcutList(GetShortcutTargetName(), &shortcut_code_list, NULL, &shorcut_command_name_list);
	for (int i = 0; i < (int)shorcut_command_name_list.size(); i++)
	{
		if (shorcut_command_name_list[i].Find(_T("Region Capture")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_TRAY_ICON_REGION_CAPTURE_MODE, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
		else if (shorcut_command_name_list[i].Find(_T("Window Capture")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_TRAY_ICON_WINDOW_CAPTURE_MODE, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
		else if (shorcut_command_name_list[i].Find(_T("Setting")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_TRAY_ICON_SETTING, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
		else if (shorcut_command_name_list[i].Find(_T("Product Info")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_TRAY_ICON_PRODUCT_INFO, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
		else if (shorcut_command_name_list[i].Find(_T("Exit")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_TRAY_ICON_EXIT, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
	}

	CTrayIconManager::GetSingletone()->SetMenu(m_context_menu);
}

void CWYCCaptureDlg::Pause()
{
	UINT message = 0;
	if (!m_region_capture_thread.IsPause())
		message = UM_REGION_CAPTURE_PAUSE;
	else if (!m_window_capture_thread.IsPause())
		message = UM_WINDOW_CAPTURE_PAUSE;

	//if (!m_region_capture_thread.IsRunning())
	//	message = UM_REGION_CAPTURE_STOP;
	//else if (!m_window_capture_thread.IsRunning())
	//	message = UM_WINDOW_CAPTURE_STOP;

	WM_SEND_MESSAGE(message);
}

void CWYCCaptureDlg::OnShortcutRegionCapture(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CWYCCaptureDlg* main = reinterpret_cast<CWYCCaptureDlg*>(CWnd::FromHandle(hwnd));
	main->OnTrayIconRegionCapture();
}

void CWYCCaptureDlg::OnShortcutWindowCapture(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CWYCCaptureDlg* main = reinterpret_cast<CWYCCaptureDlg*>(CWnd::FromHandle(hwnd));
	main->OnTrayIconWindowCapture();
}

void CWYCCaptureDlg::OnShortcutSetting(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CWYCCaptureDlg* main = reinterpret_cast<CWYCCaptureDlg*>(CWnd::FromHandle(hwnd));
	main->OnTrayIconSetting();
}

void CWYCCaptureDlg::OnShortcutProductInfo(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CWYCCaptureDlg* main = reinterpret_cast<CWYCCaptureDlg*>(CWnd::FromHandle(hwnd));
	main->OnTrayIconProductInfo();
}

void CWYCCaptureDlg::OnShortcutExit(HWND hwnd)
{
	if (!hwnd)
		return;

	CWYCCaptureDlg* main = reinterpret_cast<CWYCCaptureDlg*>(CWnd::FromHandle(hwnd));
	main->OnTrayIconExit();
}

void CWYCCaptureDlg::OnShortcutESC(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CWYCCaptureDlg* main = reinterpret_cast<CWYCCaptureDlg*>(CWnd::FromHandle(hwnd));
	main->Pause();
}
