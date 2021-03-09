#pragma once
#include "WMTarget.h"
#include "CapturedRegion.h"
#include "SafeQueue.h"
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

	CRITICAL_SECTION m_cs_captured_region;
	std::map<int, CCapturedRegion*> m_captured_region_list;

	bool m_is_capturing;

	CSafeQueue m_signaling_data_queue;

	int m_last_captured_region_id;
};