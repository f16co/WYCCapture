#include "stdafx.h"
#include "RegionCaptureDetailRegion.h"
#include "Monitors.h"
#include "MultiMonitor.h"

#define DEFAULT_REGION_CAPTURE_DETAIL_REGION_COLOR_TEXT_MARGIN	40
#define DEFAULT_REGION_CAPTURE_DETAIL_REGION_WIDTH				500
#define DEFAULT_REGION_CAPTURE_DETAIL_REGION_HEIGHT				(500 + DEFAULT_REGION_CAPTURE_DETAIL_REGION_COLOR_TEXT_MARGIN)

IMPLEMENT_DYNAMIC(CRegionCaptureDetailRegion, CWnd)

BEGIN_MESSAGE_MAP(CRegionCaptureDetailRegion, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CRegionCaptureDetailRegion::CRegionCaptureDetailRegion()
	: m_current_pos(0, 0)
	, m_current_dc(NULL)
{

}

CRegionCaptureDetailRegion::~CRegionCaptureDetailRegion()
{
}

bool CRegionCaptureDetailRegion::Create(CWnd* parent, UINT id)
{
	if (!CWnd::Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), parent, id))
		return false;

	RegisterUserMessages();

	return true;
}

void CRegionCaptureDetailRegion::OnDraw(HDC current_dc, CPoint current_pos)
{
	if (!GF()->CheckWindow(this))
		return;

	if (!current_dc)
		return;

	m_current_dc = current_dc;
	m_current_pos = current_pos;

	Draw();
}

void CRegionCaptureDetailRegion::Show()
{
	if (!GF()->CheckWindow(this))
		return;

	ShowWindow(SW_SHOW);
}

void CRegionCaptureDetailRegion::Hide()
{
	if (!GF()->CheckWindow(this))
		return;

	ShowWindow(SW_HIDE);
}

void CRegionCaptureDetailRegion::MoveWnd()
{
	int monitor_index = GF()->GetMonitorIndexFromPoint(m_current_pos);
	if (!GF()->MonitorIndexIsValid(monitor_index))
		return;

	int quadrant_index = GF()->GetQuadrantIndexFromMonitorIndexAndPoint(monitor_index, m_current_pos);
	if (!GF()->QuadrantIndexIsValid(quadrant_index))
		return;

	CRect region_rect;
	switch (quadrant_index)
	{
	case 0:
		region_rect = GetDetailRegionRect(monitor_index, 2);
		break;
	case 1:
		region_rect = GetDetailRegionRect(monitor_index, 3);
		break;
	case 2:
		region_rect = GetDetailRegionRect(monitor_index, 0);
		break;
	case 3:
		region_rect = GetDetailRegionRect(monitor_index, 1);
		break;
	}
	
	//SetWindowPos(&wndTopMost, region_rect.left, region_rect.top, region_rect.Width(), region_rect.Height(), SWP_SHOWWINDOW);
	MoveWindow(region_rect);
}

CRect CRegionCaptureDetailRegion::GetDetailRegionRect(int monitor_index, int quadrant_index)
{
	CRect region_rect;
	if (GF()->MonitorIndexIsValid(monitor_index) &&
		GF()->QuadrantIndexIsValid(quadrant_index))
	{
		int w = DEFAULT_REGION_CAPTURE_DETAIL_REGION_WIDTH;
		int h = DEFAULT_REGION_CAPTURE_DETAIL_REGION_HEIGHT;

		int divider = 1;

		CMonitors monitors;
		CMonitor monitor = monitors.GetMonitor(monitor_index);

		CRect monitor_rect;
		monitor.GetMonitorRect(monitor_rect);

		switch (quadrant_index)
		{
		case 0:
			region_rect.left = monitor_rect.right - w / divider;
			region_rect.top = monitor_rect.top;
			region_rect.right = monitor_rect.right;
			region_rect.bottom = monitor_rect.top + h / divider;
			break;
		case 1:
			region_rect.left = monitor_rect.left;
			region_rect.top = monitor_rect.top;
			region_rect.right = monitor_rect.left + w / divider;
			region_rect.bottom = monitor_rect.top + h / divider;
			break;
		case 2:
			region_rect.left = monitor_rect.left;
			region_rect.top = monitor_rect.bottom - h / divider;
			region_rect.right = monitor_rect.left + w / divider;
			region_rect.bottom = monitor_rect.bottom;
			break;
		case 3:
			region_rect.left = monitor_rect.right - w / divider;
			region_rect.top = monitor_rect.bottom - h / divider;
			region_rect.right = monitor_rect.right;
			region_rect.bottom = monitor_rect.bottom;
			break;
		}
	}

	return region_rect;
}

void CRegionCaptureDetailRegion::RegisterUserMessages()
{
}

void CRegionCaptureDetailRegion::OnPaint()
{
	if (!GF()->CheckWindow(this))
		return;

	CPaintDC dc(this);

	CRect client_rect;
	GetClientRect(client_rect);

	CDC mem_dc;
	CBitmap mem_bmp;

	mem_dc.CreateCompatibleDC(&dc);
	mem_bmp.CreateCompatibleBitmap(&dc, client_rect.Width(), client_rect.Height());

	mem_dc.SelectObject(mem_bmp);
	mem_dc.FillSolidRect(client_rect, RGB(0, 0, 0));
	int divider = 10;
	int w = DEFAULT_REGION_CAPTURE_DETAIL_REGION_WIDTH;
	int h = DEFAULT_REGION_CAPTURE_DETAIL_REGION_HEIGHT - DEFAULT_REGION_CAPTURE_DETAIL_REGION_COLOR_TEXT_MARGIN;

	w /= divider;
	h /= divider;

	int x = m_current_pos.x - w / 2;
	int y = m_current_pos.y - h / 2;

	CSize total_monitor_size = GF()->GetTotalMonitorSize();
	CDC temp_dc;
	CBitmap temp_bmp;
	CRect temp_rect;
	temp_rect.left = 0;
	temp_rect.top = 0;
	temp_rect.right = total_monitor_size.cx;
	temp_rect.bottom = total_monitor_size.cy;
	temp_dc.CreateCompatibleDC(&dc);
	temp_bmp.CreateCompatibleBitmap(&dc, temp_rect.Width(), temp_rect.Height());
	temp_dc.SelectObject(temp_bmp);

	::BitBlt(
		temp_dc,
		0, 0, temp_rect.Width(), temp_rect.Height(),
		m_current_dc,
		0, 0,
		SRCCOPY);

	DrawCrossLine(temp_dc, m_current_pos, 0, 0, temp_rect.right, temp_rect.bottom);

	mem_dc.FillSolidRect(client_rect, RGB(255, 255, 255));

	mem_dc.SetStretchBltMode(COLORONCOLOR);
	::StretchBlt(
		mem_dc,
		1, 1, client_rect.Width() - 2, client_rect.Height() - DEFAULT_REGION_CAPTURE_DETAIL_REGION_COLOR_TEXT_MARGIN - 2,
		temp_dc,
		x, y, w, h,
		SRCCOPY);

	DrawText(mem_dc, client_rect);

	dc.BitBlt(
		0, 0, client_rect.Width(), client_rect.Height(),
		&mem_dc,
		0, 0,
		SRCCOPY);
}

void CRegionCaptureDetailRegion::Draw()
{
	if (!GF()->CheckWindow(this))
		return;

	MoveWnd();
	Invalidate();
}

void CRegionCaptureDetailRegion::DrawText(CDC& mem_dc, CRect& client_rect)
{
	CFont font;
	if (font.CreateFont(15, 8, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial")))
	{
		mem_dc.SelectObject(font);
		mem_dc.SetTextColor(RGB(255, 255, 255));

		CRect text_rect(client_rect);
		text_rect.left += 1;
		text_rect.top = client_rect.bottom - DEFAULT_REGION_CAPTURE_DETAIL_REGION_COLOR_TEXT_MARGIN;
		text_rect.right -= 1;
		text_rect.bottom -= 1;

		COLORREF color = ::GetPixel(m_current_dc, m_current_pos.x, m_current_pos.y);
		DWORD r = GetRValue(color);
		DWORD g = GetGValue(color);
		DWORD b = GetBValue(color);
		mem_dc.FillSolidRect(text_rect, RGB(0, 0, 0));
		mem_dc.DrawText(CMyString(_T("Point : [%d, %d] | Region : [%d x %d] | RGB : [%d, %d, %d]"), m_current_pos.x, m_current_pos.y, m_region_rect.Width(), m_region_rect.Height(), r, g, b), text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

void CRegionCaptureDetailRegion::DrawCrossLine(CDC& mem_dc, CPoint& point, int left, int top, int right, int bottom)
{
	Gdiplus::Graphics memG(mem_dc);

	if (true)
	{
		Gdiplus::Point horizontal_start_point(left, point.y);
		Gdiplus::Point horizontal_end_point(right, point.y);
		Gdiplus::Point vertical_start_point(point.x, top);
		Gdiplus::Point vertical_end_point(point.x, bottom);

		Gdiplus::Pen pen(Gdiplus::Color(127, 255, 0, 255));

		memG.DrawLine(&pen, horizontal_start_point, horizontal_end_point);
		memG.DrawLine(&pen, vertical_start_point, vertical_end_point);
	}

	if (true)
	{
		Gdiplus::SolidBrush brush(Gdiplus::Color(127, 255, 255, 0));
		Gdiplus::Rect rect(point.x, point.y, 1, 1);
		memG.FillRectangle(&brush, rect);
	}
}
