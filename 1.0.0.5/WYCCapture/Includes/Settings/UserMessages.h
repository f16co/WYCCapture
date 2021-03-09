#pragma once
#include <afxwin.h>

enum 
{
	UM_SIGNALING = WM_USER + 1,
	UM_WANT_TO_GET_SHORTCUT_LIST,	// WPARAM: CListCtrl**
	UM_SHORTCUT_CHANGED,
	UM_REGION_CAPTURE_PAUSE,
	UM_REGION_CAPTURE_START,
	UM_REGION_CAPTURE_STOP,			// WPARAM: LPREGION_CAPTURE_STOP (must delete in receiver side)
	UM_WINDOW_CAPTURE_PAUSE,
	UM_WINDOW_CAPTURE_START,
	UM_WINDOW_CAPTURE_STOP,			// WPARAM: LPWINDOW_CAPTURE_STOP (must delete in receiver side)
	UM_CAPTURED_REGION_DELETE,		// WPARAM: int m_id
};

typedef struct _REGION_CAPTURE_STOP
{
	CPoint start_pos;
	CPoint end_pos;
	HDC current_dc;
	int current_monitor_index;

	bool IsValid()
	{
		return (this->start_pos.x >= 0 &&
				this->start_pos.y >= 0 &&
				this->end_pos.x >= 0 &&
				this->end_pos.y >= 0 &&
				this->current_dc != NULL &&
				this->current_monitor_index >= 0);
	}
	_REGION_CAPTURE_STOP()
		: start_pos(-1, -1)
		, end_pos(-1, -1)
		, current_dc(NULL)
		, current_monitor_index(-1)
	{
	}
	~_REGION_CAPTURE_STOP()
	{
	}
	_REGION_CAPTURE_STOP(CPoint& start_pos, CPoint& end_pos, HDC current_dc, int current_monitor_index)
	{
		this->start_pos = start_pos;
		this->end_pos = end_pos;
		this->current_dc = current_dc;
		this->current_monitor_index = current_monitor_index;
	}
	_REGION_CAPTURE_STOP& operator=(const _REGION_CAPTURE_STOP& r)
	{
		this->start_pos = r.start_pos;
		this->end_pos = r.end_pos;
		this->current_dc = r.current_dc;
		this->current_monitor_index = r.current_monitor_index;

		return *this;
	}
	bool operator==(const _REGION_CAPTURE_STOP& r)
	{
		return (this->start_pos == r.start_pos &&
				this->end_pos == r.end_pos &&
				this->current_dc == r.current_dc &&
				this->current_monitor_index == r.current_monitor_index);
	}
	bool operator!=(const _REGION_CAPTURE_STOP& r)
	{
		return !(*this == r);
	}
} REGION_CAPTURE_STOP, *LPREGION_CAPTURE_STOP;

typedef struct _WINDOW_CAPTURE_STOP
{
	CRect captured_region;
	HDC current_dc;
	int current_monitor_index;

	bool IsValid()
	{
		return (this->captured_region.left >= 0 &&
				this->captured_region.top >= 0 &&
				this->captured_region.right >= 0 &&
				this->captured_region.bottom >= 0 &&
				this->current_dc != NULL &&
				this->current_monitor_index >= 0);
	}
	_WINDOW_CAPTURE_STOP()
		: captured_region(-1, -1, -1, -1)
		, current_dc(NULL)
		, current_monitor_index(-1)
	{
	}
	~_WINDOW_CAPTURE_STOP()
	{
	}
	_WINDOW_CAPTURE_STOP(CRect& captured_region, HDC current_dc, int current_monitor_index)
	{
		this->captured_region = captured_region;
		this->current_dc = current_dc;
		this->current_monitor_index = current_monitor_index;
	}
	_WINDOW_CAPTURE_STOP(const _WINDOW_CAPTURE_STOP& r)
	{
		this->captured_region = r.captured_region;
		this->current_dc = r.current_dc;
		this->current_monitor_index = r.current_monitor_index;
	}
	_WINDOW_CAPTURE_STOP& operator=(const _WINDOW_CAPTURE_STOP& r)
	{
		if (this == &r)
			return *this;

		this->captured_region = r.captured_region;
		this->current_dc = r.current_dc;
		this->current_monitor_index = r.current_monitor_index;

		return *this;
	}
	bool operator==(const _WINDOW_CAPTURE_STOP& r)
	{
		return (this->captured_region == r.captured_region &&
				this->current_dc == r.current_dc &&
				this->current_monitor_index == r.current_monitor_index);
	}
	bool operator!=(const _WINDOW_CAPTURE_STOP& r)
	{
		return !(*this == r);
	}
} WINDOW_CAPTURE_STOP, *LPWINDOW_CAPTURE_STOP;;