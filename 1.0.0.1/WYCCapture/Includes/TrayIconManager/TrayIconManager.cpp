#include "stdafx.h"
#include "TrayIconManager.h"

CTrayIconManager* CTrayIconManager::m_singletone = NULL;

CTrayIconManager::CTrayIconManager()
{
	m_hwnd = NULL;
	m_hmenu = NULL;
}

CTrayIconManager::~CTrayIconManager()
{
}

bool CTrayIconManager::AddTrayIcon(HWND hwnd, CString tip, HICON hicon, HMENU hmenu /*= NULL*/)
{
	if (!hwnd || !hicon)
		return false;

	m_hwnd = hwnd;
	if (hmenu)
		m_hmenu = hmenu;

	NOTIFYICONDATA notify_icon_data = { 0, };
	notify_icon_data.cbSize = sizeof(NOTIFYICONDATA);
	notify_icon_data.hWnd = hwnd;
	notify_icon_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notify_icon_data.uCallbackMessage = TRAY_ICON_MESSAGE_RBUTTONDOWN;
	_tcscpy(notify_icon_data.szTip, tip);
	notify_icon_data.uID = 0;
	notify_icon_data.hIcon = hicon;

	if (!Shell_NotifyIcon(NIM_ADD, &notify_icon_data))
		return false;

	return true;
}

bool CTrayIconManager::DeleteTreyIcon()
{
	NOTIFYICONDATA notify_icon_data = { 0, };
	notify_icon_data.cbSize = sizeof(NOTIFYICONDATA);
	notify_icon_data.hWnd = m_hwnd;
	notify_icon_data.uFlags = NULL;

	if (!Shell_NotifyIcon(NIM_DELETE, &notify_icon_data))
		return false;

	return true;
}

void CTrayIconManager::ShowPopupMenu()
{
	CPoint pos;
	::GetCursorPos(&pos);
	if (m_hwnd)
	{
		::SetForegroundWindow(m_hwnd);
		if (m_hmenu)
			::TrackPopupMenu(m_hmenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, 0, m_hwnd, NULL);
	}
}
