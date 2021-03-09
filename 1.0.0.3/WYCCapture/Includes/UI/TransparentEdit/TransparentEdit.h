#pragma once
#include <afxwin.h>

class CTransparentEdit : public CEdit
{
	DECLARE_DYNAMIC(CTransparentEdit)

public:
	CTransparentEdit();
	virtual ~CTransparentEdit();

	bool Create(CWnd* parent, UINT id);
	void SetParentWnd(HWND parent_wnd);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);

private:
	void DrawBorder(CDC& mem_dc, CRect& client_rect);

	HWND m_parent_wnd;
	HDC m_temp_memory_dc;
	HBITMAP m_temp_memory_bitmap;

	CRect m_current_position;
};

