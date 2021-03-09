#pragma once
#include <afxwin.h>
#include "WMTarget.h"

class CWindowCaptureDetailRegion : public CWnd, CWMTarget
{
	DECLARE_DYNAMIC(CWindowCaptureDetailRegion)

public:
	CWindowCaptureDetailRegion();
	virtual ~CWindowCaptureDetailRegion();

	void SetRegionRect(CRect& region_rect) { m_region_rect = region_rect; }

	bool Create(CWnd* parent, UINT id);
	void OnDraw(HDC current_dc, CPoint& current_pos, CRect& current_rect);

	void Show();
	void Hide();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	void Draw();
	void DrawText(CDC& mem_dc, CRect& client_rect);
	void MoveWnd();

	CRect GetDetailRegionRect(int monitor_index, int quadrant_index);

	virtual void RegisterUserMessages();

private:
	CPoint m_current_pos;
	CRect m_current_rect;
	HDC m_current_dc;

	CRect m_region_rect;
};