#pragma once

class CSystemInitializer
{
public:
	static CSystemInitializer* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CSystemInitializer();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	void Init();
	void Exit();

	CString GetProductName();
	CString GetProgramName(bool full_path = true);

private:
	CSystemInitializer();
	CSystemInitializer(const CSystemInitializer& r);
	~CSystemInitializer();

	void LoadResource();
	void LoadGlobalSetting();
	void LoadShortcut();

	static void ReleaseSingletone()
	{
		if (m_singletone)
		{
			delete m_singletone;
			m_singletone = NULL;
		}
	}

	static CSystemInitializer* m_singletone;
};

