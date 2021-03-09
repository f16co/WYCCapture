#pragma once
#include "WindowCaptureDrawRegion.h"
#include "ThreadEx.h"
#include "WMTarget.h"

class CWindowCaptureThread : public CWnd, CWMTarget
{
	DECLARE_DYNAMIC(CWindowCaptureThread)

public:
	CWindowCaptureThread();
	virtual ~CWindowCaptureThread();

	virtual bool Start();
	virtual bool Stop();

	virtual void Pause();
	virtual void Resume();

	virtual bool IsPause() { return m_work.IsPause(); }
	virtual bool IsRunning() { return m_work.IsRunning(); }

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg LRESULT OnWindowCapturePause(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWindowCaptureStop(WPARAM wParam, LPARAM lParam);

	virtual void RegisterUserMessages();

private:
	class CWork : public CThreadEx
	{
	public:
		CWork(LPCTSTR name);
		virtual ~CWork();

		virtual bool Start(CWindowCaptureThread* window_capture_thread);
		virtual void Pause();
		virtual void Resume();

	protected:
		virtual BOOL DoJob();

	private:
		CWindowCaptureThread* m_window_capture_thread;
	};

	void WindowCapture();

	CWindowCaptureDrawRegion m_window_capture_draw_region;
	CWork m_work;
};