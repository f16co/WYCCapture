#pragma once
#include <afxwin.h>

class CTrayIconManager
{
public:
	enum TRAY_ICON_MESSAGE { TRAY_ICON_MESSAGE_RBUTTONDOWN = WM_USER };

	static CTrayIconManager* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CTrayIconManager();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	bool AddTrayIcon(HWND hwnd, CString tip, HICON hicon, HMENU hmenu = NULL);
	bool DeleteTreyIcon();
	void ShowPopupMenu();
	void SetMenu(HMENU hmenu) { m_hmenu = hmenu; }

private:
	CTrayIconManager();
	CTrayIconManager(const CTrayIconManager& r) {}
	~CTrayIconManager();

	static void ReleaseSingletone()
	{
		if (m_singletone)
		{
			delete m_singletone;
			m_singletone = NULL;
		}
	}

	static CTrayIconManager* m_singletone;

	HWND m_hwnd;
	HMENU m_hmenu;
};

