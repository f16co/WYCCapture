#include "stdafx.h"
#include "GlobalSetting.h"

CGlobalSetting* CGlobalSetting::m_singletone = NULL;

CGlobalSetting::CGlobalSetting()
	: m_is_debug_log_use(false)
{
}

CGlobalSetting::CGlobalSetting(const CGlobalSetting & r)
{
}

CGlobalSetting::~CGlobalSetting()
{
}
