#include "stdafx.h"
#include "WindowCaptureDrawRegion.h"

#define DEFAULT_SELECTED_WINDOW_COLOR				RGB(255, 0, 0)
#define MAX_SELECTED_WINDOW_COLOR					RGB(255, 150, 150)
#define DEFAULT_SELECTED_WINDOW_COLOR_INCREMENTS	5

IMPLEMENT_DYNAMIC(CWindowCaptureDrawRegion, CWnd)

BEGIN_MESSAGE_MAP(CWindowCaptureDrawRegion, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_SHORTCUT_GENERATED, OnShortcutGenerated)
END_MESSAGE_MAP()


CWindowCaptureDrawRegion::CWindowCaptureDrawRegion()
	: m_is_shown(false)
	, m_is_first_time(true)
	, m_is_cliecked(false)
	, m_is_pause(true)
	, m_temp_memory_bitmap(NULL)
	, m_temp_memory_dc(NULL)
	, m_selected_window_color(DEFAULT_SELECTED_WINDOW_COLOR)
	, m_selected_window_color_increments(DEFAULT_SELECTED_WINDOW_COLOR_INCREMENTS)
{
	SetShortcutTargetName(_T("CWindowCaptureDrawRegion"));
}

CWindowCaptureDrawRegion::~CWindowCaptureDrawRegion()
{
}

bool CWindowCaptureDrawRegion::Create()
{
	if (!CWnd::CreateEx(WS_EX_PALETTEWINDOW, ::AfxRegisterWndClass(NULL), _T("CWindowCaptureDrawRegion"), WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), NULL, 0, NULL))
		return false;

	CString window_name;
	GetWindowText(window_name);
	CWndManager::GetSingletone()->AddExceptionList(window_name);

	RegisterUserMessages();

	//if (!m_window_capture_detail_region.Create(this, IDC_WINDOW_CAPTURE_DETAIL_REGION))
	//	return false;

	InitShortcuts();
	InitShortcutTarget();

	return true;
}

void CWindowCaptureDrawRegion::OnDraw(CPoint current_pos)
{
	if (!GF()->CheckWindow(GetSafeHwnd()))
		return;

	m_current_pos = current_pos;
	Invalidate();
}

void CWindowCaptureDrawRegion::Show()
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

void CWindowCaptureDrawRegion::Hide()
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
		//m_window_capture_detail_region.SetRegionRect(CRect(0, 0, 0, 0));
	}
}

BOOL CWindowCaptureDrawRegion::PreTranslateMessage(MSG * pMsg)
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

void CWindowCaptureDrawRegion::OnPaint()
{
	if (!m_is_shown)
		return;

	CPaintDC dc(this);

	CRect client_rect;
	GetClientRect(&client_rect);

	//CPerformanceChecker checker(_T("CWindowCaptureDrawRegion::OnPaint"));

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
	DrawSelectedRegion(mem_dc);

	DrawToThis(dc, mem_dc, client_rect);
	if (is_valid)
		DrawBackToMonitorDC(*monitor_dc, monitor_rect);

	//m_window_capture_detail_region.OnDraw(m_temp_memory_dc, m_current_pos, m_captured_window_info.position);
}

void CWindowCaptureDrawRegion::OnDestroy()
{
	CWnd::OnDestroy();

	DeleteShortcuts();
}

void CWindowCaptureDrawRegion::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_is_cliecked = true;
	WM_POST_MESSAGE(UM_WINDOW_CAPTURE_START);

	CWnd::OnLButtonDown(nFlags, point);
}

void CWindowCaptureDrawRegion::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_is_cliecked)
	{
		m_is_cliecked = false;

		if (m_captured_window_info.position.Width() <= 0 || m_captured_window_info.position.Height() <= 0)
			return;

		LPWINDOW_CAPTURE_STOP pwindow_capture_stop = new WINDOW_CAPTURE_STOP(m_captured_window_info.position, m_temp_memory_dc, 0);
		WM_POST_MESSAGE1(UM_WINDOW_CAPTURE_STOP, pwindow_capture_stop);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

LRESULT CWindowCaptureDrawRegion::OnShortcutGenerated(WPARAM wParam, LPARAM lParam)
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

void CWindowCaptureDrawRegion::InitShortcutTarget()
{
	EnterCriticalSection(&m_cs_shortcut_func);
	m_map_shortcut_func[0] = OnShortcutLeft;
	m_map_shortcut_func[1] = OnShortcutUp;
	m_map_shortcut_func[2] = OnShortcutRight;
	m_map_shortcut_func[3] = OnShortcutDown;
	LeaveCriticalSection(&m_cs_shortcut_func);
}

void CWindowCaptureDrawRegion::InitShortcuts()
{
	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(0, GetSafeHwnd()), false, false, false, VK_LEFT);
	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(1, GetSafeHwnd()), false, false, false, VK_UP);
	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(2, GetSafeHwnd()), false, false, false, VK_RIGHT);
	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(3, GetSafeHwnd()), false, false, false, VK_DOWN);
}

void CWindowCaptureDrawRegion::DeleteShortcuts()
{
	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(0, GetSafeHwnd()), false, false, false, VK_LEFT);
	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(1, GetSafeHwnd()), false, false, false, VK_UP);
	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(2, GetSafeHwnd()), false, false, false, VK_RIGHT);
	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(3, GetSafeHwnd()), false, false, false, VK_DOWN);
}

void CWindowCaptureDrawRegion::RegisterUserMessages()
{
}

void CWindowCaptureDrawRegion::OnShortcutLeft(HWND hwnd)
{
	if (!hwnd)
		return;

	CWindowCaptureDrawRegion* main = reinterpret_cast<CWindowCaptureDrawRegion*>(CWnd::FromHandle(hwnd));

	main->OnLeft();
}

void CWindowCaptureDrawRegion::OnShortcutUp(HWND hwnd)
{
	if (!hwnd)
		return;

	CWindowCaptureDrawRegion* main = reinterpret_cast<CWindowCaptureDrawRegion*>(CWnd::FromHandle(hwnd));

	main->OnUp();
}

void CWindowCaptureDrawRegion::OnShortcutRight(HWND hwnd)
{
	if (!hwnd)
		return;

	CWindowCaptureDrawRegion* main = reinterpret_cast<CWindowCaptureDrawRegion*>(CWnd::FromHandle(hwnd));

	main->OnRight();
}

void CWindowCaptureDrawRegion::OnShortcutDown(HWND hwnd)
{
	if (!hwnd)
		return;

	CWindowCaptureDrawRegion* main = reinterpret_cast<CWindowCaptureDrawRegion*>(CWnd::FromHandle(hwnd));

	main->OnDown();
}

void CWindowCaptureDrawRegion::OnLeft()
{
	m_current_pos.x -= 1;
	SetCursorPos(m_current_pos.x, m_current_pos.y);
}

void CWindowCaptureDrawRegion::OnRight()
{
	m_current_pos.x += 1;
	SetCursorPos(m_current_pos.x, m_current_pos.y);
}

void CWindowCaptureDrawRegion::OnUp()
{
	m_current_pos.y -= 1;
	SetCursorPos(m_current_pos.x, m_current_pos.y);
}

void CWindowCaptureDrawRegion::OnDown()
{
	m_current_pos.y += 1;
	SetCursorPos(m_current_pos.x, m_current_pos.y);
}

void CWindowCaptureDrawRegion::DrawCrossLine(CDC& mem_dc, CPoint pos)
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

void CWindowCaptureDrawRegion::DrawDesktopBackground(CDC& mem_dc, CRect& client_rect)
{
	::BitBlt(
		mem_dc,
		0, 0, client_rect.Width(), client_rect.Height(),
		m_temp_memory_dc,
		0, 0,
		SRCCOPY);
}

void CWindowCaptureDrawRegion::DrawSelectedRegion(CDC& mem_dc)
{
	CPen pen(PS_SOLID, 2, m_selected_window_color);
	mem_dc.SelectObject(pen);

	if (m_selected_window_color == DEFAULT_SELECTED_WINDOW_COLOR)
		m_selected_window_color_increments = DEFAULT_SELECTED_WINDOW_COLOR_INCREMENTS;
	else if (m_selected_window_color == MAX_SELECTED_WINDOW_COLOR)
		m_selected_window_color_increments = -DEFAULT_SELECTED_WINDOW_COLOR_INCREMENTS;

	int r = (int)GetRValue(m_selected_window_color);
	int g = (int)GetGValue(m_selected_window_color) + m_selected_window_color_increments;
	int b = (int)GetBValue(m_selected_window_color) + m_selected_window_color_increments;

	m_selected_window_color = RGB(r, g, b);

	WND_INFO wnd_info = CWndManager::GetSingletone()->GetTopAllWndFromFromPoint(m_current_pos);

	m_captured_window_info = wnd_info;

	mem_dc.MoveTo(m_captured_window_info.position.left, m_captured_window_info.position.top);
	mem_dc.LineTo(m_captured_window_info.position.right, m_captured_window_info.position.top);

	mem_dc.MoveTo(m_captured_window_info.position.right, m_captured_window_info.position.top);
	mem_dc.LineTo(m_captured_window_info.position.right, m_captured_window_info.position.bottom);

	mem_dc.MoveTo(m_captured_window_info.position.right, m_captured_window_info.position.bottom);
	mem_dc.LineTo(m_captured_window_info.position.left, m_captured_window_info.position.bottom);

	mem_dc.MoveTo(m_captured_window_info.position.left, m_captured_window_info.position.bottom);
	mem_dc.LineTo(m_captured_window_info.position.left, m_captured_window_info.position.top);

	CString region_string;
	region_string.Format(_T("%d x %d"), m_captured_window_info.position.Width(), m_captured_window_info.position.Height());
	mem_dc.SetTextColor(RGB(255, 0, 0));
	//mem_dc.SetBkMode(TRANSPARENT);
	//mem_dc.DrawText(region_string, m_captured_window_info.position, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	CPoint text_position = m_captured_window_info.position.TopLeft();
	mem_dc.TextOut(text_position.x + 5, text_position.y + 5, region_string);
}

bool CWindowCaptureDrawRegion::DrawFirstTime(CDC& this_dc, CDC& monitor_dc, CRect& client_rect, CRect& monitor_rect)
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

void CWindowCaptureDrawRegion::DrawToThis(CDC& this_dc, CDC& mem_dc, CRect& client_rect)
{
	this_dc.BitBlt(
		0, 0, client_rect.Width(), client_rect.Height(),
		&mem_dc,
		0, 0,
		SRCCOPY);
}

void CWindowCaptureDrawRegion::DrawBackToMonitorDC(CDC& monitor_dc, CRect& monitor_rect)
{
	::BitBlt(
		monitor_dc,
		0, 0, monitor_rect.Width(), monitor_rect.Height(),
		m_temp_memory_dc,
		0, 0,
		SRCCOPY);
}
