#pragma once
#include <afxwin.h>
#include <vector>
#include "ShortcutTarget.h"
#include "WMTarget.h"
#include "TransparentEdit.h"

//#define DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH 40

class CCapturedRegion : public CWnd, CShortcutTarget, CWMTarget
{
	DECLARE_DYNAMIC(CCapturedRegion)

public:
	CCapturedRegion(int id);
	virtual ~CCapturedRegion();

	bool Create();

	void SetStartPoint(CPoint& start_point) { m_start_point = start_point; }
	void SetEndPoint(CPoint& end_point) { m_end_point = end_point; }

	void Capture(HDC hdc, int current_monitor_index);

	COLORREF GetPixelValue(int x, int y);

	void Reset();
	void Erase();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnChangeColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShortcutGenerated(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShortcutChanged(WPARAM wParam, LPARAM lParam);

	virtual void InitShortcutTarget();
	virtual void DeleteShortcuts();
	virtual void InitShortcuts();
	virtual void RegisterUserMessages();

	void Draw();
	//void DrawText(CDC& mem_dc, CRect& client_rect);
	void DrawCustomLine(HDC mem_dc, CRect& client_rect);
	void DrawCustomErase(HDC mem_dc, CRect& client_rect);

	Gdiplus::Color ToGdiplusColor(COLORREF color);
	Gdiplus::Rect ToGdiplusRect(CRect rect);

private:
	enum CURRENT_CUSTOM_MODE { CUSTOM_MODE_NONE, CUSTOM_MODE_DRAW, CUSTOM_MODE_ERASE };

	static void OnShortcutSaveTheOriginalImage(HWND hwnd);
	static void OnShortcutSaveTheOverlayImage(HWND hwnd);
	static void OnShortcutErase(HWND hwnd);
	static void OnShortcutReset(HWND hwnd);
	static void OnShortcutCopyTheOriginalImageToClipboard(HWND hwnd);
	static void OnShortcutCopyTheOverlayImageToClipboard(HWND hwnd);
	static void OnShortcutESC(HWND hwnd);

	bool InitTooltip();

	void CopyToClipboard(bool is_original_image);

	void Activated();
	void Inactivated();

	void MakePopupMenu();
	void DestroyPopupMenu();
	void OnContextMenuChangePenWidth(UINT id);
	void OnContextMenuChangePenColor();
	//void OnContextMenuInsertText();
	void OnContextMenuSaveTheOriginalImage();
	void OnContextMenuSaveTheOverlayImage();
	void OnContextMenuCopyTheOriginalImageToClipboard();
	void OnContextMenuCopyTheOverlayImageToClipboard();
	void OnContextMenuErase();
	void OnContextMenuReset();
	void OnContextMenuClose();

	void OnWheelDown();
	void OnWheelUp();

	void Close();

	bool SaveFileDlgOpen(CString* file_path);
	void SaveFile(bool is_original_image);

	CImage m_original_image;
	CImage m_overlay_image;

	CPoint m_start_point;
	CPoint m_end_point;

	HMENU m_context_menu;
	HMENU m_pen_width_menu;

	CString m_last_save_folder_dir;

	bool m_is_activated;
	//bool m_is_custom_draw;
	//bool m_is_custom_erase;
	CURRENT_CUSTOM_MODE m_current_custom_mode;
	CPoint m_prev_custom_draw_point;
	CPoint m_next_custom_draw_point;
	COLORREF m_custom_draw_pen_color;
	int m_custom_draw_pen_width;

	int m_id;

	int m_default_wheel_offset_x;
	int m_default_wheel_offset_y;
	int m_default_min_width;
	int m_default_min_height;

	CToolTipCtrl m_tool_tip;

	//CTransparentEdit m_edit;

	int GetGCD(int a, int b);
	void CalculateDefaultWheelOffset(CRect rect);
	void InitPoints();
	bool IsCustomDrawing();
	bool IsCustomErasing();
};