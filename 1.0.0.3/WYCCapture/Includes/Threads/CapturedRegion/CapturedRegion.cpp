#include "stdafx.h"
#include "CapturedRegion.h"
#include "ShortcutListParser.h"

enum
{
	LUM_CONTEXT_MENU_CHANGE_PEN_COLOR
};

//#define DEFAULT_WHEEL_OFFSET_X (16 * 3)
//#define DEFAULT_WHEEL_OFFSET_Y (9 * 3)
#define DEFAULT_CAPTURED_REGION_MIN_WIDTH (16 * 2)
#define DEFAULT_CAPTURED_REGION_MIN_HEIGHT (9 * 2)
#define DEFAULT_MOVE_OFFSET 2

#define DEFAULT_ERASE_WIDTH 20

IMPLEMENT_DYNAMIC(CCapturedRegion, CWnd)

BEGIN_MESSAGE_MAP(CCapturedRegion, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(LUM_CONTEXT_MENU_CHANGE_PEN_COLOR, OnChangeColor)
	ON_MESSAGE(WM_SHORTCUT_GENERATED, OnShortcutGenerated)
	ON_MESSAGE(UM_SHORTCUT_CHANGED, OnShortcutChanged)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_SAVE_ORIGINAL_IMAGE, OnContextMenuSaveTheOriginalImage)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_SAVE_OVERLAY_IMAGE, OnContextMenuSaveTheOverlayImage)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_ERASE, OnContextMenuErase)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_RESET, OnContextMenuReset)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_COPY_ORIGINAL_IMAGE, OnContextMenuCopyTheOriginalImageToClipboard)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_COPY_OVERLAY_IMAGE, OnContextMenuCopyTheOverlayImageToClipboard)
	//ON_COMMAND(ID_CAPTURED_REGION_MENU_INSERT_TEXT, OnContextMenuInsertText)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_CHANGE_PEN_COLOR, OnContextMenuChangePenColor)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_CLOSE, OnContextMenuClose)
	ON_COMMAND_RANGE(ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH1, ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH10, OnContextMenuChangePenWidth)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

CCapturedRegion::CCapturedRegion(int id)
	: m_is_activated(false)
	//, m_is_custom_draw(false)
	//, m_is_custom_erase(false)
	, m_current_custom_mode(CUSTOM_MODE_NONE)
	, m_prev_custom_draw_point(0, 0)
	, m_next_custom_draw_point(0, 0)
	, m_custom_draw_pen_color(RGB(255, 0, 0))
	, m_custom_draw_pen_width(1)
	, m_context_menu(NULL)
	, m_pen_width_menu(NULL)
	, m_id(id)
	, m_default_wheel_offset_x(-1)
	, m_default_wheel_offset_y(-1)
	, m_default_min_width(-1)
	, m_default_min_height(-1)
{
	SetShortcutTargetName(_T("CapturedRegion"));
}

CCapturedRegion::~CCapturedRegion()
{
	if (!m_original_image.IsNull())
		m_original_image.Destroy();

	if (!m_overlay_image.IsNull())
		m_overlay_image.Destroy();
}

bool CCapturedRegion::Create()
{
	if (!CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_PALETTEWINDOW | WS_EX_TOPMOST, ::AfxRegisterWndClass(NULL), CMyString(_T("CCapturedRegion[%d]"), m_id), WS_VISIBLE | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), NULL, 0, NULL))
		return false;

	if (!InitTooltip())
		return false;

	RegisterUserMessages();
	MakePopupMenu();
	InitShortcuts();
	InitShortcutTarget();

	return true;
}

void CCapturedRegion::Capture(HDC hdc, int current_monitor_index)
{
	TRACE_JUST();

	if (!m_original_image.IsNull())
		m_original_image.Destroy();

	if (!m_overlay_image.IsNull())
		m_overlay_image.Destroy();

	HDC hdc_draw = NULL;
	if (!hdc)
		hdc_draw = GF()->GetDesktopDC();
	else
		hdc_draw = hdc;

	int left = min(m_start_point.x, m_end_point.x);
	int top = min(m_start_point.y, m_end_point.y);
	int right = max(m_start_point.x, m_end_point.x);
	int bottom = max(m_start_point.y, m_end_point.y);
	CRect captured_region_rect(left, top, right, bottom);

	TRACE_RECT2(captured_region_rect, _T("captured_region_rect"));

	//GF()->ModifyPointFromMonitorScope(current_monitor_index, &m_start_point);
	//GF()->ModifyPointFromMonitorScope(current_monitor_index, &m_end_point);

	int x = min(m_start_point.x, m_end_point.x);
	int y = min(m_start_point.y, m_end_point.y);
	int width = captured_region_rect.Width();
	int height = captured_region_rect.Height();
	int bpp = GetDeviceCaps(hdc_draw, BITSPIXEL);

	TRACE_POINT2(m_start_point, _T("m_start_point"));
	TRACE_POINT2(m_end_point, _T("m_end_point"));

	//CMonitor monitor = CMonitors::GetPrimaryMonitor();
	//CRect monitor_rect;
	//monitor.GetMonitorRect(monitor_rect);
	//CalculateDefaultWheelOffset(monitor_rect);
	CalculateDefaultWheelOffset(captured_region_rect);

	if (!m_original_image.Create(width, height, bpp))
		return;
	::BitBlt(
		m_original_image.GetDC(),
		0, 0, width, height,
		hdc_draw,
		x, y,
		SRCCOPY);
	m_original_image.ReleaseDC();

	if (!m_overlay_image.Create(width, height, bpp))
		return;
	::BitBlt(
		m_overlay_image.GetDC(),
		0, 0, width, height,
		hdc_draw,
		x, y,
		SRCCOPY);
	m_overlay_image.ReleaseDC();

	CRect real_captured_region(captured_region_rect);
	if (real_captured_region.Width() < DEFAULT_CAPTURED_REGION_MIN_WIDTH
		|| real_captured_region.Height() < DEFAULT_CAPTURED_REGION_MIN_HEIGHT)
	{
		AfxMessageBox(CMyString(_T("You must capture the region at least %d x %d pixel!"), DEFAULT_CAPTURED_REGION_MIN_WIDTH, DEFAULT_CAPTURED_REGION_MIN_HEIGHT));
		WM_SEND_MESSAGE1(UM_CAPTURED_REGION_DELETE, m_id);
		return;
	}
	//real_captured_region.bottom += DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH;
	real_captured_region.InflateRect(DEFAULT_MOVE_OFFSET, DEFAULT_MOVE_OFFSET);
	MoveWindow(real_captured_region);

	Draw();
}

COLORREF CCapturedRegion::GetPixelValue(int x, int y)
{
	COLORREF color = 0;
	
	if (!m_overlay_image.IsNull() &&
		0 <= x && x < m_overlay_image.GetWidth() &&
		0 <= y && y < m_overlay_image.GetHeight())
		color = m_overlay_image.GetPixel(x, y);

	return color;
}

void CCapturedRegion::Reset()
{
	CRect window_rect;
	GetWindowRect(window_rect);
	CRect captured_region_rect(window_rect);
	int left = min(m_start_point.x, m_end_point.x);
	int top = min(m_start_point.y, m_end_point.y);
	int right = max(m_start_point.x, m_end_point.x);
	int bottom = max(m_start_point.y, m_end_point.y);
	int width = right - left + (DEFAULT_MOVE_OFFSET * 2);
	int height = bottom - top + (DEFAULT_MOVE_OFFSET * 2)/* + DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/;
	captured_region_rect.right = captured_region_rect.left + width;
	captured_region_rect.bottom = captured_region_rect.top + height;

	CRect real_captured_region(captured_region_rect);
	MoveWindow(real_captured_region);
	Draw();
}

void CCapturedRegion::Erase()
{
	if (m_original_image.IsNull())
		return;

	//if (!m_overlay_image.IsNull())
	//	m_overlay_image.Destroy();

	if (m_overlay_image.IsNull())
		return;

	int width = m_original_image.GetWidth();
	int height = m_original_image.GetHeight();
	//int bpp = m_original_image.GetBPP();
	//if (!m_overlay_image.Create(width, height, bpp))
	//	return;

	::BitBlt(
		m_overlay_image.GetDC(),
		0, 0, width, height,
		m_original_image.GetDC(),
		0, 0,
		SRCCOPY);
	m_overlay_image.ReleaseDC();
	m_original_image.ReleaseDC();

	Draw();
}

BOOL CCapturedRegion::PreTranslateMessage(MSG* pMsg)
{
	if (GF()->CheckWindow(m_tool_tip))
		m_tool_tip.RelayEvent(pMsg);

	return CWnd::PreTranslateMessage(pMsg);
}

void CCapturedRegion::PostNcDestroy()
{
	TRACE_JUST();
	CWnd::PostNcDestroy();

	//if (0 <= m_id && m_id < INT_MAX)
	//	WM_SEND_MESSAGE1(UM_CAPTURED_REGION_DELETE, m_id);
}

void CCapturedRegion::OnPaint()
{
	if (!GF()->CheckWindow(this))
		return;

	CRect client_rect;
	GetClientRect(client_rect);

	CPaintDC dc(this);

	CDC mem_dc;
	CBitmap mem_bmp;
	mem_dc.CreateCompatibleDC(&dc);
	mem_bmp.CreateCompatibleBitmap(&dc, client_rect.Width(), client_rect.Height());

	mem_dc.SelectObject(mem_bmp);
	mem_dc.FillSolidRect(client_rect, RGB(0, 0, 0));
	mem_dc.SetStretchBltMode(COLORONCOLOR);

	CDC temp_dc;
	CBitmap temp_bmp;
	temp_dc.CreateCompatibleDC(&dc);
	temp_bmp.CreateCompatibleBitmap(&dc, m_overlay_image.GetWidth(), m_overlay_image.GetHeight());

	temp_dc.SelectObject(temp_bmp);

	DrawCustomLine(*CDC::FromHandle(m_overlay_image.GetDC()), client_rect);
	m_overlay_image.ReleaseDC();

	DrawCustomErase(*CDC::FromHandle(m_overlay_image.GetDC()), client_rect);
	m_overlay_image.ReleaseDC();

	::BitBlt(
		temp_dc,
		0, 0, m_overlay_image.GetWidth(), m_overlay_image.GetHeight(),
		m_overlay_image.GetDC(),
		0, 0,
		SRCCOPY);
	m_overlay_image.ReleaseDC();

	::StretchBlt(
		mem_dc,
		0, 0, client_rect.Width(), client_rect.Height()/* - DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/,
		m_overlay_image.GetDC(),
		0, 0, m_overlay_image.GetWidth(), m_overlay_image.GetHeight(),
		SRCCOPY);
	m_overlay_image.ReleaseDC();

	//mem_dc.SetTextColor(RGB(255, 0, 0));
	//mem_dc.TextOut(5, 5, CMyString(_T("id = %d"), m_id));

	//DrawText(mem_dc, client_rect);

	::BitBlt(
		dc,
		client_rect.left, client_rect.top, client_rect.Width(), client_rect.Height(),
		mem_dc,
		client_rect.left, client_rect.top,
		SRCCOPY);

	::BitBlt(
		m_overlay_image.GetDC(),
		0, 0, m_overlay_image.GetWidth(), m_overlay_image.GetHeight(),
		temp_dc,
		0, 0,
		SRCCOPY);
	m_overlay_image.ReleaseDC();
}

void CCapturedRegion::OnDestroy()
{
	TRACE_JUST();
	CWnd::OnDestroy();

	DeleteShortcuts();

	WM_UNREGISTERALL();
}

void CCapturedRegion::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!GF()->CheckWindow(this))
		return;

	if (GF()->CheckVirtualKey(VK_CONTROL))
	{
		m_current_custom_mode = CUSTOM_MODE_DRAW;
		InitPoints();
	}
	else if (GF()->CheckVirtualKey(VK_MENU))
	{
		m_current_custom_mode = CUSTOM_MODE_ERASE;
		InitPoints();
	}
	else
		SendMessage(WM_NCLBUTTONDOWN, HTCAPTION);
}

void CCapturedRegion::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!GF()->CheckWindow(this))
		return;

	GetCursorPos(&point);
	ScreenToClient(&point);
	m_next_custom_draw_point = point;
	m_current_custom_mode = CUSTOM_MODE_NONE;
}

void CCapturedRegion::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (!GF()->CheckWindow(this))
		return;

	CWnd::OnActivate(nState, pWndOther, bMinimized);

	switch (nState)
	{
	case WA_ACTIVE:
	case WA_CLICKACTIVE:
		Activated();
		break;
	case WA_INACTIVE:
		Inactivated();
		break;
	}
}

void CCapturedRegion::InitShortcutTarget()
{
	std::vector<UINT64> shortcut_id_list;
	CShortcutListParser::GetSingletone()->GetShortcutList(GetShortcutTargetName(), NULL, &shortcut_id_list);

	EnterCriticalSection(&m_cs_shortcut_func);
	for (int i = 0; i < (int)shortcut_id_list.size(); i++)
	{
		switch (shortcut_id_list[i])
		{
		case 0:
			m_map_shortcut_func[(int)shortcut_id_list[i]] = OnShortcutSaveTheOriginalImage;
			break;
		case 1:
			m_map_shortcut_func[(int)shortcut_id_list[i]] = OnShortcutSaveTheOverlayImage;
			break;
		case 2:
			m_map_shortcut_func[(int)shortcut_id_list[i]] = OnShortcutErase;
			break;
		case 3:
			m_map_shortcut_func[(int)shortcut_id_list[i]] = OnShortcutReset;
			break;
		case 4:
			m_map_shortcut_func[(int)shortcut_id_list[i]] = OnShortcutCopyTheOriginalImageToClipboard;
			break;
		case 5:
			m_map_shortcut_func[(int)shortcut_id_list[i]] = OnShortcutCopyTheOverlayImageToClipboard;
			break;
		}
	}
	m_map_shortcut_func[(int)shortcut_id_list[(int)shortcut_id_list.size() - 1] + 1] = OnShortcutESC;
	LeaveCriticalSection(&m_cs_shortcut_func);
}

void CCapturedRegion::DeleteShortcuts()
{
	std::vector<UINT64> shortcut_id_list;
	CShortcutListParser::GetSingletone()->GetShortcutList(GetShortcutTargetName(), NULL, &shortcut_id_list);

	for (int i = 0; i < (int)shortcut_id_list.size(); i++)
		CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(shortcut_id_list[i], GetSafeHwnd()));

	CShortcutManager::GetSingletone()->DeleteShortcut(SHORTCUT_TARGET(shortcut_id_list[shortcut_id_list.size() - 1] + 1, GetSafeHwnd()), false, false, false, VK_ESCAPE);
}

void CCapturedRegion::Draw()
{
	if (m_original_image.IsNull() || m_overlay_image.IsNull())
		return;

	Invalidate();
}
//
//void CCapturedRegion::DrawText(CDC& mem_dc, CRect& client_rect)
//{
//	CFont font;
//	if (font.CreateFont(15, 8, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial")))
//	{
//		mem_dc.SelectObject(font);
//		mem_dc.SetTextColor(RGB(255, 255, 255));
//
//		float fX = (float)m_overlay_image.GetWidth() / (float)client_rect.Width();
//		float fY = (float)m_overlay_image.GetHeight() / (float)(client_rect.Height() /*- DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/);
//
//		CRect text_rect(client_rect);
//		text_rect.top = client_rect.bottom/* - DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/;
//		CPoint pos;
//		GetCursorPos(&pos);
//		ScreenToClient(&pos);
//		COLORREF color = GetPixelValue((int)(pos.x * fX), (int)(pos.y * fY));
//		DWORD r = GetRValue(color);
//		DWORD g = GetGValue(color);
//		DWORD b = GetBValue(color);
//		mem_dc.DrawText(CMyString(_T("RGB(%d, %d, %d)"), r, g, b), text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//	}
//}

void CCapturedRegion::DrawCustomLine(CDC& mem_dc, CRect& client_rect)
{
	if (m_current_custom_mode != CUSTOM_MODE_DRAW)
		return;

	Gdiplus::Graphics memG(mem_dc);
	Gdiplus::Rect rect(ToGdiplusRect(client_rect));

	//TRACE_JUST();

	m_prev_custom_draw_point = m_next_custom_draw_point;
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	m_next_custom_draw_point = point;

	float fX = (float)m_overlay_image.GetWidth() / (float)rect.Width;
	float fY = (float)m_overlay_image.GetHeight() / (float)(rect.Height /*- DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/);

	//TRACE(_T("fX = %.2f | fY = %.2f\n"), fX, fY);

	Gdiplus::Pen pen(ToGdiplusColor(m_custom_draw_pen_color), (float)m_custom_draw_pen_width);
	pen.SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound, Gdiplus::DashCapRound);
	pen.SetLineJoin(Gdiplus::LineJoinBevel);

	Gdiplus::PointF prev_point((float)m_prev_custom_draw_point.x * fX, (float)m_prev_custom_draw_point.y * fY);
	Gdiplus::PointF next_point((float)m_next_custom_draw_point.x * fX, (float)m_next_custom_draw_point.y * fY);

	//TRACE(_T("prev_point = (%.2f, %.2f) | next_point = (%.2f, %.2f)\n\n"),
	//	prev_point.X, prev_point.Y,
	//	next_point.X, next_point.Y);

	if (m_custom_draw_pen_width > 1)
	{
		Gdiplus::RectF point_rect(
			next_point.X - (float)m_custom_draw_pen_width / 2.f,
			next_point.Y - (float)m_custom_draw_pen_width / 2.f,
			(float)m_custom_draw_pen_width,
			(float)m_custom_draw_pen_width);
		//TRACE(_T("point_rect = (%.2f, %.2f, %.2f, %.2f)\n"), point_rect.X, point_rect.Y, point_rect.Width, point_rect.Height);
		Gdiplus::SolidBrush point_brush(ToGdiplusColor(m_custom_draw_pen_color));
		memG.FillEllipse(&point_brush, point_rect);
	}
	else
	{
		mem_dc.SetPixel((int)roundf(next_point.X), (int)roundf(next_point.Y), m_custom_draw_pen_color);
	}

	memG.DrawLine(&pen, prev_point, next_point);
}

void CCapturedRegion::DrawCustomErase(CDC& mem_dc, CRect& client_rect)
{
	if (m_current_custom_mode != CUSTOM_MODE_ERASE)
		return;

	TRACE_JUST();

	Gdiplus::Rect rect(ToGdiplusRect(client_rect));

	m_prev_custom_draw_point = m_next_custom_draw_point;
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	m_next_custom_draw_point = point;

	float fX = (float)m_overlay_image.GetWidth() / (float)rect.Width;
	float fY = (float)m_overlay_image.GetHeight() / (float)(rect.Height);

	Gdiplus::PointF prev_point((float)m_prev_custom_draw_point.x * fX, (float)m_prev_custom_draw_point.y * fY);
	Gdiplus::PointF next_point((float)m_next_custom_draw_point.x * fX, (float)m_next_custom_draw_point.y * fY);

	int w = DEFAULT_ERASE_WIDTH;
	int h = w;
	int x_prev = (int)prev_point.X - w / 2;
	int y_prev = (int)prev_point.Y - h / 2;
	int x_next = (int)next_point.X - w / 2;
	int y_next = (int)next_point.Y - h / 2;

	int x_min = min(x_prev, x_next);
	int y_min = min(y_prev, y_next);
	int x_max = max(x_prev, x_next);
	int y_max = max(y_prev, y_next);

	int x = x_min;
	int y = y_min;
	while (true)
	{
		::BitBlt(mem_dc, x, y, w, h, m_original_image.GetDC(), x, y, SRCCOPY);
		m_original_image.ReleaseDC();
		if (x < x_max)
			x++;
		if (y < y_max)
			y++;

		if (x >= x_max && y >= y_max)
			break;
	}
}

Gdiplus::Color CCapturedRegion::ToGdiplusColor(COLORREF color)
{
	DWORD r = GetRValue(color);
	DWORD g = GetGValue(color);
	DWORD b = GetBValue(color);
	Gdiplus::Color gp_color((BYTE)r, (BYTE)g, (BYTE)b);

	return gp_color;
}

Gdiplus::Rect CCapturedRegion::ToGdiplusRect(CRect rect)
{
	int x = rect.left;
	int y = rect.top;
	int w = rect.Width();
	int h = rect.Height();
	Gdiplus::Rect gp_rect(x, y, w, h);

	return gp_rect;
}

void CCapturedRegion::OnShortcutCopyTheOriginalImageToClipboard(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->CopyToClipboard(true);
}

void CCapturedRegion::OnShortcutCopyTheOverlayImageToClipboard(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->CopyToClipboard(false);
}

void CCapturedRegion::OnShortcutSaveTheOriginalImage(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->SaveFile(true);
}

void CCapturedRegion::OnShortcutSaveTheOverlayImage(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->SaveFile(false);
}

void CCapturedRegion::OnShortcutErase(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->Erase();
}

void CCapturedRegion::OnShortcutReset(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->Reset();
}

void CCapturedRegion::OnShortcutESC(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->Close();
}

bool CCapturedRegion::InitTooltip()
{
	if (!m_tool_tip.Create(this))
		return false;

	m_tool_tip.AddTool(this);

	m_tool_tip.SetMaxTipWidth(200);
	m_tool_tip.SetDelayTime(TTDT_AUTOPOP, 10000);

	m_tool_tip.Activate(TRUE);

	return true;
}

void CCapturedRegion::InitShortcuts()
{
	DeleteShortcuts();

	std::vector<DWORD> shortcut_code_list;
	std::vector<UINT64> shortcut_id_list;
	CShortcutListParser::GetSingletone()->GetShortcutList(GetShortcutTargetName(), &shortcut_code_list, &shortcut_id_list);

	for (int i = 0; i < (int)shortcut_id_list.size(); i++)
		CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(shortcut_id_list[i], GetSafeHwnd()), shortcut_code_list[i]);

	CShortcutManager::GetSingletone()->SetShortcut(SHORTCUT_TARGET(shortcut_id_list[shortcut_id_list.size() - 1] + 1, GetSafeHwnd()), false, false, false, VK_ESCAPE);
}

void CCapturedRegion::RegisterUserMessages()
{
	WM_REGISTER(UM_SHORTCUT_CHANGED);
}

void CCapturedRegion::CopyToClipboard(bool is_original_image)
{
	CPaintDC dc(this);
	CDC mem_dc;
	CBitmap mem_bmp;
	if (is_original_image)
	{
		if (m_original_image.IsNull())
			return;

		mem_dc.CreateCompatibleDC(&dc);
		mem_bmp.CreateCompatibleBitmap(&dc, m_original_image.GetWidth(), m_original_image.GetHeight());

		mem_dc.SelectObject(mem_bmp);

		::BitBlt(
			mem_dc,
			0, 0, m_original_image.GetWidth(), m_original_image.GetHeight(),
			m_original_image.GetDC(),
			0, 0,
			SRCCOPY);
		m_original_image.ReleaseDC();
	}
	else
	{
		if (m_overlay_image.IsNull())
			return;

		mem_dc.CreateCompatibleDC(&dc);
		mem_bmp.CreateCompatibleBitmap(&dc, m_overlay_image.GetWidth(), m_overlay_image.GetHeight());

		mem_dc.SelectObject(mem_bmp);

		::BitBlt(
			mem_dc,
			0, 0, m_overlay_image.GetWidth(), m_overlay_image.GetHeight(),
			m_overlay_image.GetDC(),
			0, 0,
			SRCCOPY);
		m_overlay_image.ReleaseDC();
	}

	::OpenClipboard(GetSafeHwnd());
	::EmptyClipboard();
	::SetClipboardData(CF_BITMAP, mem_bmp);
	::CloseClipboard();
}

void CCapturedRegion::Activated()
{
	if (!m_is_activated)
		m_is_activated = true;
}

void CCapturedRegion::Inactivated()
{
	if (m_is_activated)
		m_is_activated = false;
}

void CCapturedRegion::MakePopupMenu()
{
	if (m_context_menu)
	{
		::DestroyMenu(m_context_menu);
		m_context_menu = NULL;
	}
	if (m_pen_width_menu)
	{
		::DestroyMenu(m_pen_width_menu);
		m_pen_width_menu = NULL;
	}
	m_context_menu = ::CreatePopupMenu();
	m_pen_width_menu = ::CreatePopupMenu();
	for (int i = 0; i < 10; i++)
	{
		if (i == 9)
			::InsertMenu(m_pen_width_menu, -1, MF_BYPOSITION, ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH1 + i, CMyString(_T("x%d\t(&0)"), i + 1));
		else
			::InsertMenu(m_pen_width_menu, -1, MF_BYPOSITION, ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH1 + i, CMyString(_T("x%d\t(&%d)"), i + 1, i + 1));
	}

	::CheckMenuItem(m_pen_width_menu, ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH1, MF_CHECKED);

	::AppendMenu(m_context_menu, MF_POPUP, (UINT_PTR)m_pen_width_menu, _T("Pen Width(&W)..."));
	::InsertMenu(m_context_menu, -1, MF_BYPOSITION, ID_CAPTURED_REGION_MENU_CHANGE_PEN_COLOR, _T("Pen Color(&O)"));
	//::InsertMenu(m_context_menu, -1, MF_BYPOSITION, ID_CAPTURED_REGION_MENU_INSERT_TEXT, _T("Insert Text(&T)"));
	std::vector<DWORD> shortcut_code_list;
	std::vector<UINT64> shortcut_id_list;
	std::vector<CString> shortcut_command_name_list;
	CShortcutListParser::GetSingletone()->GetShortcutList(GetShortcutTargetName(), &shortcut_code_list, &shortcut_id_list, &shortcut_command_name_list);
	for (int i = 0; i < (int)shortcut_id_list.size(); i++)
	{
		switch (shortcut_id_list[i])
		{
		case 0:
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_SAVE_ORIGINAL_IMAGE, CMyString(_T("%s\t%s"), shortcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
			break;
		case 1:
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_SAVE_OVERLAY_IMAGE, CMyString(_T("%s\t%s"), shortcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
			break;
		case 2:
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_ERASE, CMyString(_T("%s\t%s"), shortcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
			break;
		case 3:
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_RESET, CMyString(_T("%s\t%s"), shortcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
			break;
		case 4:
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_COPY_ORIGINAL_IMAGE, CMyString(_T("%s\t%s"), shortcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
			break;
		case 5:
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_COPY_OVERLAY_IMAGE, CMyString(_T("%s\t%s"), shortcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
			break;
		}
	}
	::InsertMenu(m_context_menu, -1, MF_BYPOSITION, ID_CAPTURED_REGION_MENU_CLOSE, _T("Close"));
}

void CCapturedRegion::OnContextMenuSaveTheOriginalImage()
{
	SaveFile(true);
}

void CCapturedRegion::OnContextMenuSaveTheOverlayImage()
{
	SaveFile(false);
}

void CCapturedRegion::OnContextMenuCopyTheOriginalImageToClipboard()
{
	CopyToClipboard(true);
}

void CCapturedRegion::OnContextMenuCopyTheOverlayImageToClipboard()
{
	CopyToClipboard(false);
}

void CCapturedRegion::OnContextMenuChangePenColor()
{
	//PostMessage(LUM_CONTEXT_MENU_CHANGE_PEN_COLOR);
	SendMessage(LUM_CONTEXT_MENU_CHANGE_PEN_COLOR);
}
//
//void CCapturedRegion::OnContextMenuInsertText()
//{
//	CPoint point;
//	GetCursorPos(&point);
//	ScreenToClient(&point);
//
//	if (!GF()->CheckWindow(m_edit))
//	{
//		if (m_edit.Create(this, IDC_CAPTURED_REGION_INSERTED_EDIT))
//		{
//			m_edit.SetParentWnd(GetSafeHwnd());
//			m_edit.MoveWindow(point.x, point.y, 80, 40);
//		}
//	}
//}

void CCapturedRegion::OnContextMenuChangePenWidth(UINT id)
{
	m_custom_draw_pen_width = (int)id - (int)ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH1 + 1;

	for (UINT i = ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH1; i <= ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH10; i++)
	{
		if (i == id)
			::CheckMenuItem(m_pen_width_menu, i, MF_CHECKED);
		else
			::CheckMenuItem(m_pen_width_menu, i, MF_UNCHECKED);
	}
}

void CCapturedRegion::OnContextMenuErase()
{
	Erase();
}

void CCapturedRegion::OnContextMenuReset()
{
	Reset();
}

void CCapturedRegion::OnContextMenuClose()
{
	Close();
}

void CCapturedRegion::Close()
{
	//if (GF()->CheckWindow(this))
	//	PostMessage(WM_CLOSE);

	//if (GF()->CheckWindow(this))
	//	WM_POST_MESSAGE1(UM_CAPTURED_REGION_DELETE, m_id);

	if (GF()->CheckWindow(this))
		WM_SEND_MESSAGE1(UM_CAPTURED_REGION_DELETE, m_id);
}

bool CCapturedRegion::SaveFileDlgOpen(CString* file_path)
{
	CString filter =
		_T("JPEG File (*.jpeg;*.jpg)|*.jpeg;*.jpg|")
		_T("BMP File (*.bmp)|*.bmp|")
		_T("PNG File (*.png)|*.png||");

	CFileDialog save_file_dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, this);

	if (!m_last_save_folder_dir.IsEmpty())
		save_file_dlg.m_ofn.lpstrInitialDir = m_last_save_folder_dir;

	if (save_file_dlg.DoModal() != IDOK)
		return false;

	CString path = save_file_dlg.GetPathName();
	m_last_save_folder_dir = save_file_dlg.GetFolderPath();
	bool result = false;
	int index = path.Find(_T("."));
	if (index < 0)
	{
		int filter_index = save_file_dlg.m_ofn.nFilterIndex;
		CString extension = path.Right(path.GetLength() - index);
		extension.MakeUpper();
		switch (filter_index)
		{
		case 1:
			result |= (extension == _T("JPEG"));
			result |= (extension == _T("JPG"));
			if (!result)
				path += _T(".jpeg");
			break;
		case 2:
			result |= (extension == _T("BMP"));
			if (!result)
				path += _T(".bmp");
			break;
		case 3:
			result |= (extension == _T("PNG"));
			if (!result)
				path += _T(".png");
			break;
		}
	}

	*file_path = path;

	return true;
}

void CCapturedRegion::SaveFile(bool is_original_image)
{
	if (is_original_image)
	{
		if (m_original_image.IsNull())
			return;

		CString file_path;
		if (SaveFileDlgOpen(&file_path))
			m_original_image.Save(file_path);
	}
	else
	{
		if (m_overlay_image.IsNull())
			return;

		CString file_path;
		if (SaveFileDlgOpen(&file_path))
			m_overlay_image.Save(file_path);
	}
}

int CCapturedRegion::GetGCD(int a, int b)
{
	int _numerator = 0;
	int _denominator = 0;
	if (a > b)
	{
		_numerator = a;
		_denominator = b;
	}
	else
	{
		_numerator = b;
		_denominator = a;
	}

	int _remainder = _denominator;
	int _remaindee = _numerator;
	int _remain = 1;
	while (_remain != 0)
	{
		_remain = _remaindee % _remainder;
		_remaindee = _remainder;
		_remainder = _remain;
	}

	int _GCD = _remaindee;

	return _GCD;
}

void CCapturedRegion::CalculateDefaultWheelOffset(CRect rect)
{
	int width = rect.Width();
	int height = rect.Height();
	int gcd = GetGCD(width, height);

	TRACE(_T("width = %d | height = %d | gcd = %d\n"), width, height, gcd);

	//float ratio = (float)width / (float)height;
	//int horizontal_ratio = (int)roundf(ratio * 30.f);
	//int vertical_ratio = (int)roundf((1.f / ratio) * 30.f);

	int horizontal_ratio = width / gcd;
	int vertical_ratio = height / gcd;

	TRACE(_T("horizontal_ratio = %d | vertical_ratio = %d\n"), horizontal_ratio, vertical_ratio);

	//m_default_wheel_offset_x = horizontal_ratio * 3;
	//m_default_wheel_offset_y = vertical_ratio * 3;
	//
	//m_default_min_width = horizontal_ratio * 2;
	//m_default_min_height = vertical_ratio * 2;
	if (gcd <= 2)
	{
		m_default_wheel_offset_x = horizontal_ratio / 4;
		m_default_wheel_offset_y = vertical_ratio / 4;

		m_default_min_width = horizontal_ratio / 3;
		m_default_min_height = vertical_ratio / 3;
	}
	else
	{
		m_default_wheel_offset_x = horizontal_ratio;
		m_default_wheel_offset_y = vertical_ratio;

		m_default_min_width = horizontal_ratio;
		m_default_min_height = vertical_ratio;
	}
}

void CCapturedRegion::InitPoints()
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	m_prev_custom_draw_point = point;
	m_next_custom_draw_point = point;
}

bool CCapturedRegion::IsCustomDrawing()
{
	return (GF()->CheckVirtualKey(VK_CONTROL) && GF()->CheckVirtualKey(VK_LBUTTON));
}

bool CCapturedRegion::IsCustomErasing()
{
	return (GF()->CheckVirtualKey(VK_MENU) && GF()->CheckVirtualKey(VK_LBUTTON));
}

void CCapturedRegion::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!GF()->CheckWindow(this))
		return;

	if (!IsCustomDrawing() && !IsCustomErasing())
	{
		m_current_custom_mode = CUSTOM_MODE_NONE;
		//m_is_custom_draw = false;
		if (!m_original_image.IsNull() && GF()->CheckWindow(m_tool_tip))
		{
			CRect client_rect;
			GetClientRect(client_rect);
			float fX = (float)m_original_image.GetWidth() / (float)client_rect.Width();
			float fY = (float)m_original_image.GetHeight() / (float)(client_rect.Height() /*- DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/);
			int x = (int)((float)point.x * fX);
			int y = (int)((float)point.y * fY);
			COLORREF color = m_original_image.GetPixel(x, y);
			int r = (int)GetRValue(color);
			int g = (int)GetGValue(color);
			int b = (int)GetBValue(color);
			m_tool_tip.UpdateTipText(CMyString(_T("(x, y) = (%d, %d)\r\nRGB = (%d, %d, %d)"), x, y, r, g, b), this);
		}
	}

	Draw();

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CCapturedRegion::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GF()->CheckVirtualKey(VK_CONTROL))
	{
		if (zDelta <= 0)
			OnWheelDown();
		else
			OnWheelUp();
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CCapturedRegion::OnWheelDown()
{
	CRect this_rect;
	GetWindowRect(this_rect);
	CRect real_captured_region(this_rect);
	real_captured_region.DeflateRect(DEFAULT_MOVE_OFFSET, DEFAULT_MOVE_OFFSET);
	//real_captured_region.bottom -= DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH;
	if (real_captured_region.Width() >= 2 * m_default_min_width &&
		real_captured_region.Height() >= 2 * m_default_min_height)
	{
		this_rect.right -= m_default_wheel_offset_x;
		this_rect.bottom -= m_default_wheel_offset_y;
		//real_captured_region.bottom += DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH;
		//TRACE_RECT(this_rect);
		MoveWindow(this_rect);
		Draw();
	}
}

void CCapturedRegion::OnWheelUp()
{
	CRect this_rect;
	GetWindowRect(this_rect);
	this_rect.right += m_default_wheel_offset_x;
	this_rect.bottom += m_default_wheel_offset_y;
	//TRACE_RECT(this_rect);
	MoveWindow(this_rect);
	Draw();
}

void CCapturedRegion::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (!m_context_menu)
		return;

	::TrackPopupMenu(m_context_menu, TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, 0, GetSafeHwnd(), NULL);
}

LRESULT CCapturedRegion::OnChangeColor(WPARAM wParam, LPARAM lParam)
{
	SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	CColorDialog color_dialog(m_custom_draw_pen_color, CC_FULLOPEN | CC_RGBINIT);
	if (color_dialog.DoModal() != IDOK)
		return -1;

	TRACE_JUST();
	m_custom_draw_pen_color = color_dialog.GetColor();

	return 0;
}

LRESULT CCapturedRegion::OnShortcutGenerated(WPARAM wParam, LPARAM lParam)
{
	if (!m_is_activated)
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

LRESULT CCapturedRegion::OnShortcutChanged(WPARAM wParam, LPARAM lParam)
{
	MakePopupMenu();
	InitShortcuts();
	InitShortcutTarget();

	return 0;
}