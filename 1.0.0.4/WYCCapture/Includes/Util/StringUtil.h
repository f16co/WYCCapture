#pragma once

#include "afxtempl.h"

class CMyStringList : public CStringList
{
public:
	CString& operator[](int index);

protected:
	static CString m_null;
};

class CStrList
{
public:
	CStrList();
	CStrList(LPCTSTR str, LPCTSTR delimiter);
	virtual ~CStrList();

	int		Extract(LPCTSTR str, LPCTSTR delimiter);
	int		Count();
	int		Find(LPCTSTR str);
	void	RemoveAll();
	void	RemoveAt(int index);
	void	PushBack(LPCTSTR str);
	void	Print();
	CString& operator[](int index);
	CStrList& operator=(const CStrList& r);
	BOOL	operator==(const CStrList& r);
	BOOL	operator!=(const CStrList& r);
	CString	ToString(LPCTSTR delimiter);

protected:
	CArray<CString,LPCTSTR> m_array;
	static CString m_null;
};

class CStrListEx : public CStrList
{
public:
	CStrListEx();
	CStrListEx(LPCTSTR str, LPCTSTR delimiter);
	virtual ~CStrListEx();

	int Extract(LPCTSTR str, LPCTSTR delimiter);
};

class CToken
{
public:
	CToken();
	CToken(LPCTSTR name, LPCTSTR value);
	virtual ~CToken();

	void	Extract(const CString& str, LPCTSTR delimiter);

	BOOL	operator==(const CToken &r) const;
	BOOL	operator!=(const CToken &r) const;

	static CString& GetValue(int count, CToken* tok, LPCTSTR name);
	static BOOL SetValue(int count, CToken* tok, LPCTSTR name, LPCTSTR value);
	static void Print(int count, CToken* tok);

public:
	CString m_name;
	CString m_value;
	static CString m_null;
};

class CTokenList
{
public:
	CTokenList();
	CTokenList(const CTokenList &r);
	CTokenList(CStrList& list, LPCTSTR delimiter);
	CTokenList(CString str, LPCTSTR token_delimeter, LPCTSTR key_delimeter);
	virtual ~CTokenList();

	void	Set(CStrList& list, LPCTSTR delimiter);

	BOOL	AddHead(CToken &tok);
	BOOL	AddTail(CToken &tok);

	BOOL	AddHead(LPCTSTR name, LPCTSTR value);
	BOOL	AddTail(LPCTSTR name, LPCTSTR value);
	BOOL	AddTail(LPCTSTR parent, LPCTSTR name, LPCTSTR value) { return AddTail(CTokenList::GetName(parent, name), value); }

	BOOL	AddHead(CToken* new_tok, int count);
	BOOL	AddTail(CToken* new_tok, int count);
	BOOL	InsertAfter(CToken* new_tok, int count, LPCTSTR name);
	BOOL	InsertBefore(CToken* new_tok, int count, LPCTSTR name);

	BOOL	AddHead(CList<CToken, CToken&>* list);
	BOOL	AddTail(CList<CToken, CToken&>* list);
	BOOL	InsertAfter(CList<CToken, CToken&>* list, LPCTSTR name);
	BOOL	InsertBefore(CList<CToken, CToken&>* list, LPCTSTR name);
	BOOL	Remove(LPCTSTR name, BOOL full_match);
	BOOL	RemoveAll();

	BOOL	AddIfNotExist(LPCTSTR name, LPCTSTR value);
	BOOL	InsertAfterIfNotExist(LPCTSTR name, LPCTSTR value, LPCTSTR base);
	BOOL	InsertBeforeIfNotExist(LPCTSTR name, LPCTSTR value, LPCTSTR base);

	CString& GetValue(LPCTSTR name, BOOL print = FALSE);
	CString& GetValue(LPCTSTR parent, LPCTSTR name, BOOL print = FALSE);
	BOOL	SetValue(LPCTSTR name, LPCTSTR val);
	BOOL	SetValue(LPCTSTR parent, LPCTSTR name, LPCTSTR val);
	BOOL	SetValueOrAdd(LPCTSTR name, LPCTSTR val);
	BOOL	SetValueOrAdd(LPCTSTR parent, LPCTSTR name, LPCTSTR val);
	BOOL	HaveValue(LPCTSTR name);

	int		GetTagIndex(LPCTSTR name, int index /* zero based */);
	int		GetMaxTagIndex(LPCTSTR name);

	CString	ToString();
	CString	ToString(char deli1, char deli2);
	CString	ToString(LPCTSTR except);
	CString	ToString(CStringList& except);

	void	Print();
	void	Log(LPCTSTR path);

	CTokenList &operator=(const CTokenList &r);
	CTokenList &operator+=(const CTokenList &r);
	BOOL	operator==(CTokenList &r) const;
	BOOL	operator!=(CTokenList &r) const;
	CToken	&operator[](int index);

	CList<CToken, CToken&>m_list;
	CMap<CString, LPCTSTR, CString, LPCTSTR> m_map;

	static CString GetValue(LPCTSTR name, CList<CToken, CToken&> *list);
	static BOOL SetValue(LPCTSTR name, LPCTSTR val, CList<CToken, CToken&> *list);

	static CString GetValue(CTokenList &list1, CTokenList &list2, LPCTSTR name);

	static CString GetName(LPCTSTR name, int number)
	{
		if (number <= 0)
			return GetName(name);

		CString strTemp;
		strTemp.Format(_T("$%s%%%d$"), name, number);
		return strTemp;
	}

	static CString GetName(LPCTSTR name)
	{
		CString strTemp;
		strTemp.Format(_T("$%s$"), name);
		return strTemp;
	}

	static CString GetName(LPCTSTR parent, LPCTSTR name)
	{
		CString strTemp;
		if ((lstrlen(name) >= 0) && (name[0] != '$'))
			strTemp.Format(_T("%s$%s$"), parent, name);
		else
			strTemp.Format(_T("%s%s"), parent, name);
		return strTemp;
	}

protected:
	static CString m_null;
	static CToken m_null_token;
};

namespace str_util
{
	HMODULE	LoadLibrary(LPCTSTR dll_dir, LPCTSTR dll_name);
	void	FreeLibrary(HMODULE lib);
	CString	ModuleDir(HMODULE lib);
	BOOL	ModuleDir(HMODULE lib, CString& dir);
	CString	ModuleName(HMODULE lib);
	BOOL	ModuleName(HMODULE lib, CString& name);
	BOOL	ExtractFileDir(LPCTSTR full_path, CString& dir);
	BOOL	ExtractFileName(LPCTSTR full_path, CString& name);
	BOOL	ExtractFileExt(LPCTSTR full_path, CString& ext);
	BOOL	ExtractRelativeFileName(CString full_path, CString base_path, CString& file_name);
	BOOL	ExtractParentDir(CString full_path, CString& parent_dir);
	CString PathCombine(CString strA, CString strB);

	void	MyTrace(LPCTSTR file, int line, LPCTSTR format, ...);
	void	ExtractDecimal(int decimal, int extract[], int count);
	int		UrlEncode(LPTSTR source, LPTSTR dest);
	int		UrlDecode(LPTSTR source, LPTSTR dest);

	void	a2w(LPSTR str, CStringW& res);
	void	w2a(LPWSTR str, CStringA& res);
	CStringW a2w(LPSTR str);
	CStringA w2a(LPWSTR str);
	int		atoi(LPCTSTR str);
	//LPCTSTR	skip_space				( LPCTSTR str );
	BOOL	is_inlist(TCHAR ch, LPCTSTR list);
	LPCTSTR	search_ch(LPCTSTR str, LPCTSTR list);
	LPCTSTR	search_ch_r(LPCTSTR str, LPCTSTR list);
	LPCTSTR	search_non_ch(LPCTSTR str, LPCTSTR list);
	LPCTSTR	search_non_ch_r(LPCTSTR str, LPCTSTR list);
	void	CopyStringList(CStringList& dst, CStringList& src);

	BOOL	ParseSetup(CString& str, CStrList& list, CToken*& tok);
	CString	&GetValue(CToken* tok, int tok_count, LPCTSTR name);
	CToken	*GetToken(CToken* tok, int tok_count, LPCTSTR name);
	BOOL	SetValue(CToken* tok, int tok_count, LPCTSTR name, LPCTSTR val);
	BOOL	SetupToString(CString& str, CToken* tok, int tok_count, int setup_code);
	void	PrintStrList(CStrList& list);
	BOOL	IsDigit(CString& str);

	struct RS_TAB
	{
		CString m_key;
		CString m_val;
	};
	int		ReplaceStr(CString& str, RS_TAB* tab, int entry_count);
	int		ReplaceStr1(CString& str);	// "=&<>'"" => ;...;
	int		ReplaceStr1R(CString& str);	// ;...; => "=&<>'""
	int		ReplaceStr1X(CString& str);	// "&<>'"" => &amp; &lt; &gt; &apos; &quot;

	CStringA UrlEncode(CStringA strUrl);
	CStringA UrlDecode(CStringA strUrl);
	CStringA UrlDecode(LPCTSTR LPTSTRUrl);
	CString XmlEscapeCharsTo(CString strDecoded);
	CString XmlEscapeCharsFrom(CString strEncoded);

	CString UTF8ToString(CStringA str);
	CStringA StringToUTF8(CString strText);

};

class CMyString : public CString
{
public:
	CMyString(int count, LPCTSTR str);
	CMyString(LPCTSTR format, ...);

	CMyString(CWnd& wnd);
	CMyString(CWnd* wnd);
	CMyString(CComboBox& combo);

	CMyString& operator=(CString& r)
	{
		CString::operator=(r);
		return *this;
	}
};
