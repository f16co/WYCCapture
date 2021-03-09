#pragma once
#include "KeyMapper.h"
#include <map>
#include <vector>

#define WM_SHORTCUT_GENERATED WM_USER + 1213

typedef struct _SHORTCUT_TARGET
{
	UINT64 id;
	HWND hwnd;

	bool IsValid()
	{
		return (id >= 0 && !hwnd);
	}
	_SHORTCUT_TARGET()
	{
		id = -1;
		hwnd = NULL;
	}
	~_SHORTCUT_TARGET()
	{

	}
	_SHORTCUT_TARGET(UINT64 id, HWND hwnd)
	{
		this->id = id;
		this->hwnd = hwnd;
	}
	_SHORTCUT_TARGET(const _SHORTCUT_TARGET& r)
	{
		this->hwnd = r.hwnd;
		this->id = r.id;
	}
	_SHORTCUT_TARGET& operator=(const _SHORTCUT_TARGET& r)
	{
		if (this == &r)
			return *this;

		this->hwnd = r.hwnd;
		this->id = r.id;

		return *this;
	}
	bool operator==(const _SHORTCUT_TARGET& r)
	{
		return (this->hwnd == r.hwnd &&
				this->id == r.id);
	}
	bool operator!=(const _SHORTCUT_TARGET& r)
	{
		return !(*this == r);
	}
} SHORTCUT_TARGET, *LPSHORTCUT_TARGET;

typedef struct _SHORTCUT_DATA
{
	bool ctrl;
	bool alt;
	bool shift;
	BYTE key;

	_SHORTCUT_DATA()
	{
		ctrl = false;
		alt = false;
		shift = false;
		key = 0;
	}
	~_SHORTCUT_DATA()
	{

	}
	_SHORTCUT_DATA(bool ctrl, bool alt, bool shift, BYTE key)
	{
		this->ctrl = ctrl;
		this->alt = alt;
		this->shift = shift;
		this->key = key;
	}
	_SHORTCUT_DATA(CString key_string)
	{
		this->ctrl = false;
		this->alt = false;
		this->shift = false;
		this->key = 0;
		CKeyMapper::ParseKeyString(key_string, &this->ctrl, &this->alt, &this->shift, &this->key);
	}
	_SHORTCUT_DATA(const _SHORTCUT_DATA& r)
	{
		this->ctrl = r.ctrl;
		this->alt = r.alt;
		this->shift = r.shift;
		this->key = r.key;
	}
	_SHORTCUT_DATA& operator=(const _SHORTCUT_DATA& r)
	{
		if (this == &r)
			return *this;

		this->ctrl = r.ctrl;
		this->alt = r.alt;
		this->shift = r.shift;
		this->key = r.key;

		return *this;
	}
	bool operator==(const _SHORTCUT_DATA& r)
	{
		return (this->ctrl == r.ctrl &&
				this->alt == r.alt &&
				this->shift == r.shift &&
				this->key == r.key);
	}
	bool operator!=(const _SHORTCUT_DATA& r)
	{
		return !(*this == r);
	}
} SHORTCUT_DATA, *LPSHORTCUT_DATA;

class CShortcutManager
{
public:
	static CShortcutManager* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CShortcutManager();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	static bool InstallHook(HINSTANCE hinstance = NULL);
	static void RemoveHook();

	SHORTCUT_DATA ParseShortcutString(CString shortcut_string);

	SHORTCUT_DATA ParseShortcutCode(DWORD shortcut_code);

	DWORD MakeShortcutCode(CString shortcut_string);
	DWORD MakeShortcutCode(bool ctrl, bool alt, bool shift, BYTE key);

	void SetShortcut(SHORTCUT_TARGET& shortcut_target, DWORD shortcut_code);
	void SetShortcut(SHORTCUT_TARGET& shortcut_target, bool ctrl, bool alt, bool shift, BYTE key);
	
	void DeleteShortcut(SHORTCUT_TARGET& shortcut_target);
	void DeleteShortcut(SHORTCUT_TARGET& shortcut_target, DWORD shortcut_code);
	void DeleteShortcut(SHORTCUT_TARGET& shortcut_target, bool ctrl, bool alt, bool shift, BYTE key);

	CString MakeShortcutStrinig(DWORD shortcut_code);
	CString MakeShortcutStrinig(bool ctrl, bool alt, bool shift, BYTE key);

	std::map<DWORD, std::vector<SHORTCUT_TARGET>> GetShortcutData();

	void ClearShortcuts();

	void Pause() { m_is_pause = true; }
	void Resume() { m_is_pause = false; }

private:
	CShortcutManager();
	~CShortcutManager();
	CShortcutManager(const CShortcutManager& r) {}

	static void ReleaseSingletone()
	{
		if (m_singletone)
		{
			delete m_singletone;
			m_singletone = NULL;
		}
	}

	static LRESULT HookProc(int code, WPARAM wparam, LPARAM lparam);
	void Emit(WPARAM wparam, LPARAM lparam);

	std::vector<SHORTCUT_TARGET> GetShortcutTargetList(DWORD shortcut_code);
	std::vector<SHORTCUT_TARGET> GetShortcutTargetList(bool ctrl, bool alt, bool shift, BYTE key);

	CRITICAL_SECTION m_cs_shortcut_data;
	std::map<DWORD, std::vector<SHORTCUT_TARGET>> m_map_shortcut_data;

	bool m_is_pause;

	static CShortcutManager* m_singletone;
	static HHOOK m_hook;
};
