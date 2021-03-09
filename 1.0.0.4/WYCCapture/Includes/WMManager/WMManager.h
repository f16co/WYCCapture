#pragma once
#include <map>

#ifndef SHARED_PTR
#if _MSC_VER > 1500		// Heigher than Visual Studio 2008  
#define SHARED_PTR std::shared_ptr
#define DYNAMIC_POINTER_CAST std::dynamic_pointer_cast
#else
#define SHARED_PTR std::tr1::shared_ptr
#define DYNAMIC_POINTER_CAST std::tr1::dynamic_pointer_cast
#endif
#endif

typedef struct _MESSAGE_PARAM_BASE
{
	CTime tCreated;

	_MESSAGE_PARAM_BASE()
	{
		tCreated = CTime::GetCurrentTime();
	}

	virtual ~_MESSAGE_PARAM_BASE()
	{

	}
} MESSAGE_PARAM_BASE, *LPMESSAGE_PARAM_BASE;


class CWMManager
{
protected:
	std::multimap<UINT, HWND> m_mapMsgWindows;
	CRITICAL_SECTION m_cs;

	std::map<int, SHARED_PTR<MESSAGE_PARAM_BASE>> m_mapParameters;
	int m_nParameterNextID;

public:
	CWMManager(void);
	virtual ~CWMManager(void);

	void Register(HWND hWnd, UINT nMsg);
	void Unregister(HWND hWnd, UINT nMsg);
	void UnregisterAll(HWND hWnd);

	LRESULT SendMessage(UINT nMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);
	void PostMessage(UINT nMsg, WPARAM wParam = NULL, LPARAM lParam = NULL);

	template <class T>
	int AddParameter(T& param);

	SHARED_PTR<MESSAGE_PARAM_BASE> GetParameter(int nParameterID);

protected:
	void Lock();
	void Unlock();
	int GetNextParameterID();
};


template  <class T>
int CWMManager::AddParameter(T& param)
{
	Lock();
	T* pNewParam = new T();
	*pNewParam = param;
	SHARED_PTR<MESSAGE_PARAM_BASE> pShared = DYNAMIC_POINTER_CAST<MESSAGE_PARAM_BASE>(SHARED_PTR<T>(pNewParam));
	int nParameterID = GetNextParameterID();
	m_mapParameters[nParameterID] = pShared;
	CTime tEraseTime = CTime::GetCurrentTime() - CTimeSpan(0, 0, 0, 5);

	std::map<int, SHARED_PTR<MESSAGE_PARAM_BASE>>::iterator posParam = m_mapParameters.begin();
	for(; posParam != m_mapParameters.end();)
	{
		if (posParam->second->tCreated < tEraseTime)
		{ 
			int nKey = posParam->first;
			posParam++;
			m_mapParameters.erase(nKey);
		}
		else
			posParam++;

	}

	Unlock();
	return nParameterID;
}

class CWMManagerEx : public CWMManager
{
public:
	static CWMManagerEx* GetSingletone()
	{
		if (!m_singletone)
		{
			m_singletone = new CWMManagerEx();
			atexit(ReleaseSingletone);
		}
		return m_singletone;
	}

private:
	CWMManagerEx(void) {}
	CWMManagerEx(const CWMManagerEx& r) {}
	~CWMManagerEx(void) {}

	static void ReleaseSingletone()
	{
		if (m_singletone)
			delete m_singletone;
	}

	static CWMManagerEx* m_singletone;
};

#ifndef WM_POST_MESSAGE2
#define WM_POST_MESSAGE2(message, wparam, lparam) CWMManagerEx::GetSingletone()->PostMessage(message, (WPARAM)wparam, (LPARAM)lparam);
#endif

#ifndef WM_POST_MESSAGE1
#define WM_POST_MESSAGE1(message, wparam) CWMManagerEx::GetSingletone()->PostMessage(message, (WPARAM)wparam, (LPARAM)NULL);
#endif

#ifndef WM_POST_MESSAGE
#define WM_POST_MESSAGE(message) CWMManagerEx::GetSingletone()->PostMessage(message, (WPARAM)NULL, (LPARAM)NULL);
#endif

#ifndef WM_SEND_MESSAGE2
#define WM_SEND_MESSAGE2(message, wparam, lparam) CWMManagerEx::GetSingletone()->SendMessage(message, (WPARAM)wparam, (LPARAM)lparam);
#endif

#ifndef WM_SEND_MESSAGE1
#define WM_SEND_MESSAGE1(message, wparam) CWMManagerEx::GetSingletone()->SendMessage(message, (WPARAM)wparam, (LPARAM)NULL);
#endif

#ifndef WM_SEND_MESSAGE
#define WM_SEND_MESSAGE(message) CWMManagerEx::GetSingletone()->SendMessage(message, (WPARAM)NULL, (LPARAM)NULL);
#endif

#ifndef WM_REGISTER
#define WM_REGISTER(message) \
	if (GetSafeHwnd()) \
		CWMManagerEx::GetSingletone()->Register(GetSafeHwnd(), message);
#endif

#ifndef WM_UNREGISTER
#define WM_UNREGISTER(message) \
	if (GetSafeHwnd()) \
		CWMManagerEx::GetSingletone()->Unregister(GetSafeHwnd(), message);
#endif

#ifndef WM_UNREGISTERALL
#define WM_UNREGISTERALL() \
	if (GetSafeHwnd()) \
		CWMManagerEx::GetSingletone()->UnregisterAll(GetSafeHwnd());
#endif