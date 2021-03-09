#include "stdafx.h"
#include "GlobalInfo.h"
#include "PathEnvironments.h"

CGlobalInfo* CGlobalInfo::m_singletone = NULL;

CGlobalInfo::CGlobalInfo()
	: m_capture_mode(CAPTURE_MODE_REGION)
{
}

CGlobalInfo::~CGlobalInfo()
{
}

CString CGlobalInfo::GetDefaultShortcut()
{
	std::vector<CString> shortcut_list;
	shortcut_list.push_back(MakeShortcutInfo(_T("TrayIcon"), _T("Capture(&C)..."), 0, _T("PrintScreen")));
	shortcut_list.push_back(MakeShortcutInfo(_T("TrayIcon"), _T("Change Capture Mode(&H)..."), 1, _T("Tab")));
	shortcut_list.push_back(MakeShortcutInfo(_T("TrayIcon"), _T("Setting(&S)..."), 2, _T("Ctrl+Alt+Shift+F7")));
	shortcut_list.push_back(MakeShortcutInfo(_T("TrayIcon"), _T("Product Info(&I)..."), 3, _T("Ctrl+Alt+Shift+F8")));
	shortcut_list.push_back(MakeShortcutInfo(_T("TrayIcon"), _T("Exit(&E)..."), 4, _T("Ctrl+Alt+Shift+F4")));
	shortcut_list.push_back(MakeShortcutInfo(_T("CapturedRegion"), _T("Save the original image(&S)..."), 0, _T("Ctrl+S")));
	shortcut_list.push_back(MakeShortcutInfo(_T("CapturedRegion"), _T("Save the overlay image(&Y)..."), 1, _T("Ctrl+Shift+S")));
	shortcut_list.push_back(MakeShortcutInfo(_T("CapturedRegion"), _T("Erase the drawings(&E)..."), 2, _T("Ctrl+E")));
	shortcut_list.push_back(MakeShortcutInfo(_T("CapturedRegion"), _T("Reset size(&R)..."), 3, _T("Ctrl+R")));
	shortcut_list.push_back(MakeShortcutInfo(_T("CapturedRegion"), _T("Copy the original image to clipboard(&C)..."), 4, _T("Ctrl+C")));
	shortcut_list.push_back(MakeShortcutInfo(_T("CapturedRegion"), _T("Copy the overlay image to clipboard(&D)..."), 5, _T("Ctrl+Shift+C")));

	CString default_shortcut = GetShortcutXMLHeader();
	for (UINT i = 0; i < shortcut_list.size(); i++)
		default_shortcut += shortcut_list[i];
	default_shortcut += GetShortcutXMLTail();

	return default_shortcut;
}

CString CGlobalInfo::MakeShortcutInfo(CString receiver_name, CString command_name, int command_id, CString key)
{
	LPTSTR shortcut_format =
		_T("	<ShortcutInfo>\r\n") // 1
		_T("		<ReceiverName>%s</ReceiverName>\r\n")
		_T("		<CommandName>%s</CommandName>\r\n")
		_T("		<CommandID>%d</CommandID>\r\n")
		_T("		<Key>%s</Key>\r\n")
		_T("	</ShortcutInfo>\r\n");

	CString shortcut_info;
	shortcut_info.Format(shortcut_format, receiver_name, command_name, command_id, key);

	return shortcut_info;
}

CString CGlobalInfo::GetShortcutPath()
{
	CString shortcutlist_xml = CPathEnvironments::GetSingletone()->GetResourcePath();
	shortcutlist_xml += _T("\\ShortcutList.xml");
	return shortcutlist_xml;
}

CString CGlobalInfo::GetDefaultGlobalSetting()
{
	std::vector<CString> global_setting_info_list;
	global_setting_info_list.push_back(_T("<DebugLogUse>0</DebugLogUse>\r\n"));
	CString global_setting_info = GetGlobalSettingXMLHeader();
	for (UINT i = 0; i < global_setting_info_list.size(); i++)
		global_setting_info += global_setting_info_list[i];
	global_setting_info += GetGlobalSettingXMLTail();

	return global_setting_info;
}

CString CGlobalInfo::GetGlobalSettingPath()
{
	CString global_setting_path = CPathEnvironments::GetSingletone()->GetResourcePath();
	global_setting_path += _T("\\GlobalSetting.xml");
	return global_setting_path;
}
