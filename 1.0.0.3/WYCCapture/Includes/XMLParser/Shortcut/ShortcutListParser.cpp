#include "stdafx.h"
#include "ShortcutListParser.h"
#include "XmlSerializer.h"

CShortcutListParser* CShortcutListParser::m_singletone = NULL;

BOOL CShortcutListParser::LoadDataFromFile(CString file)
{
	return CXmlSerializer::FromXmlFile2(file, &m_shortcut_list_node);
}

BOOL CShortcutListParser::LoadDataFromData(CString data)
{
	return CXmlSerializer::FromXml(data, &m_shortcut_list_node);
}

CString CShortcutListParser::SaveDataToFile(CString file)
{
	return CXmlSerializer::ToXmlFile(file, &m_shortcut_list_node, CXmlSerializer::ENCODING_UTF8);
}

void CShortcutListParser::GetShortcutList(CString class_name, std::vector<DWORD>* shortcut_code_list, std::vector<UINT64>* shortcut_id_list, std::vector<CString>* shortcut_command_name_list)
{
	std::vector<CShortcutInfoNode> shortcut_info_node_list = m_shortcut_list_node.GetShortcutInfoNodes();
	for (int i = 0; i < (int)shortcut_info_node_list.size(); i++)
	{
		CString receiver_name = shortcut_info_node_list[i].GetReceiverName();
		if (receiver_name.CompareNoCase(class_name) == 0)
		{
			bool ctrl = shortcut_info_node_list[i].GetCtrl();
			bool alt = shortcut_info_node_list[i].GetAlt();
			bool shift = shortcut_info_node_list[i].GetShift();
			BYTE key = shortcut_info_node_list[i].GetKey();
			UINT64 shortcut_id = shortcut_info_node_list[i].GetCommandID();
			CString command_name = shortcut_info_node_list[i].GetCommandName();
			DWORD shortcut_code = CShortcutManager::GetSingletone()->MakeShortcutCode(ctrl, alt, shift, key);
			if (shortcut_code_list)
				shortcut_code_list->push_back(shortcut_code);
			if (shortcut_id_list)
				shortcut_id_list->push_back(shortcut_id);
			if (shortcut_command_name_list)
				shortcut_command_name_list->push_back(command_name);
		}
	}
}
