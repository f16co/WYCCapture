#include "stdafx.h"
#include "StringUtil.h"

namespace str_util
{
HMODULE LoadLibrary( LPCTSTR dll_dir, LPCTSTR dll_name )
{
	HMODULE lib = 0;

	CString dbg;

	// 지정한 폴더에서 검색
	if( dll_dir && _tcscmp(dll_dir, _T("")) )
	{
		CString dir( dll_dir );
		CString tmp( dll_dir );
		int index = tmp.ReverseFind( _T('\\') );
		if( index >= 0 )
		{
			if( index == tmp.GetLength() - 1 )
				dir = tmp.Left( index );
		}

		CMyString path( _T("%s\\%s"), dir, dll_name );

		lib = ::LoadLibrary( path );
		if( lib )
			return lib;
		else
		{
			CMyString msg( _T("LoadLibrary: %s, %d"), path, GetLastError() );
			AfxMessageBox( msg );
		}
	}

	// calling process가 위치한 폴더에서 검색
	CString module_path;
	if( ModuleDir(0, module_path) )
	{
		CString path;
		path.Format( _T("%s\\%s"), module_path, dll_name );
		lib = ::LoadLibrary( path );
		if( lib )
			return lib;
	}

	// current, system 폴더에서 검색
	lib = ::LoadLibrary( dll_name );
	return lib;
}

void FreeLibrary( HMODULE lib )
{
	if( lib )
		::FreeLibrary( lib );
}

CString	ModuleDir( HMODULE lib )
{
	CString dir = _T("");
	TCHAR path[512] = { 0, };
	int length = ::GetModuleFileName( lib, path, sizeof(path) );
	if( length <= 0 )
		return dir;
	ExtractFileDir( path, dir );
	return dir;
}

BOOL ModuleDir( HMODULE lib, CString& dir )
{
	TCHAR path[512] = { 0, };
	int length = ::GetModuleFileName( lib, path, sizeof(path) );
	if( length <= 0 )
		return FALSE;
	return ExtractFileDir( path, dir );
}

CString ModuleName( HMODULE lib )
{
	CString name = _T("");
	TCHAR path[512] = { 0, };
	int length = ::GetModuleFileName( lib, path, sizeof(path) );
	if( length <= 0 )
		return name;
	ExtractFileName( path, name );
	return name;
}

BOOL ModuleName( HMODULE lib, CString& name )
{
	TCHAR path[512] = { 0, };
	int length = ::GetModuleFileName( lib, path, sizeof(path) );
	if( length <= 0 )
		return FALSE;
	return ExtractFileName( path, name );
}

BOOL ExtractFileDir( LPCTSTR full_path, CString& file_dir )
{
	TCHAR drive[MAX_PATH] = { 0, };
	TCHAR dir[MAX_PATH] = { 0, };
	TCHAR file[MAX_PATH] = { 0, };
	TCHAR ext[MAX_PATH] = { 0, };
	_tsplitpath(full_path, drive, dir, file, ext);
	if ((_tcslen(dir) > 0) && dir[0] != _T('\\'))
		file_dir.Format( _T("%s\\%s"), drive, dir );
	else
		file_dir.Format( _T("%s%s"), drive, dir );
	return TRUE;
}

BOOL ExtractFileName( LPCTSTR full_path, CString& file_name )
{
	TCHAR drive[MAX_PATH] = { 0, };
	TCHAR dir[MAX_PATH] = { 0, };
	TCHAR file[MAX_PATH] = { 0, };
	TCHAR ext[MAX_PATH] = { 0, };
	_tsplitpath( full_path, drive, dir, file, ext );
	file_name.Format( _T("%s%s"), file, ext );
	return TRUE;
}

BOOL ExtractFileExt( LPCTSTR full_path, CString& file_ext )
{
	TCHAR drive[MAX_PATH] = { 0, };
	TCHAR dir[MAX_PATH] = { 0, };
	TCHAR file[MAX_PATH] = { 0, };
	TCHAR ext[MAX_PATH] = { 0, };
	_tsplitpath(full_path, drive, dir, file, ext);
	file_ext = ext + 1;
	return TRUE;
}

BOOL ExtractRelativeFileName( CString full_path, CString base_path, CString& file_name )
{
	file_name = full_path.Right( full_path.GetLength() - base_path.GetLength() - 1 );
	return TRUE;
}

BOOL ExtractParentDir(CString full_path, CString& parent_dir)
{
	CString tmp;
	if ( !ExtractFileDir( full_path, tmp ) )
		return FALSE;

	tmp = tmp.Left(tmp.GetLength()-1);

	int pos = tmp.ReverseFind(_T('\\'));
	if (pos < 0)
		return FALSE;

	parent_dir = tmp.Left(pos);	
	return TRUE;
}

CString PathCombine( CString strA, CString strB )
{
	CString strResult;

	if ((strA.GetLength() > 0) && 
		((strA[strA.GetLength() - 1] == _T('\\')) || (strA[strA.GetLength() - 1] == _T('/'))))
		strResult.Format(_T("%s%s"), strA, strB);
	else
		strResult.Format(_T("%s\\%s"), strA, strB);
	return strResult;
}

void MyTrace( LPCTSTR file, int line, LPCTSTR format, ... )
{
	va_list ap;
	va_start( ap, format );
	CString user;
	user.FormatV( format, ap );
	va_end( ap );
}

void ExtractDecimal( int decimal, int extract[], int count )
{
	int decimal2 = decimal;
	for( int i=0; i<count; i++ )
	{
		int mod = decimal2 % 10;
		extract[count - i - 1] = mod;
		decimal2 /= 10;
	}
}

int UrlEncode( LPTSTR source, LPTSTR dest )
{
	dest[0] = 0;
	TCHAR hex[4];
	LPTSTR sbuf = ( LPTSTR )dest;
	int size = 0;

	while(*source)
	{
		if( (*source > 47 && *source < 57) || (*source > 64 && *source < 92) || (*source > 96 && *source < 123) || *source == _T('-') || *source == _T('.') || *source == _T('_') )
		{
			*sbuf = *source;
		}
		else
		{
			_stprintf_s( hex, ARRAYSIZE(hex), _T("%%%02X"), *source );
			_tcsncat( sbuf, hex, 3 );
			*sbuf++;
			*sbuf++;
			size += 2;
		}
		*source++;
		*sbuf++;
		size++;
	}
	return size;
}

int UrlDecode( LPTSTR source, LPTSTR dest )
{
	dest[0] = 0;
	int num=0, i, index=0;
	int retval=0;
	while(*source)
	{
		if (*source == _T('%'))
		{
			num = 0;
			retval = 0;
			for (i = 0; i < 2; i++)
			{
				*source++;
				if (*(source) < _T(':'))
					num = *(source) - 48;
				else if (*(source) > _T('@') && *(source) < _T('['))
					num = (*(source) - _T('A'))+10;
				else
					num = (*(source) - _T('a'))+10;
				
				if ((16*(1-i)))
					num = (num*16);
				retval += num;
			}
			dest[index] = retval;
			index++;
		}
		else
			index++;
		*source++;
	}
	return index;
}

void a2w( LPSTR str, CStringW &res )
{
	USES_CONVERSION;
	res = A2W( str );
}

void w2a( LPWSTR str, CStringA &res )
{
	USES_CONVERSION;
	res = W2A( str );
}

CStringW a2w(LPSTR str )
{
	USES_CONVERSION;
	return A2W( str );
}

CStringA w2a( LPWSTR str )
{
	USES_CONVERSION;
	return W2A( str );
}

int atoi( LPCTSTR str )
{
	CString a = str;
	CStrList b( str, _T(",") );
	CString c;
	for( int i=0; i<b.Count(); i++ )
		c += b[i];
	c.Remove( _T(' ') );
	return _ttoi( c.GetBuffer() );
}
//
//LPCTSTR skip_space( LPCTSTR str )
//{
//	while( str && *str && (*str > 0) && isspace(*str) )
//		str++;
//	return str;
//}

BOOL is_inlist( TCHAR ch, LPCTSTR list )
{
	int count = (int)_tcslen( list );
	for( int i=0; i<count; i++ )
	{
		if( list[i] == ch )
			return TRUE;
	}
	return FALSE;
}

LPCTSTR search_ch( LPCTSTR str, LPCTSTR list )
{
	while( str && *str )
	{
		if( is_inlist(*str, list) )
			return str;
		str++;
	}
	return NULL;
}

LPCTSTR search_ch_r( LPCTSTR str, LPCTSTR list )
{
	int index = (int)_tcslen( str ) - 1;
	for( int i=index; i>=0; i-- )
	{
		if( is_inlist(str[i], list) )
			return str;
	}
	return NULL;
}

LPCTSTR search_non_ch( LPCTSTR str, LPCTSTR list )
{
	while( str && *str )
	{
		if( !is_inlist(*str, list) )
			return str;
		str++;
	}
	return NULL;
}

LPCTSTR search_non_ch_r( LPCTSTR str, LPCTSTR list )
{
	int index = (int)_tcslen( str ) - 1;
	for( int i=index; i>=0; i-- )
	{
		if( !is_inlist(str[i], list) )
			return str;
	}
	return NULL;
}

void CopyStringList( CStringList& dst, CStringList& src )
{
	dst.RemoveAll( );
	POSITION pos = src.GetHeadPosition( );
	while( pos )
		dst.AddTail( src.GetNext(pos) );
}

BOOL ParseSetup( CString& str, CStrList& list, CToken*& tok )
{
	if( list.Extract(str, _T("&")) <= 0 )
		return FALSE;
	tok = new CToken[ list.Count() ];
	for( int i=0; i<list.Count(); i++ )
		tok[i].Extract( list[i], _T("=") );
	return TRUE;
}

CString& GetValue( CToken* tok, int tok_count, LPCTSTR name )
{
	static CString null;
	if( !tok )
		return null;
	for( int i=0; i<tok_count; i++ )
	{
		if( !tok[i].m_name.CompareNoCase(name) )
			return tok[i].m_value;
	}
	return null;
}

CToken* GetToken( CToken* tok, int tok_count, LPCTSTR name )
{
	if( !tok )
		return NULL;
	for( int i=0; i<tok_count; i++ )
	{
		if( !tok[i].m_name.CompareNoCase(name) )
			return &tok[i];
	}
	return NULL;
}

BOOL SetValue( CToken* tok, int tok_count, LPCTSTR name, LPCTSTR val )
{
	if( !tok )
		return FALSE;
	for( int i=0; i<tok_count; i++ )
	{
		if( !tok[i].m_name.CompareNoCase(name) )
		{
			tok[i].m_value = val;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SetupToString( CString& str, CToken* tok, int tok_count, int setup_code )
{
	str = CMyString( _T("setup_code=%d"), setup_code );
	for( int i=0; i<tok_count; i++ )
	{
		str += _T("&") + tok[i].m_name + "=" + tok[i].m_value;
		//TRACE( "%s:%s\n", tok[i].Name(), tok[i].Value() );
	}
	return TRUE;
}

void PrintStrList( CStrList& list )
{
	for( int i=0; i<list.Count(); i++ )
		TRACE( _T("%s\n"), list[i] );
}

CString XmlEscapeCharsTo(CString strDecoded)
{
	CString strResult;
	for(int nLoopCnt = 0; nLoopCnt < strDecoded.GetLength(); nLoopCnt++)
	{
		switch(strDecoded[nLoopCnt])
		{
		case _T('&'):
			strResult += _T("&amp;");
			break;
		case _T('\''):
			strResult += _T("&quot;");
			break;
		case _T('\"'):
			strResult += _T("&apos;");
			break;
		case _T('<'):
			strResult += _T("&lt;");
			break;
		case _T('>'):
			strResult += _T("&gt;");
			break;
		default:
			strResult += strDecoded[nLoopCnt];
			break;
		}
	}

	return strResult;
}

CString XmlEscapeCharsFrom(CString strEncoded)
{
	CString strResult;
	BOOL bEscapeString = FALSE;
	CString strEscapeString;
	for(int nLoopCnt = 0; nLoopCnt < strEncoded.GetLength(); nLoopCnt++)
	{
		switch(strEncoded[nLoopCnt])
		{
		case _T('&'):
			if (bEscapeString)
				strResult += _T("&") + strEscapeString;
			bEscapeString = TRUE;
			strEscapeString.Empty();
			break;
		case _T(';'):
			if (bEscapeString)
			{
				if (strEscapeString == _T("amp"))
					strResult += _T("&");
				else if (strEscapeString == _T("quot"))
					strResult += _T("\"");
				else if (strEscapeString == _T("apos"))
					strResult += _T("'");
				else if (strEscapeString == _T("lt"))
					strResult += _T("<");
				else if (strEscapeString == _T("gt"))
					strResult += _T(">");
				else
					strResult += _T("&") + strEscapeString + _T(";");
				bEscapeString = FALSE;
			}
			else
				strResult += strEncoded[nLoopCnt];
			break;
		default:
			if (bEscapeString)
				strEscapeString += strEncoded[nLoopCnt];
			else
				strResult += strEncoded[nLoopCnt];
			break;
		}
	}

	if (bEscapeString)
		strResult += _T("&") + strEscapeString;

	return strResult;
}


BOOL IsDigit( CString& str )
{
	int count = str.GetLength( );
	int i = 0;
	for( i=0; i<count; i++ )
	{
		TCHAR temp = str.GetAt( i );
		if( i==0 && temp==_T('0') )
			break;
		if( temp >= _T('0') && temp <= _T('9') )
			continue;
		else
			break;
	}
	if( i != count )
		return FALSE;
	return TRUE;
}

CStringA UrlEncode(CStringA strUrl)
{
	CStringA strCode = "0123456789abcdef";
	CStringA strResult;
	for(int nLoopCnt = 0; nLoopCnt < strUrl.GetLength(); nLoopCnt++)
	{
		if (strUrl[nLoopCnt] >= 0 &&
			(isalnum(strUrl[nLoopCnt]) ||
			(strUrl[nLoopCnt] == _T('-')) ||
			(strUrl[nLoopCnt] == _T('_')) ||
			(strUrl[nLoopCnt] == _T('.')) ||
			(strUrl[nLoopCnt] == _T('~'))))
			strResult += strUrl[nLoopCnt];
		else if (strUrl[nLoopCnt] == _T(' '))
			strResult += _T('+');
		else
		{
			strResult += _T('%');
			strResult += strCode[(strUrl[nLoopCnt]>>4) & 0x0F];
			strResult += strCode[strUrl[nLoopCnt] & 0x0F];
		}
	}
	return strResult;
}


#define URL_CHAR_FROM(x) (isdigit(x) ? x - '0' : tolower(x) - 'a' + 10)

CStringA UrlDecode(LPCTSTR LPTSTRUrl)
{
	return UrlDecode(CStringA(LPTSTRUrl));
}

CStringA UrlDecode(CStringA strUrl)
{
	CStringA strResult;
	for(int nLoopCnt = 0; nLoopCnt < strUrl.GetLength(); nLoopCnt++)
	{
		if (strUrl[nLoopCnt] == '%')
		{
			char cCode1 = strUrl[++nLoopCnt];
			char cCode2 = strUrl[++nLoopCnt];
			strResult += (char)(((URL_CHAR_FROM(cCode1) << 4) & 0xF0) | (URL_CHAR_FROM(cCode2) & 0x0F));
		}
		else if (strUrl[nLoopCnt] == '+')
			strResult += ' ';
		else
			strResult += strUrl[nLoopCnt];
	}
	return strResult;
}

static RS_TAB g_rs1_tab[] =
{
	{ _T("="), _T(";EQUAL;") },
	{ _T("&"), _T(";AMP;") },
	{ _T("<"), _T(";LBRACKET;") },
	{ _T(">"), _T(";RBRACKET;") },
	{ _T("'"), _T(";APOS;") },
	{ _T("\""), _T(";QUOT;") },
};

static RS_TAB g_rs1r_tab[] =
{
	{ _T(";EQUAL;"), _T("=") },
	{ _T(";AMP;"), _T("&") },
	{ _T(";LBRACKET;"), _T("<") },
	{ _T(";RBRACKET;"), _T(">") },
	{ _T(";APOS;"), _T("'") },
	{ _T(";QUOT;"), _T("\"") },
};

static RS_TAB g_rs1x_tab[] =
{
	{ _T("&"), _T("&amp;") },
	{ _T("<"), _T("&lt;") },
	{ _T(">"), _T("&gt;") },
	{ _T("'"), _T("&apos;") },
	{ _T("\""), _T("&quot;") },
};

int ReplaceStr( CString& str, RS_TAB* tab, int entry_count )
{
	int n = 0;
	for( int i=0; i<entry_count; i++ )
		n += str.Replace( tab[i].m_key, tab[i].m_val );
	return n;
}

int ReplaceStr1( CString& str )	// "=&<>""" => ;...;
{
	return ReplaceStr( str, g_rs1_tab, sizeof(g_rs1_tab)/sizeof(RS_TAB) );
}

int ReplaceStr1R( CString& str )	// ;...; => "=&<>"""
{
	return ReplaceStr( str, g_rs1r_tab, sizeof(g_rs1r_tab)/sizeof(RS_TAB) );
}

int ReplaceStr1X( CString& str )	// "&<>""" => &amp; &lt; &gt; &apos; &quot;
{
	return ReplaceStr( str, g_rs1x_tab, sizeof(g_rs1x_tab)/sizeof(RS_TAB) );
}


CString UTF8ToString( CStringA str )
{
	if (str.GetLength() <= 0)
		return CString(str);

	CString strResult;
	int nLength = MultiByteToWideChar(CP_UTF8, 0, str.GetBuffer(), -1, NULL, 0);
	if (nLength > 0)
	{
		CStringW strTemp;
		MultiByteToWideChar(CP_UTF8, 0, str.GetBuffer(), -1, strTemp.GetBuffer(nLength), nLength);
		strResult = strTemp;
		strTemp.ReleaseBuffer();
	}

	return strResult;
}

CStringA StringToUTF8(CString strText)
{
	CStringA strResult;
	USES_CONVERSION;
	CStringW strTextW(strText);

	// Utf-8 로 인코딩 
	int nLength = WideCharToMultiByte(CP_UTF8, 0, strTextW, -1, NULL, 0, NULL, NULL);
	char* pBuffer = new char[nLength];
	WideCharToMultiByte(CP_UTF8, 0, strTextW, -1, pBuffer, nLength, NULL, NULL);
	strResult = pBuffer;
	delete[] pBuffer;
	return strResult;
}

}


//
//CMyString::CMyString( BOOL alert, LPCTSTR format, ... )
//{
//	va_list ap;
//	va_start( ap, format );
//	FormatV( format, ap );
//	va_end( ap );
//	if( alert )
//		AfxMessageBox( (LPCTSTR)this );
//}

CMyString::CMyString( int count, LPCTSTR str )
{
	if( count < 8192 )
	{
		TCHAR buf[8192];
		_tcsncpy( buf, str, count );
		buf[count] = 0;
		*this = buf;
	}
	else
	{
		TCHAR* buf = new TCHAR[count+1];
		_tcsncpy( buf, str, count );
		buf[count] = 0;
		*this = buf;
		delete [] buf;
	}
}

CMyString::CMyString( LPCTSTR format, ... )
{
	va_list ap;
	va_start( ap, format );
	FormatV( format, ap );
	va_end( ap );
}

CMyString::CMyString( CWnd &wnd )
{
	if (wnd.GetSafeHwnd())
		wnd.GetWindowText( *this );
}

CMyString::CMyString( CWnd *wnd )
{
	if (wnd->GetSafeHwnd())
		wnd->GetWindowText( *this );
}

CMyString::CMyString( CComboBox &combo )
{
	if (combo.GetSafeHwnd())
	{
		if( combo.GetCurSel() >= 0 )
			combo.GetLBText( combo.GetCurSel(), *this );
		else
			combo.GetWindowText( *this );
	}
}


CString CMyStringList::m_null;

CString& CMyStringList::operator[]( int index )
{
	POSITION pos = FindIndex( index );
	if( !pos )
		return m_null;
	return GetAt( pos );
}

CString CStrList::m_null;

CStrList::CStrList( )
{
}

CStrList::CStrList( LPCTSTR str, LPCTSTR delimiter )
{
	Extract( str, delimiter );
}

CStrList::~CStrList( )
{
}

int CStrList::Extract( LPCTSTR str, LPCTSTR delimiter )
{
	m_array.RemoveAll( );
	LPCTSTR p = str;
	while( TRUE )
	{
		LPCTSTR e = _tcsstr( p, delimiter );
		if( !e )
		{
			if( _tcslen(p) > 0 )
				m_array.Add( p );
			break;
		}
		if( p != e )
			m_array.Add( CString(p, (int)(e-p)) );
		p = e + _tcslen( delimiter );
	}
	return (int)m_array.GetCount( );
}

int CStrList::Count( )
{
	return (int)m_array.GetCount( );
}

int CStrList::Find( LPCTSTR str )
{
	int count = (int)m_array.GetCount( );
	for( int i=0; i<count; i++ )
	{
		if( !m_array[i].CompareNoCase(str) )
			return i;
	}
	return -1;
}

void CStrList::RemoveAll( )
{
	m_array.RemoveAll( );
}

void CStrList::RemoveAt(int index)
{
	m_array.RemoveAt(index);
}

void CStrList::PushBack( LPCTSTR str )
{
	m_array.Add( str );
}

void CStrList::Print( )
{
	for( int i=0; i<m_array.GetCount(); i++ )
		TRACE( _T("%s\n"), m_array[i] );
}

CString& CStrList::operator[]( int index )
{
	if( index >= m_array.GetCount() )
		return m_null;
	return m_array[index];
}

CStrList& CStrList::operator=( const CStrList& r )
{
	m_array.Copy( r.m_array );
	return *this;
}

BOOL CStrList::operator==(const CStrList& r)
{
	if (m_array.GetCount() != r.m_array.GetCount())
		return FALSE;
	
	for (int i=0; i<m_array.GetCount(); i++)
	{
		if (m_array[i] != r.m_array[i])
			return FALSE;
	}

	return TRUE;
}

BOOL CStrList::operator!=(const CStrList& r)
{
	return !(*this == r);
}

CString CStrList::ToString( LPCTSTR delimiter )
{
	CString strResult;
	for (int i=0; i<m_array.GetCount(); i++)
	{
		if (!strResult.IsEmpty())
			strResult += delimiter;
		strResult += m_array[i];
	}
	return strResult;
}

CStrListEx::CStrListEx( )
{
}

CStrListEx::CStrListEx( LPCTSTR str, LPCTSTR delimiter )
{
	Extract( str, delimiter );
}

CStrListEx::~CStrListEx( )
{
}

int CStrListEx::Extract( LPCTSTR str, LPCTSTR delimiter )
{
	m_array.RemoveAll( );
	LPCTSTR p = str;
	while( TRUE )
	{
		LPCTSTR e = _tcsstr( p, delimiter );
		if( !e )
		{
			if( _tcslen(p) > 0 )
				m_array.Add( p );
			else
				m_array.Add( _T("") );
			break;
		}
		if( p != e )
			m_array.Add( CString(p, (int)(e-p)) );
		else
			m_array.Add( _T("") );
		p = e + _tcslen( delimiter );
	}
	return (int)m_array.GetCount( );
}



CString CToken::m_null;

CToken::CToken( )
{
}

CToken::CToken( LPCTSTR name, LPCTSTR value )
{
	m_name = name;
	m_value = value;
}

CToken::~CToken( )
{
}

void CToken::Extract( const CString& str, LPCTSTR delimiter )
{
	if( str.GetLength() > 0 )
	{
		int start = 0;
		m_name = str.Tokenize( delimiter, start );
		m_value = str.Tokenize( delimiter, start );
	}
}

BOOL CToken::operator ==( const CToken &r ) const
{
	if( m_name != r.m_name )
		return FALSE;
	//if( m_value != r.m_value )
	//	return FALSE;
	return TRUE;
}

BOOL CToken::operator !=( const CToken &r ) const
{
	return *this == r ? FALSE : TRUE;
}

CString& CToken::GetValue( int count, CToken* tok, LPCTSTR name )
{
	if( !tok )
		return m_null;
	for( int i=0; i<count; i++ )
	{
		if( !tok[i].m_name.CompareNoCase(name) )
			return tok[i].m_value;
	}
	return m_null;
}

BOOL CToken::SetValue( int count, CToken* tok, LPCTSTR name, LPCTSTR value )
{
	if( !tok )
		return FALSE;
	for( int i=0; i<count; i++ )
	{
		if( !tok[i].m_name.CompareNoCase(name) )
		{
			tok[i].m_value = value;
			return TRUE;
		}
	}
	return FALSE;
}

void CToken::Print( int count, CToken* tok )
{
	for( int i=0; i<count; i++ )
		TRACE( _T("%s = %s\n"), tok[i].m_name, tok[i].m_value );
}



CString CTokenList::m_null;
CToken CTokenList::m_null_token;

CTokenList::CTokenList( )
{
}

CTokenList::CTokenList( const CTokenList &r )
{
	m_list.RemoveAll( );
	m_list.AddTail( (CList<CToken, CToken&>*)(&r.m_list) );
	m_map.RemoveAll( );
	const CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *p = r.m_map.PGetFirstAssoc( );
	while( p )
	{
		m_map[p->key] = p->value;
		p = r.m_map.PGetNextAssoc( p );
	}
}

CTokenList::CTokenList( CStrList& list, LPCTSTR delimiter )
{
	for( int i=0; i<list.Count(); i++ )
	{
		CToken tok;
		tok.Extract( list[i], delimiter );
		AddTail( &tok, 1 );
	}
}

CTokenList::CTokenList( CString str, LPCTSTR token_delimeter, LPCTSTR key_delimeter )
{
	CStrList list( str, token_delimeter );

	for( int i=0; i<list.Count(); i++ )
	{
		CToken tok;
		tok.Extract( list[i], key_delimeter );
		AddTail( &tok, 1 );
	}
}

CTokenList::~CTokenList( )
{
}

void CTokenList::Set( CStrList& list, LPCTSTR delimiter )
{
	RemoveAll( );
	for( int i=0; i<list.Count(); i++ )
	{
		CToken tok;
		tok.Extract( list[i], delimiter );
		AddTail( &tok, 1 );
	}
}

BOOL CTokenList::AddHead( CToken &tok )
{
	POSITION pos = m_list.AddHead( tok );
	m_map[tok.m_name] = tok.m_value;
	return TRUE;
}

BOOL CTokenList::AddTail( CToken &tok )
{
	POSITION pos = m_list.AddTail( tok );
	m_map[tok.m_name] = tok.m_value;
	return TRUE;
}

BOOL CTokenList::AddHead( LPCTSTR name, LPCTSTR value )
{
	CToken tok( name, value );
	return AddHead( tok );
}

BOOL CTokenList::AddTail( LPCTSTR name, LPCTSTR value )
{
	CToken tok( name, value );
	return AddTail( tok );
}

BOOL CTokenList::AddHead( CToken* new_tok, int count )
{
	POSITION pos = m_list.AddHead( new_tok[0] );
	m_map[new_tok[0].m_name] = new_tok[0].m_value;
	for( int i=1; i<count; i++ )
	{
		pos = m_list.InsertAfter( pos, new_tok[i] );
		m_map[new_tok[i].m_name] = new_tok[i].m_value;
	}
	return TRUE;
}

BOOL CTokenList::AddTail( CToken* new_tok, int count )
{
	POSITION pos = m_list.AddTail( new_tok[0] );
	m_map[new_tok[0].m_name] = new_tok[0].m_value;
	for( int i=1; i<count; i++ )
	{
		pos = m_list.InsertAfter( pos, new_tok[i] );
		m_map[new_tok[i].m_name] = new_tok[i].m_value;
	}
	return TRUE;
}

BOOL CTokenList::InsertAfter( CToken* new_tok, int count, LPCTSTR name )
{
	POSITION pos = m_list.Find( CToken(name, _T("")) );
	if( !pos )
		return FALSE;
	for( int i=0; i<count; i++ )
	{
		pos = m_list.InsertAfter( pos, new_tok[i] );
		m_map[new_tok[i].m_name] = new_tok[i].m_value;
	}
	return TRUE;
}

BOOL CTokenList::InsertBefore( CToken* new_tok, int count, LPCTSTR name )
{
	POSITION pos = m_list.Find( CToken(name, _T("")) );
	if( !pos )
		return FALSE;
	for( int i=count-1; i>=0; i-- )
	{
		pos = m_list.InsertBefore( pos, new_tok[i] );
		m_map[new_tok[i].m_name] = new_tok[i].m_value;
	}
	return TRUE;
}

BOOL CTokenList::AddHead( CList<CToken, CToken&> *list )
{
	POSITION pos = list->GetHeadPosition( );
	while( pos )
	{
		CToken new_tok = list->GetNext( pos );
		m_list.AddHead( new_tok );
		m_map[new_tok.m_name] = new_tok.m_value;
	}
	return TRUE;
}

BOOL CTokenList::AddTail( CList<CToken, CToken&> *list )
{
	POSITION pos = list->GetTailPosition( );
	while( pos )
	{
		CToken new_tok = list->GetPrev( pos );
		m_list.AddTail( new_tok );
		m_map[new_tok.m_name] = new_tok.m_value;
	}
	return TRUE;
}

BOOL CTokenList::InsertAfter( CList<CToken, CToken&> *list, LPCTSTR name )
{
	POSITION org_pos = m_list.Find( CToken(name, _T("")) );
	if( !org_pos )
		return FALSE;

	POSITION new_pos = list->GetHeadPosition( );
	while( new_pos )
	{
		CToken new_tok = list->GetNext( new_pos );
		org_pos = m_list.InsertAfter( org_pos, new_tok );
		m_map[new_tok.m_name] = new_tok.m_value;
	}
	return TRUE;
}

BOOL CTokenList::InsertBefore( CList<CToken, CToken&> *list, LPCTSTR name )
{
	POSITION org_pos = m_list.Find( CToken(name, _T("")) );
	if( !org_pos )
		return FALSE;

	POSITION new_pos = list->GetTailPosition( );
	while( new_pos )
	{
		CToken new_tok = list->GetPrev( new_pos );
		org_pos = m_list.InsertBefore( org_pos, new_tok );
		m_map[new_tok.m_name] = new_tok.m_value;
	}
	return TRUE;
}

BOOL CTokenList::Remove( LPCTSTR name, BOOL full_match )
{
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		POSITION cur = pos;
		CToken tok = m_list.GetNext( pos );
		if( full_match )
		{
			if( !tok.m_name.CompareNoCase(name) )
			{
				m_map.RemoveKey( tok.m_name );
				m_list.RemoveAt( cur );
			}
		}
		else
		{
			if( tok.m_name.Find(name) >= 0 )
			{
				m_map.RemoveKey( tok.m_name );
				m_list.RemoveAt( cur );
			}
		}
	}
	return TRUE;
}

BOOL CTokenList::RemoveAll( )
{
	m_list.RemoveAll( );
	m_map.RemoveAll( );
	return TRUE;
}

BOOL CTokenList::AddIfNotExist( LPCTSTR name, LPCTSTR value )
{
	CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *p = m_map.PLookup( name );
	if( p )
		return FALSE;
	CString total = _T("");
	CStrList str_list( name, _T("$$") );
	for( int i=0; i<str_list.Count(); i++ )
	{
		CString str = str_list[i];
		str.Remove( _T('$') );
		total += CMyString( _T("$%s$"), str );

		CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *pair = m_map.PLookup( total );
		if( !pair )
		{
			CToken tok( (LPCTSTR)total, total.CompareNoCase(name) ? _T("") : (LPCTSTR)value );
			AddTail( &tok, 1 );
		}
	}
	return TRUE;
}

BOOL CTokenList::InsertAfterIfNotExist( LPCTSTR name, LPCTSTR value, LPCTSTR base )
{
	CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *p = m_map.PLookup( name );
	if( p )
		return FALSE;
	CString total = _T("");
	CStrList str_list( name, _T("$$") );
	for( int i=0; i<str_list.Count(); i++ )
	{
		CString str = str_list[i];
		str.Remove( _T('$') );
		total += CMyString( _T("$%s$"), str );

		CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *pair = m_map.PLookup( total );
		if( !pair )
		{
			CToken tok( (LPCTSTR)total, total.CompareNoCase(name) ? _T("") : (LPCTSTR)value );
			InsertAfter( &tok, 1, base );
		}
	}
	return TRUE;
}

BOOL CTokenList::InsertBeforeIfNotExist( LPCTSTR name, LPCTSTR value, LPCTSTR base )
{
	CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *p = m_map.PLookup( name );
	if( p )
		return FALSE;
	CString total = _T("");
	CStrList str_list( name, _T("$$") );
	for( int i=0; i<str_list.Count(); i++ )
	{
		CString str = str_list[i];
		str.Remove( _T('$') );
		total += CMyString( _T("$%s$"), str );

		CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *pair = m_map.PLookup( total );
		if( !pair )
		{
			CToken tok( (LPCTSTR)total, total.CompareNoCase(name) ? _T("") : (LPCTSTR)value );
			InsertBefore( &tok, 1, base );
		}
	}
	return TRUE;
}

CString& CTokenList::GetValue( LPCTSTR name, BOOL print )
{
	if( print )
	{
		POSITION pos = m_list.GetHeadPosition( );
		while( pos )
		{
			CToken token = m_list.GetNext( pos );
			TRACE( _T("%s=%s\n"), token.m_name, token.m_value );
		}
	}
	CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *p = m_map.PLookup( name );
	if( !p )
		return m_null;
	return p->value;
}

CString& CTokenList::GetValue( LPCTSTR parent, LPCTSTR name, BOOL print )
{
	return GetValue(GetName(parent, name), print);
}

BOOL CTokenList::SetValue( LPCTSTR name, LPCTSTR val )
{
	POSITION pos = m_list.Find( CToken(name, _T("")) );
	if( !pos )
		return FALSE;

	CToken token( name, val );
	m_list.SetAt( pos, token );
	m_map.SetAt( name, val );
	return TRUE;
}

BOOL CTokenList::SetValueOrAdd( LPCTSTR name, LPCTSTR val )
{
	if (!SetValue(name, val))
	{
		CStrList lstNames(name, _T("$"));
		CString strName;
		for(int nLoopCnt = 0; nLoopCnt < lstNames.Count() - 1; nLoopCnt++)
		{
			strName += GetName(lstNames[nLoopCnt]);
			if (!HaveValue(strName))
				AddTail(strName, _T(""));
		}
		return AddTail(name, val);
	}
	return TRUE;
}

BOOL CTokenList::SetValueOrAdd( LPCTSTR parent, LPCTSTR name, LPCTSTR val )
{
	return SetValueOrAdd(GetName(parent, name), val);
}

BOOL CTokenList::SetValue( LPCTSTR parent, LPCTSTR name, LPCTSTR val )
{
	return SetValue(GetName(parent, name), val);
}

BOOL CTokenList::HaveValue( LPCTSTR name )
{
	CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *p = m_map.PLookup(name);
	return p != NULL;
}

CString CTokenList::GetValue( LPCTSTR name, CList<CToken, CToken&> *list )
{
	POSITION pos = list->Find( CToken(name, _T("")) );
	if( !pos )
		return m_null;
	CToken token = list->GetAt( pos );
	return token.m_value;
}

BOOL CTokenList::SetValue( LPCTSTR name, LPCTSTR val, CList<CToken, CToken&> *list )
{
	POSITION pos = list->Find( CToken(name, _T("")) );
	if( !pos )
		return FALSE;
	CToken token( name, val );
	list->SetAt( pos, token );
	return TRUE;
}

CString CTokenList::GetValue( CTokenList &list1, CTokenList &list2, LPCTSTR name )
{
	CString val = list1.GetValue( name );
	if( val.GetLength() > 0 )
		return val;
	return list2.GetValue( name );
}

int CTokenList::GetTagIndex( LPCTSTR name, int index )
{
	if( index < 0 )
		return -1;

	int cur_index = 0;
	int tag_index = -1;
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		CToken tok = m_list.GetNext( pos );
		if( tok.m_name.Find(name) >= 0 )
		{
			//TRACE( "%s=%s\n", tok.m_name, tok.m_value );
			LPCTSTR p = tok.m_name;
			p += _tcslen( name );
			if (*p != _T('%') )
				continue;

			LPCTSTR dollar = _tcsstr( p, _T("$$") );
			if( dollar )
				continue;

			dollar = _tcsstr( p, _T("$") );
			if( !dollar )
				continue;

			TCHAR buf[1024];
			LPTSTR dst = buf;
			LPTSTR src = (LPTSTR)( p + 1 );
			while( src < dollar )
				*dst++ = *src++;
			*dst = 0;

			tag_index = _ttoi( buf );
			cur_index++;
			if( cur_index > index )
				return tag_index;
		}
	}
	return -1;
}

int CTokenList::GetMaxTagIndex( LPCTSTR name )
{
	int max_index = 0;
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		CToken tok = m_list.GetNext( pos );
		if( tok.m_name.Find(name) >= 0 )
		{
			//TRACE( "%s=%s\n", tok.m_name, tok.m_value );
			LPCTSTR p = tok.m_name;
			p += _tcslen( name );
			if( *p != _T('%') )
				continue;

			LPCTSTR dollar = _tcsstr( p, _T("$$") );
			if( dollar )
				continue;

			dollar = _tcsstr( p, _T("$") );
			if( !dollar )
				continue;

			TCHAR buf[1024];
			LPTSTR dst = buf;
			LPTSTR src = (LPTSTR)( p + 1 );
			while( src < dollar )
				*dst++ = *src++;
			*dst = 0;

			int tag_index = _ttoi( buf );
			if( tag_index > max_index )
				max_index = tag_index;
		}
	}
	return max_index;
}

CString CTokenList::ToString( )
{
	CString str = _T("");
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		CToken token = m_list.GetNext( pos );
		if( str.GetLength() > 0 )
			str += "&";
		str += CMyString( _T("%s=%s"), token.m_name, token.m_value );
	}
	return str;
}

CString CTokenList::ToString( char deli1, char deli2 )
{
	CString str = _T("");
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		CToken token = m_list.GetNext( pos );
		if( str.GetLength() > 0 )
			str += CMyString( _T("%c%s%c%s"), deli1, token.m_name, deli2, token.m_value );
		else
			str += CMyString( _T("%s%c%s"), token.m_name, deli2, token.m_value );
	}
	return str;
}

CString CTokenList::ToString( LPCTSTR except )
{
	CString str = _T("");
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		CToken token = m_list.GetNext( pos );
		if( !_tcsicmp(except, token.m_name) )
			continue;
		if( str.GetLength() > 0 )
			str += _T("&");
		str += CMyString( _T("%s=%s"), token.m_name, token.m_value );
	}
	return str;
}

CString CTokenList::ToString( CStringList& except )
{
	CString str = _T("");
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		CToken token = m_list.GetNext( pos );
		if( except.Find(token.m_name) )
			continue;
		if( str.GetLength() > 0 )
			str += _T("&");
		str += CMyString( _T("%s=%s"), token.m_name, token.m_value );
	}
	return str;
}

void CTokenList::Print( )
{
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		CToken token = m_list.GetNext( pos );
		CString name = token.m_name;
		CString value = token.m_value;
		name.Replace( _T("%"), _T("%%") );
		value.Replace( _T("%"), _T("%%") );
		TRACE( _T("%s : %s\n"), name, value );
	}
}

void CTokenList::Log( LPCTSTR path )
{
	CFile file;
	if( file.Open(path, CFile::modeCreate|CFile::modeReadWrite) )
	{
		POSITION pos = m_list.GetHeadPosition( );
		while( pos )
		{
			CToken token = m_list.GetNext( pos );
			CString name = token.m_name;
			CString value = token.m_value;
			name.Replace( _T("%"), _T("%%") );
			value.Replace( _T("%"), _T("%%") );
			CMyString str( _T("%s : %s\n"), name, value );
			file.Write( (LPCTSTR)str, str.GetLength() );
		}
	}
	file.Close( );
}

CTokenList &CTokenList::operator=( const CTokenList &r )
{
	m_list.RemoveAll( );
	m_map.RemoveAll( );
	m_list.AddTail( (CList<CToken, CToken&>*)(&r.m_list) );
	const CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *p = r.m_map.PGetFirstAssoc( );
	while( p )
	{
		m_map[p->key] = p->value;
		p = r.m_map.PGetNextAssoc( p );
	}
	return *this;
}

CTokenList &CTokenList::operator+=( const CTokenList &r )
{
	m_list.AddTail( (CList<CToken, CToken&>*)(&r.m_list) );
	const CMap<CString,LPCTSTR,CString,LPCTSTR>::CPair *p = r.m_map.PGetFirstAssoc( );
	while( p )
	{
		m_map[p->key] = p->value;
		p = r.m_map.PGetNextAssoc( p );
	}
	return *this;
}

BOOL CTokenList::operator ==( CTokenList &r ) const
{
	if( m_list.GetCount() != r.m_list.GetCount() )
		return FALSE;
	POSITION pos = m_list.GetHeadPosition( );
	while( pos )
	{
		CToken tok = m_list.GetNext( pos );
		CString val = r.GetValue( tok.m_name );
		if( val.CompareNoCase(tok.m_value) )
			return FALSE;
	}

	// TODO: map
	return TRUE;
}

BOOL CTokenList::operator !=( CTokenList &r ) const
{
	return *this == r ? FALSE : TRUE;
}

CToken &CTokenList::operator[]( int index )
{
	POSITION pos = m_list.FindIndex( index );
	if( !pos )
		return m_null_token;
	return m_list.GetAt( pos );
}
