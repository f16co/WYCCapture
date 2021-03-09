#include "stdafx.h"
#include "RegionCaptureDrawRegion.h"
#include "WndManager.h"

IMPLEMENT_DYNAMIC(CRegionCaptureDrawRegion, CWnd)

BEGIN_MESSAGE_MAP(CRegionCaptureDrawRegion, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_SHORTCUT_GENERATED, OnShortcutGenerated)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CRegionCaptureDrawRegion::CRegionCaptureDrawRegion()
	: m_is_shown(false)
	, m_is_first_time(true)
	, m_is_cliecked(false)
	, m_is_pause(true)
	, m_temp_memory_bitmap(NULL)
	, m_temp_memory_dc(NULL)
{
	SetShortcutTargetName(_T("CRegionCaptureDrawRegion"));
}

CRegionCaptureDrawRegion::~CRegionCaptureDrawRegion()
{
}

bool CRegionCaptureDrawRegion::Create()
{
	if (!CWnd::CreateEx(WS_EX_PALETTEWINDOW, ::AfxRegisterWndClass(NULL), _T("CRegionCaptureDrawRegion"), WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), NULL, 0, NULL))
		return false;

	CString window_name;
	GetWindowText(window_name);
	CWndManager::GetSingletone()->AddExceptionList(window_name);

	RegisterUserMessages();

	if (!m_region_capture_detail_region.Create(this, IDC_REGION_CAPTURE_DETAIL_REGION))
		return false;

	InitShortcuts();
	InitShortcutTarget();

	return true;
}

void CRegionCaptureDrawRegion::OnDraw(CPoint current_pos)
{
	if (!GF()->CheckWindow(GetSafeHwnd()))
		return;

	m_current_pos = current_pos;
	Invalidate();
}

void CRegionCaptureDrawRegion::Show()
{
	if (!GF()->CheckWindow(GetSafeHwnd()))
		return;

	if (!m_is_shown)
	{
		TRACE_JUST();
		ShowWindow(SW_SHOW);
		m_is_shown = true;
	}
}

void CRegionCaptureDrawRegion::Hide()
{
	if (!GF()->CheckWindow(GetSafeHwnd()))
		return;

	if (m_is_shown)
	{
		TRACE_JUST();
		ShowWindow(SW_HIDE);
		m_is_shown = false;
		if (!m_is_first_time)
		{
			::DeleteDC(m_temp_memory_dc);
			::DeleteObject(m_temp_memory_bitmap);
			m_is_first_time = true;
		}
		m_region_capture_detail_region.SetRegionRect(CRect(0, 0, 0, 0));
	}

	if (m_is_cliecked)
		m_is_cliecked = false;
}

BOOL CRegionCaptureDrawRegion::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam == VK_F4)
		{
			return TRUE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CRegionCaptureDrawRegion::OnPaint()
{
	if (!m_is_shown)
		return;

	CPaintDC dc(this);

	CRect client_rect;
	GetClientRect(&client_rect);

	//CPerformanceChecker checker(_T("CRegionCaptureDrawRegion::OnPaint"));

	CDC mem_dc;
	CBitmap mem_bmp;
	mem_dc.CreateCompatibleDC(&dc);
	mem_bmp.CreateCompatibleBitmap(&dc, client_rect.Width(), client_rect.Height());
	mem_dc.SelectObject(mem_bmp);

	CPen pen(PS_SOLID, 1, RGB(255, 0, 255));
	mem_dc.SelectObject(pen);

	CRect monitor_rect = client_rect;

	CDC* monitor_dc = CDC::FromHandle(GF()->GetDesktopDC());
	
	bool is_valid = DrawFirstTime(dc, *monitor_dc, client_rect, monitor_rect);
	DrawDesktopBackground(mem_dc, client_rect);
	DrawCrossLine(mem_dc, m_current_pos);
	DrawSelectedRegion(dc, mem_dc, monitor_rect);

	DrawToThis(dc, mem_dc, client_rect);
	if (is_valid)
		DrawBackToMonitorDC(*monitor_dc, monitor_rect);

	m_region_capture_detail_region.OnDraw(m_temp_memory_dc, m_current_pos);
}

void CRegionCaptureDrawRegion::OnDestroy()
{
	CWnd::OnDestroy();

	DeleteShortcuts();
}

void CRegionCaptureDrawRegion::OnShortcutLeft(HWND hwnd)
{
	if (!hwnd)
		return;

	CRegionCaptureDrawRegion* main = reinterpret_cast<CRegionCaptureDrawRegion*>(CWnd::FromHandle(hwnd));

	main->OnLeft();
}

void CRegionCaptureDrawRegion::OnShortcutUp(HWND hwnd)
{
	if (!hwnd)
		return;

	CRegionCaptureDrawRegion* main = reinterpret_cast<CRegionCaptureDrawRegion*>(CWnd::FromHandle(hwnd));

	main->OnUp();
}

void CRegionCaptureDrawRegion::OnShortcutRight(HWND hwnd)
{
	if (!hwnd)
		return;

	CRegionCaptureDrawRegion* main = reinterpret_cast<CRegionCaptureDrawRegion*>(CWnd::FromHandle(hwnd));

	main->OnRight();
}

void CRegionCaptureDrawRegion::OnShortcutDown(HWND hwnd)
{
	if (!hwnd)
		return;

	CRegionCaptureDrawRegion* main = reinterpret_cast<CRegionCaptureDrawRegion*>(CWnd::FromHandle(hwnd));

	main->OnDown();
}

void CRegionCaptureDrawRegion::OnLeft()
{
	m_current_pos.x -= 1;
	SetCursorPos(m_current_pos.x, m_current_pos.y);
}

void CRegionCaptureDrawRegion::OnRight()
{
	m_current_pos.x += 1;
	SetCursorPos(m_current_pos.x, m_current_pos.y);
}

void CRegionCaptureDrawRegion::OnUp()
{
	m_current_pos.y -= 1;
	SetCursorPos(m_current_pos.x, m_current_pos.y);
}

void CRegionCaptureDrawRegion::OnDown()
{
	m_current_pos.y += 1;
	SetCursorPos(m_current_pos.x, m_current_pos.y);
}

void CRegionCaptureDrawRegion::DrawCrossLine(CDC& mem_dc, CPoint pos)
{
	CRect client_rect;
	GetClientRect(client_rect);
	ScreenToClient(&pos);

	CPoint point1, point2;
	
	point1 = CPoint(client_rect.left, pos.y);
	point2 = CPoint(client_rect.right, pos.y);
	mem_dc.MoveTo(point1);
	mem_dc.LineTo(point2);

	point1 = CPoint(pos.x, client_rect.top);
	point2 = CPoint(pos.x, client_rect.bottom);
	mem_dc.MoveTo(point1);
	mem_dc.LineTo(point2);
}

void CRegionCaptureDrawRegion::DrawDesktopBackground(CDC& mem_dc, CRect& client_rect)
{
	::BitBlt(
		mem_dc,
		0, 0, client_rect.Width(), client_rect.Height(),
		m_temp_memory_dc,
		0, 0,
		SRCCOPY);
}

void CRegionCaptureDrawRegion::DrawSelectedRegion(CDC& this_dc, CDC& mem_dc, CRect& monitor_rect)
{
	if (m_is_cliecked)
	{
		int left = min(m_clicked_pos.x, m_current_pos.x) - monitor_rect.left;
		int top = min(m_clicked_pos.y, m_current_pos.y) - monitor_rect.top;
		int right = max(m_clicked_pos.x, m_current_pos.x) - monitor_rect.left;
		int bottom = max(m_clicked_pos.y, m_current_pos.y) - monitor_rect.top;
		CRect region_rect(left, top, right, bottom);

		m_region_capture_detail_region.SetRegionRect(region_rect);

		Gdiplus::Graphics memG(mem_dc);
		Gdiplus::SolidBrush selected_region_brush(Gdiplus::Color(55, 12, 199, 55));
		Gdiplus::Pen selected_region_pen(Gdiplus::Color(255, 66, 66, 66));
		selected_region_pen.SetDashStyle(Gdiplus::DashStyleDot);

		memG.FillRectangle(&selected_region_brush, Gdiplus::Rect(region_rect.left + 1, region_rect.top + 1, region_rect.Width() - 2, region_rect.Height() - 2));
		memG.DrawRectangle(&selected_region_pen, Gdiplus::Rect(region_rect.left, region_rect.top, region_rect.Width(), region_rect.Height()));
	}
}

bool CRegionCaptureDrawRegion::DrawFirstTime(CDC& this_dc, CDC& monitor_dc, CRect& client_rect, CRect& monitor_rect)
{
	bool is_valid = false;
	if (m_is_first_time)
	{
		m_temp_memory_dc = ::CreateCompatibleDC(this_dc);
		m_temp_memory_bitmap = ::CreateCompatibleBitmap(this_dc, client_rect.Width(), client_rect.Height());
		::SelectObject(m_temp_memory_dc, m_temp_memory_bitmap);
		::StretchBlt(
			m_temp_memory_dc,
			0, 0, client_rect.Width(), client_rect.Height(),
			monitor_dc,
			monitor_rect.left, monitor_rect.top, monitor_rect.Width(), monitor_rect.Height(),
			SRCCOPY);
		m_is_first_time = false;
		is_valid = true;
	}

	return is_valid;
}

void CRegionCaptureDrawRegion::DrawToThis(CDC& this_dc, CDC& mem_dc, CRect& client_rect)
{
	this_dc.BitBlt(
		0, 0, client_rect.Width(), client_rect.Height(),
		&mem_dc,
		0, 0,
		SRCCOPY);
}

void CRegionCaptureDrawRegion::DrawBackToMonitorDC(CDC& monitor_dc, CRect& monitor_rect)
{
	::BitBlt(
		monitor_dc,
		0, 0, monitor_rect.Width(), monitor_rect.Height(),
		m_temp_memory_dc,
		0, 0,
		SRCCOPY);
}

void CRegionCaptureDrawRegion::OnLButtonDown(UINT nFlags, CPoint point)
{
	TRACE_JUST();
	CPoint pos;
	::GetCursorPos(&pos);
	m_clicked_pos = pos;

	m_is_cliecked = true;
	WM_POST_MESSAGE(UM_REGION_CAPTURE_START);

	CWnd::OnLButtonDown(nFlags, point);
}

void CRegionCaptureDrawRegion::OnLButtonUp(UINT nFlags, CPoint point)
{
	TRACE_JUST();

	m_is_cliecked = false;

	int left = min(m_clicked_pos.x, m_current_pos.x);
	int top = min(m_clicked_pos.y, m_current_pos.y);
	int right = max(m_clicked_pos.x, m_current_pos.x);
	int bottom = max(m_clicked_pos.y, m_current_pos.y);
	CRect region_rect(left, top, right, bottom);

	if (region_rect.Width() <= 0 || region_rect.Height() <= 0)
		return;

	LPREGION_CAPTURE_STOP pregion_capture_stop = new REGION_CAPTURE_STOP(m_clicked_pos, m_current_pos, m_temp_memory_dc, 0);
	WM_POST_MESSAGE1(UM_REGION_CAPTURE_STOP, pregion_capture_stop);

	CWnd::OnLButtonUp(nFlags, point);
}

LRESULT CRegionCaptureDrawRegion::OnShortcutGenerated(WPARAM wParam, LPARAM lParam)
{
	if (m_is_pause)
		return -1;

	DWORD shortcut_code = (DWORD)wParam;
	int shortcut_id = (DWORD)lParam;
	SHORTCUT_DATA shortcut_data = CShortcutManager::GetSingletone()->ParseShortcutCode(shortcut_code);
	bool ctrl = shortcut_data.ctrl;
	bool alt = shortcut_data.alt;
	bool shift = shortcut_data.shift;
	BYTE key = shortcut_data.key;

	if (shortcut_id >= 0)
	{
		SHORTCUT_FUNC shortcut_func = NULL;
		EnterCriticalSection(&m_cs_shortcut_func);
		std::map<int, SHORTCUT_FUNC>::iterator pos = m_map_shortcut_func.find(shortcut_id);
		if (pos != m_map_shortcut_func.end())
			shortcut_func = pos->second;
		LeaveCriticalSection(&m_cs_shortcut_func);

		if (shortcut_func)
			shortcut_func(GetSafeHwnd());
	}

	return 0;
}

void CRegionCaptureDrawRegion::InitShortcutTarget()
{
	EnterCriticalSection(&m_cs_shortcut_func);
	m_map_shortcut_func[0] = OnShortcutLeft;
	m_map_shortcut_func[1] = OnShortcutUp;
	m_map_shortcut_func[2] = OnShortcutRight;
	m_map_shortcut_func[3] = OnShortcutDown;
	LeaveCriticalSection(&m_cs_shortcut_func);
}

void CRegionCaptureDrawRegion::InitShortcuts()
{
	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(0, GetSafeHwnd()), false, false, false, VK_LEFT);
	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(1, GetSafeHwnd()), false, false, false, VK_UP);
	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(2, GetSafeHwnd()), false, false, false, VK_RIGHT);
	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(3, GetSafeHwnd()), false, false, false, VK_DOWN);
}

void CRegionCaptureDrawRegion::DeleteShortcuts()
{
	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(0, GetSafeHwnd()), false, false, false, VK_LEFT);
	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(1, GetSafeHwnd()), false, false, false, VK_UP);
	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(2, GetSafeHwnd()), false, false, false, VK_RIGHT);
	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(3, GetSafeHwnd()), false, false, false, VK_DOWN);
}

void CRegionCaptureDrawRegion::RegisterUserMessages()
{
}
