#include "stdafx.h"
#include "ShortcutManager.h"

CShortcutManager* CShortcutManager::m_singletone = NULL;
HHOOK CShortcutManager::m_hook = NULL;

CShortcutManager::CShortcutManager()
{
	m_is_pause = true;
	InitializeCriticalSectionEx(&m_cs_shortcut_data, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
}

CShortcutManager::~CShortcutManager()
{
	DeleteCriticalSection(&m_cs_shortcut_data);
}

void CShortcutManager::SetShortcut(SHORTCUT_TARGET& shortcut_target, DWORD shortcut_code)
{
	EnterCriticalSection(&m_cs_shortcut_data);
	m_map_shortcut_data[shortcut_code].push_back(shortcut_target);
	LeaveCriticalSection(&m_cs_shortcut_data);
}

void CShortcutManager::SetShortcut(SHORTCUT_TARGET& shortcut_target, bool ctrl, bool alt, bool shift, BYTE key)
{
	DWORD shortcut_code = MakeShortcutCode(ctrl, alt, shift, key);
	SetShortcut(shortcut_target, shortcut_code);
}

void CShortcutManager::DeleteShortcut(SHORTCUT_TARGET& shortcut_target)
{
	EnterCriticalSection(&m_cs_shortcut_data);
	std::map<DWORD, std::vector<SHORTCUT_TARGET>>::iterator pos1 = m_map_shortcut_data.begin();
	while (pos1 != m_map_shortcut_data.end())
	{
		std::vector<SHORTCUT_TARGET>::iterator pos2 = std::find(pos1->second.begin(), pos1->second.end(), shortcut_target);
		if (pos2 != pos1->second.end())
			pos1->second.erase(pos2);
		pos1++;
	}
	LeaveCriticalSection(&m_cs_shortcut_data);
}

void CShortcutManager::DeleteShortcut(SHORTCUT_TARGET& shortcut_target, DWORD shortcut_code)
{
	EnterCriticalSection(&m_cs_shortcut_data);
	std::map<DWORD, std::vector<SHORTCUT_TARGET>>::iterator find_result = m_map_shortcut_data.find(shortcut_code);
	if (find_result != m_map_shortcut_data.end())
	{
		std::vector<SHORTCUT_TARGET>::iterator pos = std::find(find_result->second.begin(), find_result->second.end(), shortcut_target);
		if (pos != find_result->second.end())
			find_result->second.erase(pos);
	}
	LeaveCriticalSection(&m_cs_shortcut_data);
}

void CShortcutManager::DeleteShortcut(SHORTCUT_TARGET& shortcut_target, bool ctrl, bool alt, bool shift, BYTE key)
{
	DWORD shortcut_code = MakeShortcutCode(ctrl, alt, shift, key);
	DeleteShortcut(shortcut_target, shortcut_code);
}

std::vector<SHORTCUT_TARGET> CShortcutManager::GetShortcutTargetList(DWORD shortcut_code)
{
	std::vector<SHORTCUT_TARGET> shortcut_target_list;
	EnterCriticalSection(&m_cs_shortcut_data);
	std::map<DWORD, std::vector<SHORTCUT_TARGET>>::iterator find_result = m_map_shortcut_data.find(shortcut_code);
	if (find_result != m_map_shortcut_data.end())
		shortcut_target_list = find_result->second;
	LeaveCriticalSection(&m_cs_shortcut_data);

	return shortcut_target_list;
}

std::vector<SHORTCUT_TARGET> CShortcutManager::GetShortcutTargetList(bool ctrl, bool alt, bool shift, BYTE key)
{
	DWORD shortcut_code = MakeShortcutCode(ctrl, alt, shift, key);
	return GetShortcutTargetList(shortcut_code);
}

bool CShortcutManager::InstallHook(HINSTANCE hinstance /*= NULL*/)
{
	m_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, hinstance, NULL);
	if (!m_hook)
	{
		TRACE(_T("Unable to install hook!\n"));
		return false;
	}

	return true;
}

void CShortcutManager::RemoveHook()
{
	if (!m_hook)
		return;

	UnhookWindowsHookEx(m_hook);
}

CString CShortcutManager::MakeShortcutStrinig(DWORD shortcut_code)
{
	SHORTCUT_DATA shortcut_data = ParseShortcutCode(shortcut_code);
	return MakeShortcutStrinig(shortcut_data.ctrl, shortcut_data.alt, shortcut_data.shift, shortcut_data.key);
}

CString CShortcutManager::MakeShortcutStrinig(bool ctrl, bool alt, bool shift, BYTE key)
{
	return CKeyMapper::MakeKeyString(ctrl, alt, shift, key);
}

std::map<DWORD, std::vector<SHORTCUT_TARGET>> CShortcutManager::GetShortcutData()
{
	std::map<DWORD, std::vector<SHORTCUT_TARGET>> shortcut_data;

	EnterCriticalSection(&m_cs_shortcut_data);
	shortcut_data = m_map_shortcut_data;
	LeaveCriticalSection(&m_cs_shortcut_data);

	return shortcut_data;
}

void CShortcutManager::ClearShortcuts()
{
	EnterCriticalSection(&m_cs_shortcut_data);
	m_map_shortcut_data.clear();
	LeaveCriticalSection(&m_cs_shortcut_data);
}

SHORTCUT_DATA CShortcutManager::ParseShortcutString(CString shortcut_string)
{
	bool ctrl = false;
	bool alt = false;
	bool shift = false;
	BYTE key = 0;
	CKeyMapper::ParseKeyString(shortcut_string, &ctrl, &alt, &shift, &key);

	return SHORTCUT_DATA(ctrl, alt, shift, key);
}

SHORTCUT_DATA CShortcutManager::ParseShortcutCode(DWORD shortcut_code)
{
	DWORD dw_ctrl = 0, dw_alt = 0, dw_shift = 0, dw_key = 0;
	dw_ctrl = (shortcut_code & 0xff000000) >> 24;
	dw_alt = (shortcut_code & 0x00ff0000) >> 16;
	dw_shift = (shortcut_code & 0x0000ff00) >> 8;
	dw_key = shortcut_code & 0x000000ff;

	bool ctrl = (bool)dw_ctrl;
	bool alt = (bool)dw_alt;
	bool shift = (bool)dw_shift;
	BYTE key = (BYTE)dw_key;

	return SHORTCUT_DATA(ctrl, alt, shift, key);
}

LRESULT CShortcutManager::HookProc(int code, WPARAM wparam, LPARAM lparam)
{
	if (code >= 0)
		GetSingletone()->Emit(wparam, lparam);

	return CallNextHookEx(m_hook, code, wparam, lparam);
}

void CShortcutManager::Emit(WPARAM wparam, LPARAM lparam)
{
	if (m_is_pause)
		return;

	LPKBDLLHOOKSTRUCT keyboard_param = (LPKBDLLHOOKSTRUCT)lparam;
	if (wparam == WM_KEYDOWN)
	{
		BYTE key = (BYTE)keyboard_param->vkCode;
		bool ctrl = false, alt = false, shift = false;
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000 || GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000)
			ctrl = true;
		if (GetAsyncKeyState(VK_MENU) & 0x8000 || GetAsyncKeyState(VK_LMENU) & 0x8000 || GetAsyncKeyState(VK_RMENU) & 0x8000)
			alt = true;
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000 || GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000)
			shift = true;

		std::vector<SHORTCUT_TARGET> shortcut_target_list = GetShortcutTargetList(ctrl, alt, shift, key);
		int shortcut_target_list_size = (int)shortcut_target_list.size();
		for (int i = 0; i < shortcut_target_list_size; i++)
		{
			DWORD shortcut_code = MakeShortcutCode(ctrl, alt, shift, key);
			::PostMessage(shortcut_target_list[i].hwnd, WM_SHORTCUT_GENERATED, (WPARAM)shortcut_code, (LPARAM)shortcut_target_list[i].id);
		}
	}
}

DWORD CShortcutManager::MakeShortcutCode(CString shortcut_string)
{
	bool ctrl = false;
	bool alt = false;
	bool shift = false;
	BYTE key = 0;
	CKeyMapper::ParseKeyString(shortcut_string, &ctrl, &alt, &shift, &key);

	return MakeShortcutCode(ctrl, alt, shift, key);
}

DWORD CShortcutManager::MakeShortcutCode(bool ctrl, bool alt, bool shift, BYTE key)
{
	DWORD shortcut_code = 0;
	DWORD dw_ctrl = 0, dw_alt = 0, dw_shift = 0, dw_key = 0;
	dw_ctrl = ((DWORD)ctrl) << 24;
	dw_alt = ((DWORD)alt) << 16;
	dw_shift = ((DWORD)shift) << 8;
	dw_key = (DWORD)key;
	
	shortcut_code = dw_ctrl | dw_alt | dw_shift | dw_key;
	return shortcut_code;
}
