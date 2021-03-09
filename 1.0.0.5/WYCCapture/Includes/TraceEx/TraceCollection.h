#pragma once
#include "TraceEx.h"

#ifdef _TRACEEX
#pragma warning(push)
#pragma warning(disable: 4005)
#define TRACE CTraceEx(_T(__FILE__), _T(__FUNCTION__), __LINE__, true, GS()->GetDebugLogUse())
#pragma warning(pop)
#endif

#ifndef TRACE_INT
#define TRACE_INT(value) TRACE(_T("INT value = %d\n"), value);
#endif

#ifndef TRACE_INT2
#define TRACE_INT2(value, string) TRACE(_T("%s = %d\n"), string, value);
#endif

#ifndef TRACE_UINT
#define TRACE_UINT(value) TRACE(_T("UINT value = %d\n"), value);
#endif

#ifndef TRACE_UINT2
#define TRACE_UINT2(value, string) TRACE(_T("%s = %d\n"), string, value);
#endif

#ifndef TRACE_INT64
#define TRACE_INT64(value) TRACE(_T("INT64 value = %I64d\n"), value);
#endif

#ifndef TRACE_INT64_2
#define TRACE_INT64_2(value, string) TRACE(_T("%s = %I64d\n"), string, value);
#endif

#ifndef TRACE_UINT64
#define TRACE_UINT64(value) TRACE(_T("UINT64 value = %I64d\n"), value);
#endif

#ifndef TRACE_UINT64_2
#define TRACE_UINT64_2(value, string) TRACE(_T("%s = %I64d\n"), string, value);
#endif

#ifndef TRACE_FLOAT
#define TRACE_FLOAT(value) TRACE(_T("FLOAT value = %.2f\n"), value);
#endif

#ifndef TRACE_FLOAT2
#define TRACE_FLOAT2(value, string) TRACE(_T("%s = %.2f\n"), string, value);
#endif

#ifndef TRACE_DOUBLE
#define TRACE_DOUBLE(value) TRACE(_T("DOUBLE value = %.5lf\n"), value);
#endif

#ifndef TRACE_DOUBLE2
#define TRACE_DOUBLE2(value, string) TRACE(_T("%s = %.5lf\n"), string value);
#endif

#ifndef TRACE_COLOR
#define TRACE_COLOR(color) TRACE(_T("RGB = (%d, %d, %d)\n"), GetRValue(color), GetGValue(color), GetBValue(color));
#endif

#ifndef TRACE_COLOR2
#define TRACE_COLOR2(color, string) TRACE(_T("%s = (%d, %d, %d)\n"), string, GetRValue(color), GetGValue(color), GetBValue(color));)
#endif

#ifndef TRACE_RECT
#define TRACE_RECT(rect) TRACE(_T("rect = (%d, %d, %d, %d)\n"), rect.left, rect.top, rect.Width(), rect.Height());
#endif

#ifndef TRACE_RECT2
#define TRACE_RECT2(rect, string) TRACE(_T("%s = (%d, %d, %d, %d)\n"), string, rect.left, rect.top, rect.Width(), rect.Height());
#endif

#ifndef TRACE_POINT
#define TRACE_POINT(point) TRACE(_T("point = (%d, %d)\n"), point.x, point.y);
#endif

#ifndef TRACE_POINT2
#define TRACE_POINT2(point, string) TRACE(_T("%s = (%d, %d)\n"), string, point.x, point.y);
#endif

#ifndef TRACE_SIZE
#define TRACE_SIZE(size) TRACE(_T("size = (%d, %d)\n"), size.cx, size.cy);
#endif

#ifndef TRACE_SIZE2
#define TRACE_SIZE2(size, string) TRACE(_T("%s = (%d, %d)\n"), string, size.cx, size.cy);
#endif

#ifndef TRACE_DC
#define TRACE_DC(hdc, file_name) CTraceEx::TraceDC(hdc, file_name);
#endif

#ifndef TRACE_DC2
#define TRACE_DC2(hdc, width, height, file_name) CTraceEx::TraceDC(hdc, width, height, file_name);
#endif

#ifndef TRACE_STR
#define TRACE_STR(str) TRACE(_T("%s\n"), str);
#endif

#ifndef TRACE_JUST
#define TRACE_JUST() TRACE(_T("\n"));
#endif
