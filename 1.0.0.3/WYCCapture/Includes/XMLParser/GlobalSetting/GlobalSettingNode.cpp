#include "stdafx.h"
#include "GlobalSettingNode.h"

void CGlobalSettingNode::Initialize()
{
	m_is_debug_log_use = false;
}

void CGlobalSettingNode::Uninitialize()
{
}

CGlobalSettingNode::CGlobalSettingNode(const CGlobalSettingNode& r)
{
	this->m_is_debug_log_use = r.m_is_debug_log_use;
}

CGlobalSettingNode& CGlobalSettingNode::operator=(const CGlobalSettingNode& r)
{
	if (this == &r)
		return *this;

	this->m_is_debug_log_use = r.m_is_debug_log_use;

	return *this;
}

bool CGlobalSettingNode::operator==(const CGlobalSettingNode& r)
{
	return (this->m_is_debug_log_use == r.m_is_debug_log_use);
}

bool CGlobalSettingNode::operator!=(const CGlobalSettingNode& r)
{
	return !(*this == r);
}

void CGlobalSettingNode::GetObjectData(CSerializationInfo& info)
{
	info.SetObjectName(GetEntityName());

	info.SetInt(_T("DebugLogUse"), (int)m_is_debug_log_use);
}

void CGlobalSettingNode::SetObjectData(CSerializationInfo& info)
{
	if (info.GetObjectName().CompareNoCase(GetEntityName()))
		return;

	m_is_debug_log_use = (bool)info.GetInt(_T("debugloguse"));
}