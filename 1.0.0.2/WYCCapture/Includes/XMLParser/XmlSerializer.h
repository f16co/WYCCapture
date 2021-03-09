#pragma once
#include "Serializable.h"
#include "ezxml.h"

#ifndef _USE_XMLLITE
#define _USE_XMLLITE
#endif

#ifdef _USE_XMLLITE
#include <xmllite.h>
#endif

class CXmlSerializer
{
public:
	enum ENCODING
	{
		ENCODING_ANSI = 0,
		ENCODING_ASCII = 1,
		ENCODING_UTF8 = 2,
		ENCODING_UTF16 = 3
	};

protected:

public:
	CXmlSerializer();
	virtual ~CXmlSerializer();

	static CString ToXml(CSerializable* pObject);
	static CString ToXml(CSerializable* pObject, ENCODING eEncoding);
	static CString ToXmlFile(CString strFile, CSerializable* pObject, ENCODING eEncoding = ENCODING_ANSI);
	static BOOL FromXml(CString& strXml, CSerializable* pObject);
	static BOOL FromXml(LPCSTR lpstrXml, CSerializable* pObject);
#ifdef _USE_XMLLITE
	static BOOL FromXml2(LPCSTR lpstrXml, CSerializable* pObject);
#endif
	static BOOL FromXmlFile(CString strFile, CSerializable* pObject);
	static BOOL FromXmlFile2(CString strFile, CSerializable* pObject);

protected:
	static void RecursiveMakeXml(CSerializationInfo* pInfo, CString& strXml);
	static void RecursiveMakeInfo(ezxml_t xml, CSerializationInfo* pInfo);
	static LPCTSTR GetEncodingString(ENCODING eEncoding);
#ifdef _USE_XMLLITE
	static void RecursiveMakeInfo(IXmlReader* pReader, CSerializationInfo* pInfo);
#endif
};