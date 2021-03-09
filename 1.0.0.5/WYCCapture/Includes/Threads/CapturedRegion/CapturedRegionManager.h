#pragma once
#include "WMTarget.h"
#include "CapturedRegion.h"
#include "UserMessages.h"
#include <map>

class CCapturedRegionManager : public CWnd, CWMTarget
{
	DECLARE_DYNAMIC(CCapturedRegionManager)

public:
	CCapturedRegionManager();
	virtual ~CCapturedRegionManager();

	bool Create();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg LRESULT OnCapturedRegionDelete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRegionCaptureStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRegionCaptureStop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRegionCapturePause(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWindowCaptureStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWindowCaptureStop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWindowCapturePause(WPARAM wParam, LPARAM lParam);

private:
	void RegisterUserMessages();
	int AllocateCapturedRegionID();
	bool IsValidCaputredRegionID(int captured_region_id);

	void DisposeCaptureRegionList();

	void PushRegionCapture(REGION_CAPTURE_STOP& region_capture_stop);
	REGION_CAPTURE_STOP PopRegionCapture();

	void PushWindowCapture(WINDOW_CAPTURE_STOP& window_capture_stop);
	WINDOW_CAPTURE_STOP PopWindowCapture();

	CRITICAL_SECTION m_cs_captured_region;
	std::map<int, CCapturedRegion*> m_captured_region_list;

	bool m_is_capturing;

	CRITICAL_SECTION m_cs_region_capture_stop_msg_list;
	std::vector<REGION_CAPTURE_STOP> m_region_capture_stop_msg_list;

	CRITICAL_SECTION m_cs_window_capture_stop_msg_list;
	std::vector<WINDOW_CAPTURE_STOP> m_window_capture_stop_msg_list;

	int m_last_captured_region_id;
};