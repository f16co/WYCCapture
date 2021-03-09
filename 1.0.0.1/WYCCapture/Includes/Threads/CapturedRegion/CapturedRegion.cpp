#include "stdafx.h"
#include "CapturedRegion.h"
#include "ShortcutListParser.h"

enum
{
	LUM_CONTEXT_MENU_CHANGE_PEN_COLOR
};

#define DEFAULT_WHEEL_OFFSET_X (16 * 3)
#define DEFAULT_WHEEL_OFFSET_Y (9 * 3)
#define DEFAULT_CAPTURED_REGION_MIN_WIDTH 30
#define DEFAULT_CAPTURED_REGION_MIN_HEIGHT 30
#define DEFAULT_MOVE_OFFSET 2

IMPLEMENT_DYNAMIC(CCapturedRegion, CWnd)

BEGIN_MESSAGE_MAP(CCapturedRegion, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(LUM_CONTEXT_MENU_CHANGE_PEN_COLOR, OnChangeColor)
	ON_MESSAGE(WM_SHORTCUT_GENERATED, OnShortcutGenerated)
	ON_MESSAGE(UM_SHORTCUT_CHANGED, OnShortcutChanged)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_SAVE, OnContextMenuSave)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_ERASE, OnContextMenuErase)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_RESET, OnContextMenuReset)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_COPY, OnContextMenuCopy)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_CHANGE_PEN_COLOR, OnContextMenuChangePenColor)
	ON_COMMAND(ID_CAPTURED_REGION_MENU_CLOSE, OnContextMenuClose)
	ON_COMMAND_RANGE(ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH1, ID_CAPTURED_REGION_MENU_CHANGE_PEN_WIDTH10, OnContextMenuChangePenWidth)
END_MESSAGE_MAP()

CCapturedRegion::CCapturedRegion(int id)
	: m_is_activated(false)
	, m_is_custom_draw(false)
	, m_prev_custom_draw_point(0, 0)
	, m_next_custom_draw_point(0, 0)
	, m_custom_draw_pen_color(RGB(255, 0, 0))
	, m_custom_draw_pen_width(1)
	, m_context_menu(NULL)
	, m_pen_width_menu(NULL)
	, m_id(id)
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
	if (!CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_PALETTEWINDOW | WS_EX_TOPMOST, ::AfxRegisterWndClass(NULL), CMyString(_T("CCapturedRegion[%d]"), m_id), WS_VISIBLE | WS_POPUP, CRect(0, 0, 0, 0), NULL, 0, NULL))
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

	GF()->ModifyPointFromMonitorScope(current_monitor_index, &m_start_point);
	GF()->ModifyPointFromMonitorScope(current_monitor_index, &m_end_point);

	int x = min(m_start_point.x, m_end_point.x);
	int y = min(m_start_point.y, m_end_point.y);
	int width = captured_region_rect.Width();
	int height = captured_region_rect.Height();
	int bpp = GetDeviceCaps(hdc_draw, BITSPIXEL);

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
		AfxMessageBox(CMyString(_T("You must capture the region at least %dx%dpx!"), DEFAULT_CAPTURED_REGION_MIN_WIDTH, DEFAULT_CAPTURED_REGION_MIN_HEIGHT));
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

	if (!m_overlay_image.IsNull())
		m_overlay_image.Destroy();

	int width = m_original_image.GetWidth();
	int height = m_original_image.GetHeight();
	int bpp = m_original_image.GetBPP();
	if (!m_overlay_image.Create(width, height, bpp))
		return;

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
	if (GF()->CheckWindow(m_tool_tip.GetSafeHwnd()))
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
	if (!GF()->CheckWindow(GetSafeHwnd()))
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

	Gdiplus::Graphics overlay_imageG(m_overlay_image.GetDC());
	m_overlay_image.ReleaseDC();
	if (true)
	{
		Gdiplus::Rect rect(ToGdiplusRect(client_rect));
		DrawCustomLine(overlay_imageG, rect);
	}

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

	//DrawCustomLine(mem_dc);

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
	if (!GF()->CheckWindow(GetSafeHwnd()))
		return;

	if (GetAsyncKeyState(VK_CONTROL) < 0)
	{
		m_is_custom_draw = true;
		GetCursorPos(&point);
		ScreenToClient(&point);
		m_prev_custom_draw_point = point;
		m_next_custom_draw_point = point;
	}
	else
		SendMessage(WM_NCLBUTTONDOWN, HTCAPTION);

	//CWnd::OnLButtonDown(nFlags, point);
}

void CCapturedRegion::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!GF()->CheckWindow(GetSafeHwnd()))
		return;

	if (GetAsyncKeyState(VK_CONTROL) < 0)
	{
		GetCursorPos(&point);
		ScreenToClient(&point);
		m_next_custom_draw_point = point;
	}

	//CWnd::OnLButtonUp(nFlags, point);
}

void CCapturedRegion::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	TRACE_JUST();
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CCapturedRegion::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (!GF()->CheckWindow(GetSafeHwnd()))
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
	EnterCriticalSection(&m_cs_shortcut_func);
	m_map_shortcut_func[0] = OnShortcutSave;
	m_map_shortcut_func[1] = OnShortcutErase;
	m_map_shortcut_func[2] = OnShortcutReset;
	m_map_shortcut_func[3] = OnShortcutCopy;
	m_map_shortcut_func[4] = OnShortcutESC;
	LeaveCriticalSection(&m_cs_shortcut_func);
}

void CCapturedRegion::DeleteShortcuts()
{
	std::vector<DWORD> shortcut_code_list;
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

void CCapturedRegion::DrawText(CDC& mem_dc, CRect& client_rect)
{
	CFont font;
	if (font.CreateFont(15, 8, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial")))
	{
		mem_dc.SelectObject(font);
		mem_dc.SetTextColor(RGB(255, 255, 255));

		float fX = (float)m_overlay_image.GetWidth() / (float)client_rect.Width();
		float fY = (float)m_overlay_image.GetHeight() / (float)(client_rect.Height() /*- DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/);

		CRect text_rect(client_rect);
		text_rect.top = client_rect.bottom/* - DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/;
		CPoint pos;
		GetCursorPos(&pos);
		ScreenToClient(&pos);
		COLORREF color = GetPixelValue((int)(pos.x * fX), (int)(pos.y * fY));
		DWORD r = GetRValue(color);
		DWORD g = GetGValue(color);
		DWORD b = GetBValue(color);
		mem_dc.DrawText(CMyString(_T("RGB(%d, %d, %d)"), r, g, b), text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

void CCapturedRegion::DrawCustomLine(Gdiplus::Graphics& memG, Gdiplus::Rect& rect)
{
	if (!m_is_custom_draw)
		return;

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

	memG.DrawLine(&pen, prev_point, next_point);
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

void CCapturedRegion::OnShortcutCopy(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->CopyToClipboard();
}

void CCapturedRegion::OnShortcutSave(HWND hwnd)
{
	TRACE_JUST();

	if (!hwnd)
		return;

	CCapturedRegion* main = reinterpret_cast<CCapturedRegion*>(CWnd::FromHandle(hwnd));
	main->SaveFile();
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

void CCapturedRegion::CopyToClipboard()
{
	if (m_overlay_image.IsNull())
		return;

	CPaintDC dc(this);
	CDC mem_dc;
	CBitmap mem_bmp;
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
	std::vector<DWORD> shortcut_code_list;
	std::vector<CString> shorcut_command_name_list;
	CShortcutListParser::GetSingletone()->GetShortcutList(GetShortcutTargetName(), &shortcut_code_list, NULL, &shorcut_command_name_list);
	for (int i = 0; i < (int)shorcut_command_name_list.size(); i++)
	{
		if (shorcut_command_name_list[i].Find(_T("Save")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_SAVE, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
		else if (shorcut_command_name_list[i].Find(_T("Erase")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_ERASE, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
		else if (shorcut_command_name_list[i].Find(_T("Reset")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_RESET, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
		else if (shorcut_command_name_list[i].Find(_T("Copy")) >= 0)
			::AppendMenu(m_context_menu, MF_STRING, ID_CAPTURED_REGION_MENU_COPY, CMyString(_T("%s\t%s"), shorcut_command_name_list[i], CShortcutManager::GetSingletone()->MakeShortcutStrinig(shortcut_code_list[i])));
	}
	::InsertMenu(m_context_menu, -1, MF_BYPOSITION, ID_CAPTURED_REGION_MENU_CLOSE, _T("Close"));
}

void CCapturedRegion::OnContextMenuSave()
{
	SaveFile();
}

void CCapturedRegion::OnContextMenuCopy()
{
	CopyToClipboard();
}

void CCapturedRegion::OnContextMenuChangePenColor()
{
	PostMessage(LUM_CONTEXT_MENU_CHANGE_PEN_COLOR);
}

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
	//if (GF()->CheckWindow(GetSafeHwnd()))
	//	PostMessage(WM_CLOSE);

	if (GF()->CheckWindow(GetSafeHwnd()))
		WM_POST_MESSAGE1(UM_CAPTURED_REGION_DELETE, m_id);
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

void CCapturedRegion::SaveFile()
{
	if (m_overlay_image.IsNull())
		return;

	CString file_path;
	if (SaveFileDlgOpen(&file_path))
		m_overlay_image.Save(file_path);
}

void CCapturedRegion::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!GF()->CheckWindow(GetSafeHwnd()))
		return;

	if (GetAsyncKeyState(VK_CONTROL) >= 0 || GetAsyncKeyState(VK_LBUTTON) >= 0)
		m_is_custom_draw = false;

	//if (m_is_custom_draw)
	//{
	//	m_prev_custom_draw_point = m_next_custom_draw_point;
	//	CPoint point;
	//	GetCursorPos(&point);
	//	ScreenToClient(&point);
	//	m_next_custom_draw_point = point;
	//}
	if (!m_overlay_image.IsNull() && GF()->CheckWindow(m_tool_tip.GetSafeHwnd()))
	{
		CRect client_rect;
		GetClientRect(client_rect);
		float fX = (float)m_overlay_image.GetWidth() / (float)client_rect.Width();
		float fY = (float)m_overlay_image.GetHeight() / (float)(client_rect.Height() /*- DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH*/);
		int x = (int)((float)point.x * fX);
		int y = (int)((float)point.y * fY);
		COLORREF color = m_overlay_image.GetPixel(x, y);
		int r = (int)GetRValue(color);
		int g = (int)GetGValue(color);
		int b = (int)GetBValue(color);
		m_tool_tip.UpdateTipText(CMyString(_T("(x, y) = (%d, %d)\r\nRGB = (%d, %d, %d)"), x, y, r, g, b), this);
	}

	Draw();

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CCapturedRegion::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (GetAsyncKeyState(VK_CONTROL) < 0)
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
	if (real_captured_region.Width() >= DEFAULT_CAPTURED_REGION_MIN_WIDTH &&
		real_captured_region.Height() >= DEFAULT_CAPTURED_REGION_MIN_HEIGHT)
	{
		this_rect.right -= DEFAULT_WHEEL_OFFSET_X;
		this_rect.bottom -= DEFAULT_WHEEL_OFFSET_Y;
		//real_captured_region.bottom += DEFAULT_CAPTURED_REGION_BOTTOM_LENGTH;
		MoveWindow(this_rect);
		Draw();
	}
}

void CCapturedRegion::OnWheelUp()
{
	CRect this_rect;
	GetWindowRect(this_rect);
	this_rect.right += DEFAULT_WHEEL_OFFSET_X;
	this_rect.bottom += DEFAULT_WHEEL_OFFSET_Y;
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
	color_dialog.DoModal();
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
