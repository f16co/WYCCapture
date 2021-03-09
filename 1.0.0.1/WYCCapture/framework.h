#pragma once

#ifdef _DEBUG
#include <vld.h>
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include <memory>

#ifndef SHARED_PTR
#if _MSC_VER > 1500		// Heigher than Visual Studio 2008  
#define SHARED_PTR std::shared_ptr
#define DYNAMIC_POINTER_CAST std::dynamic_pointer_cast
#else
#define SHARED_PTR std::tr1::shared_ptr
#define DYNAMIC_POINTER_CAST std::tr1::dynamic_pointer_cast
#endif
#endif

#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")
using namespace Gdiplus;

#include "TraceCollection.h"
#include "UserResources.h"
#include "UserMessages.h"
#include "TraceEx.h"
#include "LocalSystemTime.h"
#include "ShortcutManager.h"
#include "GlobalFunction.h"
#include "GlobalSetting.h"
#include "PerformanceChecker.h"
#include "GlobalInfo.h"
#include "StringUtil.h"
#include "ScopeLock.h"
#include "WMManager.h"
