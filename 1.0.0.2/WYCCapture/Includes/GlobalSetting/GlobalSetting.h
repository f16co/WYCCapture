#pragma once

class CGlobalSetting
{
public:
	static CGlobalSetting* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CGlobalSetting();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	void SetDebugLogUse(bool is_debug_log_use) { m_is_debug_log_use = is_debug_log_use; }
	bool GetDebugLogUse() const { return m_is_debug_log_use; }

private:
	CGlobalSetting();
	CGlobalSetting(const CGlobalSetting& r);
	~CGlobalSetting();

	static void ReleaseSingletone()
	{
		if (m_singletone)
		{
			delete m_singletone;
			m_singletone = NULL;
		}
	}

	static CGlobalSetting* m_singletone;

	bool m_is_debug_log_use;
};

#ifndef GS
#define GS() CGlobalSetting::GetSingletone()
#endif