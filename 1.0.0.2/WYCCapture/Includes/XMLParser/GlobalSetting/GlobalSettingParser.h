#pragma once

#include "GlobalSettingNode.h"

class CGlobalSettingParser
{
public:
	static CGlobalSettingParser* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CGlobalSettingParser();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	BOOL LoadDataFromFile(CString file);
	BOOL LoadDataFromData(CString data);
	CString SaveDataToFile(CString file);

	CGlobalSettingNode* GetGlobalSettingNode() { return &m_global_setting_node; }

private:
	CGlobalSettingParser() {}
	CGlobalSettingParser(const CGlobalSettingParser& r) {}
	~CGlobalSettingParser() {}

	static void ReleaseSingletone()
	{
		if (m_singletone)
		{
			delete m_singletone;
			m_singletone = NULL;
		}
	}

	static CGlobalSettingParser* m_singletone;

	CGlobalSettingNode m_global_setting_node;
};