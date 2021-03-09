#include "stdafx.h"
#include "CapturedRegionManager.h"

IMPLEMENT_DYNAMIC(CCapturedRegionManager, CWnd)

BEGIN_MESSAGE_MAP(CCapturedRegionManager, CWnd)
	ON_WM_DESTROY()
	ON_MESSAGE(UM_CAPTURED_REGION_DELETE, OnCapturedRegionDelete)
	ON_MESSAGE(UM_REGION_CAPTURE_START, OnRegionCaptureStart)
	ON_MESSAGE(UM_REGION_CAPTURE_STOP, OnRegionCaptureStop)
	ON_MESSAGE(UM_REGION_CAPTURE_PAUSE, OnRegionCapturePause)
	ON_MESSAGE(UM_WINDOW_CAPTURE_START, OnWindowCaptureStart)
	ON_MESSAGE(UM_WINDOW_CAPTURE_STOP, OnWindowCaptureStop)
	ON_MESSAGE(UM_WINDOW_CAPTURE_PAUSE, OnWindowCapturePause)
END_MESSAGE_MAP()

CCapturedRegionManager::CCapturedRegionManager()
	: m_is_capturing(false)
{
	CScopeLock::InitCS(&m_cs_captured_region);
	CScopeLock::InitCS(&m_cs_region_capture_stop_msg_list);
	CScopeLock::InitCS(&m_cs_window_capture_stop_msg_list);
	m_last_captured_region_id = -1;
}

CCapturedRegionManager::~CCapturedRegionManager()
{
	DisposeCaptureRegionList();
	CScopeLock::DeleteCS(&m_cs_captured_region);
	CScopeLock::DeleteCS(&m_cs_region_capture_stop_msg_list);
	CScopeLock::DeleteCS(&m_cs_window_capture_stop_msg_list);
}

bool CCapturedRegionManager::Create()
{
	if (!CWnd::CreateEx(WS_EX_PALETTEWINDOW, ::AfxRegisterWndClass(NULL), NULL, WS_VISIBLE | WS_POPUP, CRect(0, 0, 0, 0), NULL, 0, NULL))
		return false;

	RegisterUserMessages();

	return true;
}

void CCapturedRegionManager::OnDestroy()
{
	CWnd::OnDestroy();

	WM_UNREGISTERALL();
}

LRESULT CCapturedRegionManager::OnCapturedRegionDelete(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();

	int captured_region_id = (int)wParam;
	if (!IsValidCaputredRegionID(captured_region_id))
		return -1;

	CScopeLock lock(&m_cs_captured_region);
	std::map<int, CCapturedRegion*>::iterator find_result = m_captured_region_list.find(captured_region_id);

	if (find_result != m_captured_region_list.end())
	{
		if (find_result->second)
		{
			CCapturedRegion* captured_region = find_result->second;
			m_captured_region_list.erase(find_result);
			if (GF()->CheckWindow(captured_region) && captured_region->DestroyWindow())
			{
				TRACE(_T("id(%d) is deleted\n"), captured_region_id);
				delete captured_region;
				captured_region = NULL;
			}
		}
	}

	return 0;
}

LRESULT CCapturedRegionManager::OnRegionCaptureStart(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();
	m_is_capturing = true;
	return 0;
}

LRESULT CCapturedRegionManager::OnRegionCaptureStop(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();
	if (wParam)
	{
		LPREGION_CAPTURE_STOP pregion_capture_stop = (LPREGION_CAPTURE_STOP)wParam;
		if (pregion_capture_stop->IsValid())
		{
			PushRegionCapture(*pregion_capture_stop);
			PostMessage(UM_REGION_CAPTURE_STOP);
		}
	}
	else
	{
		if (m_is_capturing)
		{
			REGION_CAPTURE_STOP region_capture_stop = PopRegionCapture();
			if (region_capture_stop.IsValid())
			{
				CPoint start_pos = region_capture_stop.start_pos;
				CPoint end_pos = region_capture_stop.end_pos;
				HDC current_dc = region_capture_stop.current_dc;
				int current_monitor_index = region_capture_stop.current_monitor_index;

				int captured_region_id = AllocateCapturedRegionID();
				CCapturedRegion* captured_region = new CCapturedRegion(captured_region_id);

				if (true)
				{
					CScopeLock lock(&m_cs_captured_region);
					m_captured_region_list[captured_region_id] = captured_region;
				}

				captured_region->SetStartPoint(start_pos);
				captured_region->SetEndPoint(end_pos);
				if (!captured_region->Create())
					return -1;

				//TRACE_DC2(current_dc, GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN), _T("test.jpeg"));

				captured_region->Capture(current_dc, current_monitor_index);
			}
		}
	}

	return 0;
}

LRESULT CCapturedRegionManager::OnRegionCapturePause(WPARAM wParam, LPARAM lParam)
{
	if (m_is_capturing)
		m_is_capturing = false;

	return 0;
}

LRESULT CCapturedRegionManager::OnWindowCaptureStart(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();
	m_is_capturing = true;
	return 0;
}

LRESULT CCapturedRegionManager::OnWindowCaptureStop(WPARAM wParam, LPARAM lParam)
{
	TRACE_JUST();
	if (wParam)
	{
		LPWINDOW_CAPTURE_STOP pwindow_capture_stop = (LPWINDOW_CAPTURE_STOP)wParam;
		if (pwindow_capture_stop->IsValid())
		{
			PushWindowCapture(*pwindow_capture_stop);
			PostMessage(UM_WINDOW_CAPTURE_STOP);
		}
	}
	else
	{
		if (m_is_capturing)
		{
			WINDOW_CAPTURE_STOP window_capture_stop = PopWindowCapture();
			if (window_capture_stop.IsValid())
			{
				CRect rect = window_capture_stop.captured_region;
				HDC current_dc = window_capture_stop.current_dc;
				int current_monitor_index = window_capture_stop.current_monitor_index;
				TRACE_RECT(rect);

				int captured_region_id = AllocateCapturedRegionID();
				CCapturedRegion* captured_region = new CCapturedRegion(captured_region_id);

				if (true)
				{
					CScopeLock lock(&m_cs_captured_region);
					m_captured_region_list[captured_region_id] = captured_region;
				}

				CPoint start_point(rect.left, rect.top);
				CPoint end_point(rect.right, rect.bottom);

				captured_region->SetStartPoint(start_point);
				captured_region->SetEndPoint(end_point);
				if (!captured_region->Create())
					return -1;

				captured_region->Capture(current_dc, current_monitor_index);
			}
		}
	}

	//if (pwindow_capture_stop &&
	//	pwindow_capture_stop->IsValid())
	//{
	//	if (m_is_capturing)
	//	{
	//		CRect rect = pwindow_capture_stop->captured_region;
	//		HDC current_dc = pwindow_capture_stop->current_dc;
	//		int current_monitor_index = pwindow_capture_stop->current_monitor_index;
	//		TRACE_RECT(rect);

	//		int captured_region_id = AllocateCapturedRegionID();
	//		CCapturedRegion* captured_region = new CCapturedRegion(captured_region_id);
	//		
	//		if (true)
	//		{
	//			CScopeLock lock(&m_cs_captured_region);
	//			m_captured_region_list[captured_region_id] = captured_region;
	//		}

	//		CPoint start_point(rect.left, rect.top);
	//		CPoint end_point(rect.right, rect.bottom);

	//		captured_region->SetStartPoint(start_point);
	//		captured_region->SetEndPoint(end_point);
	//		if (!captured_region->Create())
	//			return -1;

	//		captured_region->Capture(current_dc, current_monitor_index);
	//	}
	//}
	//SAFE_DELETE(pwindow_capture_stop);

	return 0;
}

LRESULT CCapturedRegionManager::OnWindowCapturePause(WPARAM wParam, LPARAM lParam)
{
	if (m_is_capturing)
		m_is_capturing = false;

	return 0;
}

void CCapturedRegionManager::RegisterUserMessages()
{
	WM_REGISTER(UM_CAPTURED_REGION_DELETE);
	WM_REGISTER(UM_REGION_CAPTURE_START);
	WM_REGISTER(UM_REGION_CAPTURE_STOP);
	WM_REGISTER(UM_REGION_CAPTURE_PAUSE);
	WM_REGISTER(UM_WINDOW_CAPTURE_START);
	WM_REGISTER(UM_WINDOW_CAPTURE_STOP);
	WM_REGISTER(UM_WINDOW_CAPTURE_PAUSE);
}

int CCapturedRegionManager::AllocateCapturedRegionID()
{
	// 0부터 INT_MAX - 1까지의 범위
	if (0 <= m_last_captured_region_id && m_last_captured_region_id < INT_MAX)
		m_last_captured_region_id++;
	else
		m_last_captured_region_id = 0;

	return m_last_captured_region_id;
}

bool CCapturedRegionManager::IsValidCaputredRegionID(int captured_region_id)
{
	return (0 <= captured_region_id && captured_region_id < INT_MAX);
}

void CCapturedRegionManager::DisposeCaptureRegionList()
{
	CScopeLock lock(&m_cs_captured_region);
	std::map<int, CCapturedRegion*>::iterator pos = m_captured_region_list.begin();
	while (pos != m_captured_region_list.end())
	{
		if (pos->second)
		{
			delete pos->second;
			pos->second = NULL;
		}
		pos++;
	}
}

void CCapturedRegionManager::PushRegionCapture(REGION_CAPTURE_STOP& region_capture_stop)
{
	CScopeLock lock(&m_cs_region_capture_stop_msg_list);
	m_region_capture_stop_msg_list.push_back(region_capture_stop);
}

REGION_CAPTURE_STOP CCapturedRegionManager::PopRegionCapture()
{
	CScopeLock lock(&m_cs_region_capture_stop_msg_list);
	if (m_region_capture_stop_msg_list.size() == 0)
		return REGION_CAPTURE_STOP();

	REGION_CAPTURE_STOP region_capture_stop = *m_region_capture_stop_msg_list.begin();
	m_region_capture_stop_msg_list.erase(m_region_capture_stop_msg_list.begin());

	return region_capture_stop;
}

void CCapturedRegionManager::PushWindowCapture(WINDOW_CAPTURE_STOP& window_capture_stop)
{
	CScopeLock lock(&m_cs_window_capture_stop_msg_list);
	m_window_capture_stop_msg_list.push_back(window_capture_stop);
}

WINDOW_CAPTURE_STOP CCapturedRegionManager::PopWindowCapture()
{
	CScopeLock lock(&m_cs_window_capture_stop_msg_list);
	if (m_window_capture_stop_msg_list.size() == 0)
		return WINDOW_CAPTURE_STOP();
	
	WINDOW_CAPTURE_STOP window_capture_stop = *m_window_capture_stop_msg_list.begin();
	m_window_capture_stop_msg_list.erase(m_window_capture_stop_msg_list.begin());

	return window_capture_stop;
}
