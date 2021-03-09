#pragma once

#include <Windows.h>
#include <afxwin.h>

enum THREAD_NOTIFY_CODE
{
	THREAD_NOTIFY_CREATE = 0,
	THREAD_NOTIFY_START,
	THREAD_NOTIFY_DOJOB,
	// Don't care return value.
	THREAD_NOTIFY_EXIT,
	// Don't care return value.
	THREAD_NOTIFY_NORMAL_EXIT,
	// Don't care return value.
	THREAD_NOTIFY_ABNORMAL_EXIT
};

class CThread;
typedef BOOL (CALLBACK *ThreadNotifyHandler)(CThread* pThread, THREAD_NOTIFY_CODE code, LPVOID param);

class CThread
{
public:
	CThread							( );
	virtual ~CThread				( );

	BOOL	CreateThread			( LPCTSTR name );
	void	DeleteThread			( );
	virtual BOOL Wait				( int wait_sec=2 );

	BOOL	IsRunning				( );
	BOOL	IsValid					( );
	BOOL	NeedToExit				( );
	void	SetName					( LPCTSTR name );

	// Don't call this method.
	BOOL ThreadProc					( );

	void RegisterNotifyHandler		( ThreadNotifyHandler handler, LPVOID param );
	void UnregisterNotifyHandler	( );

private:
	void SetSystemThreadName		( LPCSTR name );

protected:
	// This is called by CreateThread().
	// Thread is not created on FALSE is returned.
	virtual BOOL OnCreateThread		( ) { return Notify(THREAD_NOTIFY_CREATE); }

	// This is called after thread is created.
	// Thread terminates on FALSE returned and OnAbnormalExit() is called.
	virtual BOOL OnThreadStart		( ) { return Notify(THREAD_NOTIFY_START); }

	// Thread terminates on FALSE returned.
	virtual BOOL DoJob				( ) { return Notify(THREAD_NOTIFY_DOJOB); }

	virtual void OnExit				( ) { Notify(THREAD_NOTIFY_EXIT); }
	virtual void OnNormalExit		( ) { Notify(THREAD_NOTIFY_NORMAL_EXIT); }

	// This is called when an internal error occurs or DoJob() or OnThreadStart() returns FALSE.
	// Calling DeleteThread() don't call this method.
	virtual void OnAbnormalExit		( ) { Notify(THREAD_NOTIFY_ABNORMAL_EXIT); }

	BOOL	Delay					( int sec );

	BOOL	Notify					( THREAD_NOTIFY_CODE code );

	ThreadNotifyHandler				m_handler;
	LPVOID							m_handler_param;
	CRITICAL_SECTION				m_cs_handler;

	LPCTSTR							m_name;

public:
	//CWinThread						*m_thread;
	HANDLE							m_thread;
	UINT							m_threadId;
	BOOL							m_exit_inner;
	BOOL							m_exit_outer;
};

class CThreadEx : public CThread
{
public:
	CThreadEx(LPCTSTR name)
	{
		m_parent = NULL;
		m_name = name;
		m_pause = true;
	}
	virtual ~CThreadEx()
	{
		if (IsRunning())
			Stop();
	}

	virtual bool IsPause()
	{
		return m_pause;
	}
	virtual bool Start(HWND parent = NULL)
	{
		m_parent = parent;

		Pause();

		return CreateThread(m_name) ? true : false;
	}
	virtual bool Stop(int retry_cnt = 10)
	{
		DeleteThread();
		bool result = false;
		for (int i = 0; i < retry_cnt; i++)
		{
			result |= (Wait() ? true : false);
			if (result)
				break;
		}

		return result;
	}
	virtual void Resume()
	{
		m_pause = false;
	}
	virtual void Pause()
	{
		m_pause = true;
	}

protected:
	bool m_pause;
	HWND m_parent;
};

typedef void ( *ON_TIMER )( void *param );
class CTimerThread : public CThread
{
public:
	CTimerThread( )
	{
		Init( );
	}
	virtual ~CTimerThread( )
	{
		DeleteThread( );
		Wait( );
	}

	BOOL Run( LPCTSTR name, int delay_ms, ON_TIMER on_timer, void *param )
	{
		m_delay_ms = delay_ms;
		m_on_timer = on_timer;
		m_param = param;
		return CreateThread( name );
	}

protected:
	void Init( )
	{
		m_delay_ms = 0;
		m_on_timer = NULL;
		m_param = NULL;
	}
	virtual BOOL DoJob( )
	{
		int sleep = m_delay_ms / 1000;
		if( sleep > 0 )
			Delay( sleep );
		Sleep( m_delay_ms % 1000 );
		m_on_timer( m_param );
		return TRUE;
	}

protected:
	int								m_delay_ms;
	ON_TIMER						m_on_timer;
	void							*m_param;
};


class CTimerThread2 : public CThread
{
public:
	CTimerThread2( )
	{
		Init( );
	}
	virtual ~CTimerThread2( )
	{
		DeleteThread( );
		Wait( );
	}

	BOOL Run( LPCTSTR name, int delay_ms, ON_TIMER on_timer, void *param )
	{
		m_delay_ms = delay_ms;
		m_on_timer = on_timer;
		m_param = param;
		return CreateThread( name );
	}

protected:
	void Init( )
	{
		m_delay_ms = 0;
		m_on_timer = NULL;
		m_param = NULL;
	}
	virtual BOOL DoJob( )
	{
		ULONGLONG ullTick1 = GetTickCount64();
		m_on_timer( m_param );
		ULONGLONG ullTick2 = GetTickCount64();
		int nSleep = m_delay_ms - (int)(ullTick2 - ullTick1);
		if (nSleep > 0)
		{
			if (nSleep / 1000)
				Delay(nSleep / 1000);
			Sleep(nSleep % 1000);
		}
		return TRUE;
	}

protected:
	int								m_delay_ms;
	ON_TIMER						m_on_timer;
	void							*m_param;
};