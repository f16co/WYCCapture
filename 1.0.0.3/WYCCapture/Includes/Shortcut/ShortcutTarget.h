#pragma once
#include <afxwin.h>
#include <map>
typedef void(*SHORTCUT_FUNC)(HWND hwnd);

class CShortcutTarget
{
public:
	CShortcutTarget()
	{
		InitializeCriticalSectionEx(&m_cs_shortcut_func, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
		m_shortcut_target_name = _T("");
	}
	virtual ~CShortcutTarget()
	{
		DeleteCriticalSection(&m_cs_shortcut_func);
	}

protected:
	virtual void InitShortcuts() = 0;
	virtual void DeleteShortcuts() = 0;

	virtual void InitShortcutTarget() = 0;

	void SetShortcutTargetName(CString shortcut_target_name) { m_shortcut_target_name = shortcut_target_name; }
	CString GetShortcutTargetName() const { return m_shortcut_target_name; }

	CRITICAL_SECTION m_cs_shortcut_func;
	std::map<int, SHORTCUT_FUNC> m_map_shortcut_func;

private:
	CString m_shortcut_target_name;
};