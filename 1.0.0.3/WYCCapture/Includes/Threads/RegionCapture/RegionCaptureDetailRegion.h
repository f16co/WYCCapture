#pragma once
#include <afxwin.h>
#include "WMTarget.h"

class CRegionCaptureDetailRegion : public CWnd, CWMTarget
{
	DECLARE_DYNAMIC(CRegionCaptureDetailRegion)

public:
	CRegionCaptureDetailRegion();
	virtual ~CRegionCaptureDetailRegion();

	void SetRegionRect(CRect& region_rect) { m_region_rect = region_rect; }

	bool Create(CWnd* parent, UINT id);
	void OnDraw(HDC current_dc, CPoint current_pos);

	void Show();
	void Hide();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();

	void Draw();
	void DrawText(CDC& mem_dc, CRect& client_rect);
	void DrawCrossLine(CDC& mem_dc, CPoint& point, int left, int top, int right, int bottom);
	void MoveWnd();

	CRect GetDetailRegionRect(int monitor_index, int quadrant_index);

	virtual void RegisterUserMessages();

private:
	CPoint m_current_pos;
	HDC m_current_dc;

	CRect m_region_rect;
};