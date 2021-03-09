#include "stdafx.h"
#include "ThreadEx.h"
#include "process.h"

CThread::CThread( )
	: m_exit_inner(TRUE)
	, m_exit_outer(TRUE)
	, m_thread(NULL)
	, m_handler(NULL)
	, m_handler_param(NULL)
{
	InitializeCriticalSectionEx(&m_cs_handler, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
}

CThread::~CThread( )
{
	DeleteThread();
	Wait();
	DeleteCriticalSection(&m_cs_handler);

	// VS2017에서 SetName 함수가 적용이 되기는 하나 (디버깅 시 스레드 탭에서 확인했을 시에 이름이 설정되어 있음)
	// 종료 시점에 스레드 이름이 나오지 않는 증상을 발견.
	// 그래서 2017 사용을 대비해 소멸자에 name을 TRACE 해주는 부분을 추가.
#if _MSC_VER > 1500
	CString name;
#if defined(_UNICODE) || defined(UNICODE)
	name = CStringW(m_name);
#else
	name = m_name;
#endif
	if (!name.IsEmpty())
		TRACE(_T("%s exit.\n"), name);
#endif
}

static UINT __stdcall g_run_thread( void *arg )
{
	return ( (CThread*)arg )->ThreadProc( ) ? 0 : -1;
}

BOOL CThread::CreateThread( LPCTSTR name )
{
	if( m_thread )
		return FALSE;

	if( !OnCreateThread() )
		return FALSE;

	m_exit_inner = FALSE;
	m_exit_outer = FALSE;
	m_thread = (HANDLE)_beginthreadex( NULL, 0, &g_run_thread, this, 0, &m_threadId );
	//m_thread = AfxBeginThread( g_run_thread, (void*)this );
	if( m_thread <= 0 )
	{
		m_exit_outer = TRUE;
		m_exit_inner = TRUE;
		return FALSE;
	}

	if( name )
		SetName( name );

	return TRUE;
}

void CThread::SetName( LPCTSTR name )
{
	if( name )
	{
		m_name = name;
		SetSystemThreadName(CStringA(name));
	}
}

void CThread::SetSystemThreadName(LPCSTR name)
{
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCTSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	}THREADNAME_INFO;
#pragma pack(pop)

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = m_name;
	info.dwThreadID = m_threadId;
	info.dwFlags = 0;
	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

void CThread::DeleteThread( )
{
	if( m_thread )
		m_exit_outer = TRUE;
}

BOOL CThread::Wait( int wait_sec )
{
	if( !m_thread )
		return TRUE;

	for( int i=0; i<wait_sec*10; i++ )
	{
		Sleep( 100 );
		if( m_exit_inner && !m_thread )
		{
			m_thread = NULL;
			return TRUE;
		}
	}
	TRACE( _T("%s : FALSE\n"), m_name );
	return FALSE;
}

BOOL CThread::IsRunning( )
{
	return !m_exit_inner;
}

BOOL CThread::IsValid( )
{
	return !m_exit_outer;
}

BOOL CThread::ThreadProc( )
{
	if( !OnThreadStart() )
		goto ABNORMAL;

	while( !m_exit_outer )
	{
		if( !DoJob() )
			goto ABNORMAL;
	}

	OnNormalExit( );
	OnExit( );
	CloseHandle( m_thread );
	m_exit_inner = TRUE;
	m_thread = NULL;
	//AfxEndThread( 0, TRUE );
	return TRUE;

ABNORMAL:
	OnAbnormalExit( );
	OnExit( );
	CloseHandle( m_thread );
	m_exit_inner = TRUE;
	m_thread = NULL;
	//AfxEndThread( 0, TRUE );
	return TRUE;
}

BOOL CThread::Delay( int sec )
{
	for( int i=0; i<sec * 10; i++ )
	{
		if( m_exit_outer )
			return FALSE;
		Sleep( 100 );
	}
	return TRUE;
}

BOOL CThread::NeedToExit( )
{
	return m_exit_outer;
}


BOOL CThread::Notify( THREAD_NOTIFY_CODE code )
{
	ThreadNotifyHandler handler = NULL;
	LPVOID param = NULL;
	
	EnterCriticalSection(&m_cs_handler);
	handler = m_handler;
	param = m_handler_param;
	LeaveCriticalSection(&m_cs_handler);

	if (!handler)
		return TRUE;

	return handler(this, code, param);
}

void CThread::RegisterNotifyHandler( ThreadNotifyHandler handler, LPVOID param )
{
	EnterCriticalSection(&m_cs_handler);
	m_handler = handler;
	m_handler_param = param;
	LeaveCriticalSection(&m_cs_handler);
}

void CThread::UnregisterNotifyHandler( )
{
	EnterCriticalSection(&m_cs_handler);
	m_handler = NULL;
	m_handler_param = NULL;
	LeaveCriticalSection(&m_cs_handler);
}
