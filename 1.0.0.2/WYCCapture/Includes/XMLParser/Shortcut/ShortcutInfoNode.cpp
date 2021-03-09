#include "stdafx.h"
#include "ShortcutInfoNode.h"
#include "KeyMapper.h"

void CShortcutInfoNode::Initialize()
{
	m_receiver_name = _T("");
	m_command_name = _T("");
	m_ctrl = false;
	m_alt = false;
	m_shift = false;
	m_key = 0;
	m_command_id = -1;
}

void CShortcutInfoNode::Uninitialize()
{

}

CShortcutInfoNode::CShortcutInfoNode(const CShortcutInfoNode& r)
{
	this->m_receiver_name = r.m_receiver_name;
	this->m_command_name = r.m_command_name;
	this->m_command_id = r.m_command_id;
	this->m_ctrl = r.m_ctrl;
	this->m_alt = r.m_alt;
	this->m_shift = r.m_shift;
	this->m_key = r.m_key;
}

CShortcutInfoNode& CShortcutInfoNode::operator=(const CShortcutInfoNode& r)
{
	if (this == &r)
		return *this;

	this->m_receiver_name = r.m_receiver_name;
	this->m_command_name = r.m_command_name;
	this->m_command_id = r.m_command_id;
	this->m_ctrl = r.m_ctrl;
	this->m_alt = r.m_alt;
	this->m_shift = r.m_shift;
	this->m_key = r.m_key;

	return *this;
}

bool CShortcutInfoNode::operator==(const CShortcutInfoNode& r)
{
	return (this->m_receiver_name == r.m_receiver_name &&
			this->m_command_name == r.m_command_name &&
			this->m_command_id == r.m_command_id &&
			this->m_ctrl == r.m_ctrl &&
			this->m_alt == r.m_alt &&
			this->m_shift == r.m_shift &&
			this->m_key == r.m_key);
}

bool CShortcutInfoNode::operator!=(const CShortcutInfoNode& r)
{
	return !(*this == r);
}

void CShortcutInfoNode::GetObjectData(CSerializationInfo& info)
{
	info.SetObjectName(GetEntityName());

	info.SetString(_T("ReceiverName"), m_receiver_name);
	info.SetString(_T("CommandName"), m_command_name);
	info.SetInt(_T("CommandID"), m_command_id);
	CString key_string = CKeyMapper::MakeKeyString(m_ctrl, m_alt, m_shift, m_key);
	info.SetString(_T("Key"), key_string);
}

void CShortcutInfoNode::SetObjectData(CSerializationInfo& info)
{
	if (info.GetObjectName().CompareNoCase(GetEntityName()))
		return;

	m_receiver_name = info.GetString(_T("receivername"));
	m_command_name = info.GetString(_T("commandname"));
	m_command_id = info.GetInt(_T("commandid"));
	CString key_string = info.GetString(_T("key"));
	if (!CKeyMapper::ParseKeyString(key_string, &m_ctrl, &m_alt, &m_shift, &m_key))
		TRACE(_T("Failed to parse keyboard string!\n"));
}
