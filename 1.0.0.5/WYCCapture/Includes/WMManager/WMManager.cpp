#include "StdAfx.h"
#include "WMManager.h"
#include <list>

CWMManager::CWMManager(void)
{
	InitializeCriticalSectionEx(&m_cs, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
	m_nParameterNextID = 0;
}

CWMManager::~CWMManager(void)
{
	DeleteCriticalSection(&m_cs);
	m_mapParameters.clear();
	m_mapMsgWindows.clear();
}

void CWMManager::Register(HWND hWnd, UINT nMsg)
{
	Lock();
	m_mapMsgWindows.insert( std::pair<UINT, HWND>(nMsg, hWnd) );
	Unlock();
}

void CWMManager::Unregister(HWND hWnd, UINT nMsg)
{
	std::multimap<UINT, HWND>::iterator pos;
	std::pair<std::multimap<UINT, HWND>::iterator, std::multimap<UINT, HWND>::iterator> range;

	Lock();

	range = m_mapMsgWindows.equal_range(nMsg);
	pos = range.first;
	while(pos != range.second)
	{
		if( pos->second == hWnd )
			pos = m_mapMsgWindows.erase(pos);
		else
			pos++;
	}

	Unlock();
}

void CWMManager::UnregisterAll(HWND hWnd)
{
	std::multimap<UINT, HWND>::iterator pos;

	Lock();

	pos = m_mapMsgWindows.begin();
	while(pos != m_mapMsgWindows.end())
	{
		if( pos->second == hWnd )
			pos = m_mapMsgWindows.erase(pos);
		else
			pos++;
	}

	Unlock();
}

LRESULT CWMManager::SendMessage(UINT nMsg, WPARAM wParam/* = NULL*/, LPARAM lParam/* = NULL*/)
{
	std::multimap<UINT, HWND>::iterator pos;
	std::pair<std::multimap<UINT, HWND>::iterator, std::multimap<UINT, HWND>::iterator> range;
	std::list<HWND> lstHwnds;

	Lock();

	range = m_mapMsgWindows.equal_range(nMsg);
	pos = range.first;

	while(pos != range.second)
	{
		lstHwnds.push_back(pos->second);
		pos++;
	}

	Unlock();

	LRESULT lResult = 0;
	std::list<HWND>::iterator posHwnd = lstHwnds.begin();
	while(posHwnd != lstHwnds.end())
	{
		lResult = ::SendMessage(*posHwnd, nMsg, wParam, lParam);
		posHwnd++;
	}
	return lResult;
}

void CWMManager::PostMessage(UINT nMsg, WPARAM wParam/* = NULL*/, LPARAM lParam/* = NULL*/)
{
	std::multimap<UINT, HWND>::iterator pos;
	std::pair<std::multimap<UINT, HWND>::iterator, std::multimap<UINT, HWND>::iterator> range;
	std::list<HWND> lstHwnds;

	Lock();

	range = m_mapMsgWindows.equal_range(nMsg);
	pos = range.first;

	while(pos != range.second)
	{
		lstHwnds.push_back(pos->second);
		pos++;
	}

	Unlock();

	std::list<HWND>::iterator posHwnd = lstHwnds.begin();
	while(posHwnd != lstHwnds.end())
	{
		::PostMessage(*posHwnd, nMsg, wParam, lParam);
		posHwnd++;
	}
}

void CWMManager::Lock()
{
	while(!TryEnterCriticalSection(&m_cs))
	{
		Sleep(1);
	}
}

void CWMManager::Unlock()
{
	LeaveCriticalSection(&m_cs);
}

int CWMManager::GetNextParameterID()
{
	if (m_nParameterNextID >= 2147483646)
		m_nParameterNextID = 0;

	m_nParameterNextID++;
	return m_nParameterNextID;
}

SHARED_PTR<MESSAGE_PARAM_BASE> CWMManager::GetParameter(int nParameterID)
{
	SHARED_PTR<MESSAGE_PARAM_BASE> pResult;
	Lock();
	if (m_mapParameters.find(nParameterID) == m_mapParameters.end())
	{
		Unlock();
		return pResult;
	}
	pResult = m_mapParameters[nParameterID];
	Unlock();
	return pResult;
}

CWMManagerEx* CWMManagerEx::m_singletone = NULL;
