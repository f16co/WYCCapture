#pragma once

#include "ShortcutListNode.h"

// ����Ű ����� hwnd�� ���� Ŭ������ Ŭ������� ����Ű�� �������� �Ѵ�.

// ex)
// <ShortcutList>
//		<ShortcutInfo>
//			<ReceiverName>CLoadingPanel</ReceiverName>
//			<CommandName>Exit(&E)...</CommandName>
//			<Key>ESC</Key>
//		</ShortcutInfo>
// </ShortcutList>

class CShortcutListParser
{
public:
	static CShortcutListParser* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CShortcutListParser();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	BOOL LoadDataFromFile(CString file);
	BOOL LoadDataFromData(CString data);
	CString SaveDataToFile(CString file);

	void GetShortcutList(CString class_name, std::vector<DWORD>* shortcut_code_list = NULL, std::vector<UINT64>* shortcut_id_list = NULL, std::vector<CString>* shortcut_command_name_list = NULL);

	CShortcutListNode* GetShortcutListNode() { return &m_shortcut_list_node; }

private:
	CShortcutListParser() {}
	CShortcutListParser(const CShortcutListParser& r) {}
	~CShortcutListParser() {}

	static void ReleaseSingletone()
	{
		if (m_singletone)
		{
			delete m_singletone;
			m_singletone = NULL;
		}
	}

	static CShortcutListParser* m_singletone;

	CShortcutListNode m_shortcut_list_node;
};