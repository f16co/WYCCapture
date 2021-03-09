#include "stdafx.h"
#include "SystemInitializer.h"
#include "PathEnvironments.h"
#include "ShortcutListParser.h"
#include "WndManager.h"
#include "GlobalSettingParser.h"

CSystemInitializer* CSystemInitializer::m_singletone = NULL;

CSystemInitializer::CSystemInitializer()
{

}

CSystemInitializer::~CSystemInitializer()
{
	
}

void CSystemInitializer::LoadResource()
{
	CString resource_path = CPathEnvironments::GetSingletone()->GetResourcePath();
	if (!GF()->CheckFileExist(resource_path))
		CreateDirectory(resource_path, NULL);

	LoadGlobalSetting();
	LoadShortcut();
}

void CSystemInitializer::LoadGlobalSetting()
{
	CString global_setting_path = GI()->GetGlobalSettingPath();
	if (!GF()->CheckFileExist(global_setting_path))
	{
		CString default_global_setting = GI()->GetDefaultGlobalSetting();
		CGlobalSettingParser::GetSingletone()->LoadDataFromData(default_global_setting);
		CGlobalSettingParser::GetSingletone()->SaveDataToFile(global_setting_path);
	}
	else
		CGlobalSettingParser::GetSingletone()->LoadDataFromFile(global_setting_path);

	CGlobalSettingNode* global_setting_node = CGlobalSettingParser::GetSingletone()->GetGlobalSettingNode();
	GS()->SetDebugLogUse(global_setting_node->GetDebugLogUse());
}

void CSystemInitializer::LoadShortcut()
{
	CString shortcut_path = GI()->GetShortcutPath();
	if (!GF()->CheckFileExist(shortcut_path))
	{
		CString default_shortcut = GI()->GetDefaultShortcut();
		CShortcutListParser::GetSingletone()->LoadDataFromData(default_shortcut);
		CShortcutListParser::GetSingletone()->SaveDataToFile(shortcut_path);
	}
	else
		CShortcutListParser::GetSingletone()->LoadDataFromFile(shortcut_path);
}

void CSystemInitializer::Init()
{
	CShortcutManager::InstallHook(AfxGetInstanceHandle());
	
	GI()->SetProgramName(GetProgramName(false));
	GI()->SetProductName(GetProductName());

	if (!CWndManager::GetSingletone()->LoadAllWindows())
		return;

	LoadResource();

	CTraceEx::Init(GS()->GetDebugLogUse());
}

void CSystemInitializer::Exit()
{
	CString shortcut_path = GI()->GetShortcutPath();
	CShortcutListParser::GetSingletone()->SaveDataToFile(shortcut_path);
	CShortcutManager::RemoveHook();
	CTraceEx::Exit(GS()->GetDebugLogUse());
}

CString CSystemInitializer::GetProductName()
{
	TCHAR program_name[MAX_PATH] = { 0, };
	::GetModuleFileName(NULL, program_name, MAX_PATH);

	CString temp_string = program_name;
	int start_index = temp_string.ReverseFind(_T('\\'));
	int end_index = temp_string.Find(_T(".exe"), start_index);

	return temp_string.Mid(start_index + 1, end_index - (start_index + 1));;
}

CString CSystemInitializer::GetProgramName(bool full_path /*= true*/)
{
	TCHAR program_name[MAX_PATH] = { 0, };
	::GetModuleFileName(NULL, program_name, MAX_PATH);

	CString temp_str = program_name;
	if (!full_path)
		temp_str = temp_str.Right(temp_str.GetLength() - (temp_str.ReverseFind(_T('\\')) + 1));

	return temp_str;
}
