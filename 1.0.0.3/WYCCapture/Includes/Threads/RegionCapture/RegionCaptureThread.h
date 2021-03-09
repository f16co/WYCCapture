#pragma once
#include "RegionCaptureDrawRegion.h"
#include "ThreadEx.h"
#include "WMTarget.h"

class CRegionCaptureThread : public CWnd, CWMTarget
{
	DECLARE_DYNAMIC(CRegionCaptureThread)

public:
	CRegionCaptureThread();
	virtual ~CRegionCaptureThread();

	virtual bool Start();
	virtual bool Stop();

	virtual void Pause();
	virtual void Resume();

	virtual bool IsPause() { return m_work.IsPause(); }
	virtual bool IsRunning() { return m_work.IsRunning(); }

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg LRESULT OnRegionCapturePause(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRegionCaptureStop(WPARAM wParam, LPARAM lParam);

	virtual void RegisterUserMessages();

private:
	class CWork : public CThreadEx
	{
	public:
		CWork(LPCTSTR name);
		virtual ~CWork();

		virtual bool Start(CRegionCaptureThread* region_capture_thread);
		virtual void Pause();
		virtual void Resume();

	protected:
		virtual BOOL DoJob();

	private:
		CRegionCaptureThread* m_region_capture_thread;
	};

	void RegionCapture();

	CRegionCaptureDrawRegion m_region_capture_draw_region;
	CWork m_work;
};