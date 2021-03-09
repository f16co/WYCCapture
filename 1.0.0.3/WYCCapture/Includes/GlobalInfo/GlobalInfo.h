#pragma once
#include <afxwin.h>

class CGlobalInfo
{
public:
	static CGlobalInfo* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CGlobalInfo();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}
	
	void SetShortcutTargetName(CString shortcut_target_name) { m_shortcut_target_name = shortcut_target_name; }
	CString GetShortcutTargetName() const { return m_shortcut_target_name; }

	void SetProgramName(CString program_name) { m_program_name = program_name; }
	CString GetProgramName() const { return m_program_name; }

	void SetProductName(CString product_name) { m_product_name = product_name; }
	CString GetProductName() const { return m_product_name; }

	CString GetVersion() const;

	CString GetShortcutXMLHeader() const { return _T("<ShortcutList>\r\n"); }
	CString GetShortcutXMLTail() const { return _T("</ShortcutList>\r\n"); }
	CString GetDefaultShortcut();
	CString MakeShortcutInfo(CString receiver_name, CString command_name, int command_id, CString key);
	CString GetShortcutPath();

	CString GetGlobalSettingXMLHeader() const { return _T("<GlobalSetting>\r\n"); }
	CString GetGlobalSettingXMLTail() const { return _T("</GlobalSetting>\r\n"); }
	CString GetDefaultGlobalSetting();
	CString GetGlobalSettingPath();

private:
	CGlobalInfo();
	CGlobalInfo(const CGlobalInfo& r) {}
	~CGlobalInfo();

	static void ReleaseSingletone()
	{
		if (m_singletone)
			delete m_singletone;
	}

	static CGlobalInfo* m_singletone;

	CString m_shortcut_target_name;
	CString m_program_name;
	CString m_product_name;
};

#ifndef GI
#define GI() CGlobalInfo::GetSingletone()
#endif