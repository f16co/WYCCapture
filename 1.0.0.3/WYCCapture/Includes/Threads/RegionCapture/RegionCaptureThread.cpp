#include "stdafx.h"
#include "RegionCaptureThread.h"
#include "Monitors.h"
#include "MultiMonitor.h"

IMPLEMENT_DYNAMIC(CRegionCaptureThread, CWnd)

BEGIN_MESSAGE_MAP(CRegionCaptureThread, CWnd)
	ON_WM_DESTROY()
	ON_MESSAGE(UM_REGION_CAPTURE_PAUSE, OnRegionCapturePause)
	ON_MESSAGE(UM_REGION_CAPTURE_STOP, OnRegionCaptureStop)
END_MESSAGE_MAP()

CRegionCaptureThread::CRegionCaptureThread()
	: m_work(_T("CRegionCaptureThread"))
{
}

CRegionCaptureThread::~CRegionCaptureThread()
{
}

void CRegionCaptureThread::OnDestroy()
{
	CWnd::OnDestroy();

	WM_UNREGISTERALL();
}

LRESULT CRegionCaptureThread::OnRegionCapturePause(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();
	Pause();
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return 0;
}

LRESULT CRegionCaptureThread::OnRegionCaptureStop(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();
	Pause();
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return 0;
}

bool CRegionCaptureThread::Start()
{
	if (!GF()->CheckWindow(this))
	{
		if (!CWnd::CreateEx(WS_EX_APPWINDOW, ::AfxRegisterWndClass(NULL), NULL, WS_POPUP, CRect(0, 0, 0, 0), NULL, 0, NULL))
			return false;

		RegisterUserMessages();
	}

	if (!GF()->CheckWindow(m_region_capture_draw_region))
	{
		if (!m_region_capture_draw_region.Create())
			return false;
	}

	if (!m_work.Start(this))
		return false;

	return true;
}

bool CRegionCaptureThread::Stop()
{
	if (!GF()->CheckWindow(this) ||
		!GF()->CheckWindow(m_region_capture_draw_region))
		return false;

	return m_work.Stop() ? true : false;
}

void CRegionCaptureThread::Pause()
{
	if (!GF()->CheckWindow(this) ||
		!GF()->CheckWindow(m_region_capture_draw_region))
		return;

	m_region_capture_draw_region.Pause();
	m_work.Pause();
}

void CRegionCaptureThread::Resume()
{
	if (!GF()->CheckWindow(this) ||
		!GF()->CheckWindow(m_region_capture_draw_region))
		return;

	m_region_capture_draw_region.Resume();
	m_work.Resume();
}

void CRegionCaptureThread::RegisterUserMessages()
{
	WM_REGISTER(UM_REGION_CAPTURE_STOP);
	WM_REGISTER(UM_REGION_CAPTURE_PAUSE);
}

void CRegionCaptureThread::RegionCapture()
{
	CPoint cursor_pos;
	::GetCursorPos(&cursor_pos);

	CSize total_monitor_size = GF()->GetTotalMonitorSize();

	if (GF()->CheckWindow(m_region_capture_draw_region))
	{
		m_region_capture_draw_region.MoveWindow(0, 0, total_monitor_size.cx, total_monitor_size.cy);
		m_region_capture_draw_region.OnDraw(cursor_pos);
	}
}


CRegionCaptureThread::CWork::CWork(LPCTSTR name) : CThreadEx(name)
{
	m_region_capture_thread = NULL;
}

CRegionCaptureThread::CWork::~CWork()
{

}

bool CRegionCaptureThread::CWork::Start(CRegionCaptureThread* region_capture_thread)
{
	if (!region_capture_thread)
		return false;

	m_region_capture_thread = region_capture_thread;

	return CThreadEx::Start(m_region_capture_thread->GetSafeHwnd()) ? true : false;
}

void CRegionCaptureThread::CWork::Pause()
{
	if (!m_region_capture_thread || 
		!GF()->CheckWindow(m_region_capture_thread))
		return;

	CThreadEx::Pause();
	m_region_capture_thread->m_region_capture_draw_region.Pause();
}

void CRegionCaptureThread::CWork::Resume()
{
	if (!m_region_capture_thread || 
		!GF()->CheckWindow(m_region_capture_thread))
		return;

	CThreadEx::Resume();
	m_region_capture_thread->m_region_capture_draw_region.Resume();
}

BOOL CRegionCaptureThread::CWork::DoJob()
{
	while (!NeedToExit())
	{
		if (!m_region_capture_thread || 
			!GF()->CheckWindow(m_region_capture_thread))
		{
			Sleep(20);
			continue;
		}

		if (m_pause)
		{
			Sleep(20);
			m_region_capture_thread->m_region_capture_draw_region.Hide();
			continue;
		}

		m_region_capture_thread->RegionCapture();
		m_region_capture_thread->m_region_capture_draw_region.Show();
		Sleep(30);
	}

	return TRUE;
}
