#include "stdafx.h"
#include "TransparentEdit.h"

#define TRANSPARENT_EDIT_HITTEST_MARGIN 5

IMPLEMENT_DYNAMIC(CTransparentEdit, CEdit)

BEGIN_MESSAGE_MAP(CTransparentEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

CTransparentEdit::CTransparentEdit()
{
	m_parent_wnd = NULL;
	m_temp_memory_dc = NULL;
	m_temp_memory_bitmap = NULL;

	m_current_position = CRect(0, 0, 0, 0);
}

CTransparentEdit::~CTransparentEdit()
{
}

bool CTransparentEdit::Create(CWnd* parent, UINT id)
{
	if (!parent)
		return false;

	if (!CEdit::Create(WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | ES_MULTILINE, CRect(0, 0, 0, 0), parent, id))
		return false;

	return true;
}

void CTransparentEdit::SetParentWnd(HWND parent_wnd)
{
	m_parent_wnd = parent_wnd;
	HDC parent_dc = ::GetDC(m_parent_wnd);
	if (!m_temp_memory_dc)
	{
		::DeleteDC(m_temp_memory_dc);
		m_temp_memory_dc = NULL;
	}

	if (!m_temp_memory_bitmap)
	{
		::DeleteObject(m_temp_memory_bitmap);
		m_temp_memory_bitmap = NULL;
	}

	CRect parent_rect;
	::GetWindowRect(m_parent_wnd, parent_rect);

	m_temp_memory_dc = ::CreateCompatibleDC(parent_dc);
	m_temp_memory_bitmap = ::CreateCompatibleBitmap(parent_dc, parent_rect.Width(), parent_rect.Height());

	::SelectObject(m_temp_memory_dc, m_temp_memory_bitmap);
	::BitBlt(m_temp_memory_dc, 0, 0, parent_rect.Width(), parent_rect.Height(), parent_dc, 0, 0, SRCCOPY);
}

BOOL CTransparentEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		Invalidate();
	}

	return CEdit::PreTranslateMessage(pMsg);
}

HBRUSH CTransparentEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if (!GetSafeHwnd() || !::IsWindow(GetSafeHwnd()))
		return NULL;

	//CWnd* parent = GetParent();
	//if (!parent)
	//	return NULL;

	CWnd* parent = CWnd::FromHandle(m_parent_wnd);
	if (!parent)
		return NULL;

	CDC* parent_dc = CDC::FromHandle(m_temp_memory_dc);
	if (!parent_dc)
		return NULL;

	CRect edit_window_rect;
	GetWindowRect(edit_window_rect);

	CRect edit_client_rect;
	GetClientRect(edit_client_rect);

	CRect parent_window_rect;
	parent->GetWindowRect(parent_window_rect);

	CRect parent_client_rect;
	parent->GetClientRect(parent_client_rect);

	CRect parent_screen_rect(parent_window_rect);
	parent_screen_rect.left += ((parent_window_rect.Width() - parent_client_rect.Width()) / 2);
	parent_screen_rect.top += ((parent_window_rect.Height() - parent_client_rect.Height()) - ((parent_window_rect.Width() - parent_client_rect.Width()) / 2));

	CDC mem_dc;
	CBitmap mem_bmp;
	mem_dc.CreateCompatibleDC(parent_dc);
	mem_bmp.CreateCompatibleBitmap(parent_dc, edit_client_rect.Width(), edit_client_rect.Height());
	mem_dc.SelectObject(mem_bmp);

	int x = edit_window_rect.left - parent_screen_rect.left;
	int y = edit_window_rect.top - parent_screen_rect.top;
	mem_dc.BitBlt(
		0, 0, edit_client_rect.Width(), edit_client_rect.Height(),
		parent_dc,
		x, y,
		SRCCOPY);
	DrawBorder(mem_dc, edit_client_rect);

	HBRUSH hbr = ::CreatePatternBrush(mem_bmp);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0, 255, 0));
	pDC->BitBlt(
		0, 0, edit_client_rect.Width(), edit_client_rect.Height(),
		&mem_dc,
		0, 0,
		SRCCOPY);

	return hbr;
}


void CTransparentEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	Invalidate();

	CEdit::OnLButtonDown(nFlags, point);
}


void CTransparentEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	Invalidate();

	CEdit::OnLButtonUp(nFlags, point);
}


void CTransparentEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	Invalidate();

	CEdit::OnLButtonDblClk(nFlags, point);
}


void CTransparentEdit::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	Invalidate();

	CEdit::OnRButtonDblClk(nFlags, point);
}


void CTransparentEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	Invalidate();

	CEdit::OnRButtonDown(nFlags, point);
}


void CTransparentEdit::OnRButtonUp(UINT nFlags, CPoint point)
{
	Invalidate();

	CEdit::OnRButtonUp(nFlags, point);
}


void CTransparentEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!GF()->CheckWindow(this))
		return;

	CWnd* parent = CWnd::FromHandle(m_parent_wnd);
	if (!parent || !GF()->CheckWindow(parent))
		return;

	CPoint pos;
	GetCursorPos(&pos);
	parent->ScreenToClient(&pos);

	TRACE_POINT(pos);

	CRect screen_rect = GF()->GetScreenRect(parent, this);
	
	CRect lefttop_hittest_region(
		screen_rect.left - TRANSPARENT_EDIT_HITTEST_MARGIN, screen_rect.top - TRANSPARENT_EDIT_HITTEST_MARGIN,
		screen_rect.left + TRANSPARENT_EDIT_HITTEST_MARGIN, screen_rect.top + TRANSPARENT_EDIT_HITTEST_MARGIN);
	CRect righttop_hittest_region(
		screen_rect.right - TRANSPARENT_EDIT_HITTEST_MARGIN, screen_rect.top - TRANSPARENT_EDIT_HITTEST_MARGIN,
		screen_rect.right + TRANSPARENT_EDIT_HITTEST_MARGIN, screen_rect.top + TRANSPARENT_EDIT_HITTEST_MARGIN);
	CRect leftbottom_hittest_region(
		screen_rect.left - TRANSPARENT_EDIT_HITTEST_MARGIN, screen_rect.bottom - TRANSPARENT_EDIT_HITTEST_MARGIN,
		screen_rect.left + TRANSPARENT_EDIT_HITTEST_MARGIN, screen_rect.bottom + TRANSPARENT_EDIT_HITTEST_MARGIN);
	CRect rightbottom_hittest_region(
		screen_rect.right - TRANSPARENT_EDIT_HITTEST_MARGIN, screen_rect.bottom - TRANSPARENT_EDIT_HITTEST_MARGIN,
		screen_rect.right + TRANSPARENT_EDIT_HITTEST_MARGIN, screen_rect.bottom + TRANSPARENT_EDIT_HITTEST_MARGIN);
	CRect left_hittest_region(
		lefttop_hittest_region.left, lefttop_hittest_region.bottom,
		leftbottom_hittest_region.right, leftbottom_hittest_region.top);
	CRect top_hittest_region(
		lefttop_hittest_region.right, lefttop_hittest_region.top,
		righttop_hittest_region.left, righttop_hittest_region.bottom);
	CRect right_hittest_region(
		righttop_hittest_region.left, righttop_hittest_region.bottom,
		rightbottom_hittest_region.right, rightbottom_hittest_region.top);
	CRect bottom_hittest_region(
		leftbottom_hittest_region.right, leftbottom_hittest_region.top,
		rightbottom_hittest_region.left, rightbottom_hittest_region.bottom);

	if (lefttop_hittest_region.PtInRect(pos) ||
		righttop_hittest_region.PtInRect(pos) ||
		leftbottom_hittest_region.PtInRect(pos) ||
		rightbottom_hittest_region.PtInRect(pos) ||
		left_hittest_region.PtInRect(pos) ||
		top_hittest_region.PtInRect(pos) ||
		right_hittest_region.PtInRect(pos) ||
		bottom_hittest_region.PtInRect(pos))
		::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
	else
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));


	Invalidate();

	CEdit::OnMouseMove(nFlags, point);
}

void CTransparentEdit::OnMouseLeave()
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	CEdit::OnMouseLeave();
}

void CTransparentEdit::OnDestroy()
{
	CEdit::OnDestroy();

	if (!m_temp_memory_dc)
	{
		::DeleteDC(m_temp_memory_dc);
		m_temp_memory_dc = NULL;
	}

	if (!m_temp_memory_bitmap)
	{
		::DeleteObject(m_temp_memory_bitmap);
		m_temp_memory_bitmap = NULL;
	}
}

void CTransparentEdit::OnMove(int x, int y)
{
	CEdit::OnMove(x, y);

	if (!GF()->CheckWindow(this))
		return;

	CRect client_rect;
	GetClientRect(client_rect);

	m_current_position = CRect(x, y, x + client_rect.Width(), y + client_rect.Height());
	TRACE_RECT2(m_current_position, _T("m_current_position"));
}

void CTransparentEdit::DrawBorder(CDC& mem_dc, CRect& client_rect)
{
	CPen pen(PS_DOT, 1, RGB(66, 66, 66));

	CRect border_rect(client_rect);
	//border_rect.InflateRect(1, 1);
	border_rect.right -= 1;
	border_rect.bottom -= 1;

	mem_dc.SelectObject(pen);

	mem_dc.MoveTo(border_rect.left, border_rect.top);
	mem_dc.LineTo(border_rect.right, border_rect.top);

	mem_dc.MoveTo(border_rect.right, border_rect.top);
	mem_dc.LineTo(border_rect.right, border_rect.bottom);

	mem_dc.MoveTo(border_rect.right, border_rect.bottom);
	mem_dc.LineTo(border_rect.left, border_rect.bottom);

	mem_dc.MoveTo(border_rect.left, border_rect.bottom);
	mem_dc.LineTo(border_rect.left, border_rect.top);
}