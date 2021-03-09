#include "stdafx.h"
#include "GlobalSettingParser.h"
#include "XmlSerializer.h"

CGlobalSettingParser* CGlobalSettingParser::m_singletone = NULL;

BOOL CGlobalSettingParser::LoadDataFromFile(CString file)
{
	return CXmlSerializer::FromXmlFile2(file, &m_global_setting_node);
}

BOOL CGlobalSettingParser::LoadDataFromData(CString data)
{
	return CXmlSerializer::FromXml(data, &m_global_setting_node);
}

CString CGlobalSettingParser::SaveDataToFile(CString file)
{
	return CXmlSerializer::ToXmlFile(file, &m_global_setting_node, CXmlSerializer::ENCODING_UTF8);
}
