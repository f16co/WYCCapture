#pragma once
#include <stdlib.h>

class CPathEnvironments
{
public:
	static CPathEnvironments* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CPathEnvironments();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

	CString GetExePath() const { return m_exe_path; }
	CString GetResourcePath() const { return m_resource_path; }

private:
	CPathEnvironments();
	CPathEnvironments(const CPathEnvironments& r);
	~CPathEnvironments();

	void SetExePath();
	void SetResourcePath();

	static void ReleaseSingletone()
	{
		if (m_singletone)
			delete m_singletone;
	}
	
	static CPathEnvironments* m_singletone;

	CString m_exe_path;
	CString m_resource_path;
};

