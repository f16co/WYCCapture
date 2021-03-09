#include "stdafx.h"
#include "WindowCaptureThread.h"

IMPLEMENT_DYNAMIC(CWindowCaptureThread, CWnd)

BEGIN_MESSAGE_MAP(CWindowCaptureThread, CWnd)
	ON_WM_DESTROY()
	ON_MESSAGE(UM_WINDOW_CAPTURE_PAUSE, OnWindowCapturePause)
	ON_MESSAGE(UM_WINDOW_CAPTURE_STOP, OnWindowCaptureStop)
END_MESSAGE_MAP()

CWindowCaptureThread::CWindowCaptureThread()
	: m_work(_T("CWindowCaptureThread"))
{
}

CWindowCaptureThread::~CWindowCaptureThread()
{
}

void CWindowCaptureThread::OnDestroy()
{
	CWnd::OnDestroy();

	WM_UNREGISTERALL();
}

LRESULT CWindowCaptureThread::OnWindowCapturePause(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();
	Pause();
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return 0;
}

LRESULT CWindowCaptureThread::OnWindowCaptureStop(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();
	Pause();
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return 0;
}

bool CWindowCaptureThread::Start()
{
	if (!GF()->CheckWindow(this))
	{
		if (!CWnd::CreateEx(WS_EX_APPWINDOW, ::AfxRegisterWndClass(NULL), NULL, WS_POPUP, CRect(0, 0, 0, 0), NULL, 0, NULL))
			return false;

		RegisterUserMessages();
	}

	if (!GF()->CheckWindow(m_window_capture_draw_region))
	{
		if (!m_window_capture_draw_region.Create())
			return false;
	}

	if (!m_work.Start(this))
		return false;

	return true;
}

bool CWindowCaptureThread::Stop()
{
	if (!GF()->CheckWindow(this) ||
		!GF()->CheckWindow(m_window_capture_draw_region))
		return false;

	return m_work.Stop() ? true : false;
}

void CWindowCaptureThread::Pause()
{
	if (!GF()->CheckWindow(this) ||
		!GF()->CheckWindow(m_window_capture_draw_region))
		return;

	m_window_capture_draw_region.Pause();
	m_work.Pause();
}

void CWindowCaptureThread::Resume()
{
	if (!GF()->CheckWindow(this) ||
		!GF()->CheckWindow(m_window_capture_draw_region))
		return;

	m_window_capture_draw_region.Resume();
	m_work.Resume();
}

void CWindowCaptureThread::RegisterUserMessages()
{
	WM_REGISTER(UM_WINDOW_CAPTURE_STOP);
	WM_REGISTER(UM_WINDOW_CAPTURE_PAUSE);
}

void CWindowCaptureThread::WindowCapture()
{
	CPoint cursor_pos;
	::GetCursorPos(&cursor_pos);

	CSize total_monitor_size = GF()->GetTotalMonitorSize();

	if (GF()->CheckWindow(m_window_capture_draw_region))
	{
		m_window_capture_draw_region.MoveWindow(0, 0, total_monitor_size.cx, total_monitor_size.cy);
		m_window_capture_draw_region.OnDraw(cursor_pos);
	}
}


CWindowCaptureThread::CWork::CWork(LPCTSTR name) : CThreadEx(name)
{
	m_window_capture_thread = NULL;
}

CWindowCaptureThread::CWork::~CWork()
{

}

bool CWindowCaptureThread::CWork::Start(CWindowCaptureThread* window_capture_thread)
{
	if (!window_capture_thread)
		return false;

	m_window_capture_thread = window_capture_thread;

	return CThreadEx::Start(m_window_capture_thread->GetSafeHwnd()) ? true : false;
}

void CWindowCaptureThread::CWork::Pause()
{
	if (!m_window_capture_thread || 
		!GF()->CheckWindow(m_window_capture_thread))
		return;

	m_window_capture_thread->m_window_capture_draw_region.Pause();
	CThreadEx::Pause();
}

void CWindowCaptureThread::CWork::Resume()
{
	if (!m_window_capture_thread || 
		!GF()->CheckWindow(m_window_capture_thread))
		return;

	m_window_capture_thread->m_window_capture_draw_region.Resume();
	CThreadEx::Resume();
}

BOOL CWindowCaptureThread::CWork::DoJob()
{
	while (!NeedToExit())
	{
		if (!m_window_capture_thread || 
			!GF()->CheckWindow(m_window_capture_thread))
		{
			Sleep(20);
			continue;
		}

		if (m_pause)
		{
			Sleep(20);
			m_window_capture_thread->m_window_capture_draw_region.Hide();
			continue;
		}

		m_window_capture_thread->WindowCapture();
		m_window_capture_thread->m_window_capture_draw_region.Show();
		Sleep(30);
	}

	return TRUE;
}
