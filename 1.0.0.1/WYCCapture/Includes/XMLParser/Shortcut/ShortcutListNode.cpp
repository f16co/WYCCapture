#include "stdafx.h"
#include "ShortcutListNode.h"

void CShortcutListNode::Initialize()
{
	m_shortcut_info_nodes.clear();
}

void CShortcutListNode::Uninitialize()
{
	m_shortcut_info_nodes.clear();
}

CShortcutListNode::CShortcutListNode(const CShortcutListNode& r)
{
	this->m_shortcut_info_nodes = r.m_shortcut_info_nodes;
}

CShortcutListNode& CShortcutListNode::operator=(const CShortcutListNode& r)
{
	if (this == &r)
		return *this;

	this->m_shortcut_info_nodes = r.m_shortcut_info_nodes;

	return *this;
}

bool CShortcutListNode::operator==(const CShortcutListNode& r)
{
	if (this->m_shortcut_info_nodes.size() != r.m_shortcut_info_nodes.size())
		return false;
	for (int i = 0; i < (int)this->m_shortcut_info_nodes.size(); i++)
	{
		if (this->m_shortcut_info_nodes[i] != r.m_shortcut_info_nodes[i])
			return false;
	}
	return true;
}

bool CShortcutListNode::operator!=(const CShortcutListNode& r)
{
	return !(*this == r);
}

void CShortcutListNode::GetObjectData(CSerializationInfo& info) 
{
	info.SetObjectName(GetEntityName());

	if (m_shortcut_info_nodes.size() <= 0)
		return;

	CSerializationInfo* pNext = info.CreateChildInfo();
	std::vector<CShortcutInfoNode>::iterator pos_item = m_shortcut_info_nodes.begin();
	for (; pos_item != m_shortcut_info_nodes.end(); pos_item++)
	{
		if (pos_item != m_shortcut_info_nodes.begin())
			pNext = pNext->CreateNextInfo();
		(*pos_item).GetObjectData(*pNext);
	}
}

void CShortcutListNode::SetObjectData(CSerializationInfo& info) 
{
	if (info.GetObjectName().CompareNoCase(GetEntityName()))
		return;

	CSerializationInfo* pNext = info.GetChildInfo();
	for (; pNext; pNext = pNext->GetNextInfo())
	{
		CShortcutInfoNode shortcut_info;
		shortcut_info.SetObjectData(*pNext);
		m_shortcut_info_nodes.push_back(shortcut_info);
	}
}

CString CShortcutListNode::GetCommandName(UINT64 shortcut_id)
{
	CString command_name;

	std::vector<CShortcutInfoNode>::iterator pos = m_shortcut_info_nodes.begin();
	while (pos != m_shortcut_info_nodes.end())
	{
		if ((*pos).GetCommandID() == shortcut_id)
		{
			command_name = (*pos).GetCommandName();
			break;
		}
		pos++;
	}

	return command_name;
}

CString CShortcutListNode::GetReceiverName(UINT64 shortcut_id)
{
	CString receiver_name;

	std::vector<CShortcutInfoNode>::iterator pos = m_shortcut_info_nodes.begin();
	while (pos != m_shortcut_info_nodes.end())
	{
		if ((*pos).GetCommandID() == shortcut_id)
		{
			receiver_name = (*pos).GetReceiverName();
			break;
		}
		pos++;
	}

	return receiver_name;
}