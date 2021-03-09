#include "stdafx.h"
#include "XmlSerializer.h"
#include "StringUtil.h"

#ifdef _USE_XMLLITE
#pragma comment(lib, "xmllite.lib")

#define SAFE_RELEASE(I)         do { if (I){ I->Release(); } I = NULL; } while(0) 
#endif

CXmlSerializer::CXmlSerializer()
{

}

CXmlSerializer::~CXmlSerializer()
{

}

CString CXmlSerializer::ToXml(CSerializable* pObject)
{
	return ToXml(pObject, ENCODING_ANSI);
}

CString CXmlSerializer::ToXml(CSerializable* pObject, ENCODING eEncoding)
{
	// TODO : 선택된 인코딩 방법에 따라 실제 인코딩 되도록 한다.
	CSerializationInfo info;
	pObject->GetObjectData(info);

	CString strTemp;
	CString strXml = _T("<?xml version=\"1.0\"");
	if (eEncoding)
	{
		strTemp.Format(_T(" encoding=\"%s\""), GetEncodingString(eEncoding));
		strXml += strTemp;
	}
	strXml += _T("?>\r\n");
	RecursiveMakeXml(&info, strXml);

	switch (eEncoding)
	{
	case ENCODING_UTF8:
		strXml = str_util::StringToUTF8(strXml);
		break;
	case ENCODING_UTF16:
		// TODO
		break;
	}

	return strXml;
}


CString CXmlSerializer::ToXmlFile(CString strFile, CSerializable* pObject, ENCODING eEncoding/* = ENCODING_ANSI*/)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	CString strResultXml;

	try
	{
		hFile = CreateFile(strFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TRACE(_T("[CXmlSerializer::ToXmlFile] ERROR - Can't create file.(File: %s, Error: %u)\n"), strFile, GetLastError());
			return CString(_T(""));
		}

		DWORD dwTotalWritten = 0;
		DWORD dwWritten = 0;
		strResultXml = ToXml(pObject, eEncoding);

		// TODO : Encoding 에 따라 인코딩 한다.

		/*USES_CONVERSION;
		LPCTSTR pBuf = T2A(strResultXml);
		LPCTSTR pTempBuf = pBuf;*/

		USES_CONVERSION;
		CStringA strMultiByteString = CT2A((LPCTSTR)strResultXml);
		LPCSTR pTempBuf = (LPCSTR)strMultiByteString;

		while(WriteFile(hFile, pTempBuf, strMultiByteString.GetLength(), &dwWritten, NULL))
		{
			if (dwWritten == 0)
				break;

			dwTotalWritten += dwWritten;
			pTempBuf += dwWritten;
			if((int)dwTotalWritten >= strMultiByteString.GetLength())
				break;
		}

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		if((int)dwTotalWritten < strMultiByteString.GetLength())
			return CString("");
	}
	catch (...)
	{
		TRACE(NULL, 1, _T("[CXmlSerializer::ToXmlFile] ERROR - Exception(%s)\n"), strFile);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
		strResultXml = _T("");
	}

	if(strResultXml.IsEmpty())
		return _T("");

	return strResultXml;
}

void CXmlSerializer::RecursiveMakeXml(CSerializationInfo* pInfo, CString& strXml)
{
	CString strDepth;
	CString strSTag, strETag;

	for(int nLoopCnt = 0; nLoopCnt < pInfo->GetDepth(); nLoopCnt++)
		strDepth += _T("\t");

	if (pInfo->GetChildInfo() || pInfo->HaveValues())
	{
		strSTag.Format(_T("%s<%s>\r\n"), strDepth, pInfo->GetObjectName());
		strETag.Format(_T("%s</%s>\r\n"), strDepth, pInfo->GetObjectName());
	}

	strXml += strSTag;


	std::vector<std::pair<CString, CString>> vKeyValues;
	pInfo->GetKeyValues(&vKeyValues);

	CString strTemp;
	std::vector<std::pair<CString, CString>>::iterator pos = vKeyValues.begin();
	while(pos != vKeyValues.end())
	{
		strTemp.Format(_T("\t%s<%s>%s</%s>\r\n"), 
			strDepth,
			pos->first, 
			str_util::XmlEscapeCharsTo(pos->second), 
			pos->first);
		strXml += strTemp;
		pos++;
	}

	if (pInfo->GetChildInfo())
	{
		RecursiveMakeXml(pInfo->GetChildInfo(), strXml);
	}

	strXml += strETag;

	if (pInfo->GetNextInfo())
	{
		RecursiveMakeXml(pInfo->GetNextInfo(), strXml);
	}
}


BOOL CXmlSerializer::FromXml(CString& strXml, CSerializable* pObject)
{
	USES_CONVERSION;
	return FromXml(CT2A((LPCTSTR)strXml), pObject);
}


BOOL CXmlSerializer::FromXml(LPCSTR lpstXml, CSerializable* pObject)
{
	CStringA strXmlA = lpstXml;
	ezxml_t hXml = ezxml_parse_str(strXmlA.GetBuffer(), strXmlA.GetLength());
	BOOL bResult = FALSE;

	do 
	{
		if (!hXml || (hXml->name == NULL))
			break;

		if (pObject)
		{
			CSerializationInfo info;
			RecursiveMakeInfo(hXml, &info);
			pObject->SetObjectData(info);
		}
		bResult = TRUE;
	} while (FALSE);

	ezxml_free(hXml);

	return bResult;
}

#ifdef _USE_XMLLITE
BOOL CXmlSerializer::FromXml2(LPCSTR lpstXml, CSerializable* pObject)
{
	IStream *pStream = NULL;
	IXmlReader *pReader = NULL;
	HRESULT hr = S_OK;
	DWORD dwWritten = 0;
	BOOL bResult = FALSE;

	CSerializationInfo info;
	USES_CONVERSION;

	do
	{
		if (FAILED(CreateStreamOnHGlobal(NULL, TRUE, &pStream)))
			break;

		if (FAILED(pStream->Write(CA2T(lpstXml), lstrlenA(lpstXml) * sizeof(TCHAR), &dwWritten)))
			break;

		if (FAILED(CreateXmlReader(_uuidof(IXmlReader), (void**)&pReader, NULL)))
			break;

		//if (FAILED(pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)))
		//	break;

		LARGE_INTEGER position;
		position.QuadPart = 0;

		if (FAILED(pStream->Seek(position, STREAM_SEEK_SET, NULL)))
			break;

		if (FAILED(pReader->SetInput(pStream)))
			break;

		if (pObject)
		{
			RecursiveMakeInfo(pReader, &info);
			pObject->SetObjectData(info);
		}
		bResult = TRUE;
	}
	while(0);

	SAFE_RELEASE(pReader);
	SAFE_RELEASE(pStream);

	return bResult;
}
#endif

BOOL CXmlSerializer::FromXmlFile(CString strFile, CSerializable* pObject)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BOOL bResult = TRUE;
	PCHAR pBuf = NULL;

	try
	{
		hFile = CreateFile(strFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Can't open file(Path: %s, Error: %u)\n"), strFile, GetLastError());
			return FALSE;
		}


		DWORD dwLength = SetFilePointer(hFile, 0, NULL, FILE_END);
		if (dwLength == INVALID_SET_FILE_POINTER)
		{
			TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Can't set file pointer(Path: %s, Error: %u)\n"), strFile, GetLastError());
			CloseHandle(hFile);
			return FALSE;
		}

		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Can't set file pointer(Path: %s, Error: %u)\n"), strFile, GetLastError());
			CloseHandle(hFile);
			return FALSE;
		}

		if (dwLength < 0)
		{
			TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Can't have data in file(Path: %s)\n"), strFile);
			CloseHandle(hFile);
			return FALSE;
		}

		pBuf = new CHAR[dwLength + 1];
		PCHAR pBufPt = pBuf;
		DWORD dwTotalRead = 0;
		DWORD dwRead = 0;

		pBuf[dwLength] = '\0';
		while(ReadFile(hFile, pBufPt, (dwLength - dwTotalRead), &dwRead, NULL))
		{
			if (dwRead == 0)
				break;

			pBufPt = pBufPt + dwRead;
			dwTotalRead += dwRead;
			if (dwTotalRead >= dwLength)
				break;
		}

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		if (dwTotalRead < dwLength)
		{
			delete[] pBuf;
			pBuf = NULL;
			return FALSE;
		}

#ifdef _USE_XMLLITE
		bResult = FromXml2(pBuf, pObject);
		delete[] pBuf;
		pBuf = NULL;
#else
		bResult = FromXml(pBuf, pObject);
		delete[] pBuf;
		pBuf = NULL;
#endif
	}
	catch(...)
	{
		TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Exception(%s)\n"), strFile);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}

		if (!pBuf)
		{
			delete[] pBuf;
			pBuf = NULL;
		}

		bResult = FALSE;
	}

	return bResult;
}

// NOTE : XML 헤더가 붙은 파일의 경우 Xml FromXml2() 를 사용하면 파싱이 되지 않아 FromXml() 을 사용하도록 함수를 만듦
BOOL CXmlSerializer::FromXmlFile2(CString strFile, CSerializable* pObject)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BOOL bResult = TRUE;
	PCHAR pBuf = NULL;

	try
	{
		hFile = CreateFile(strFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Can't open file(Path: %s, Error: %u)\n"), strFile, GetLastError());
			return FALSE;
		}


		DWORD dwLength = SetFilePointer(hFile, 0, NULL, FILE_END);
		if (dwLength == INVALID_SET_FILE_POINTER)
		{
			TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Can't set file pointer(Path: %s, Error: %u)\n"), strFile, GetLastError());
			CloseHandle(hFile);
			return FALSE;
		}

		if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{
			TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Can't set file pointer(Path: %s, Error: %u)\n"), strFile, GetLastError());
			CloseHandle(hFile);
			return FALSE;
		}

		if (dwLength < 0)
		{
			TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Can't have data in file(Path: %s)\n"), strFile);
			CloseHandle(hFile);
			return FALSE;
		}

		pBuf = new CHAR[dwLength + 1];
		PCHAR pBufPt = pBuf;
		DWORD dwTotalRead = 0;
		DWORD dwRead = 0;

		pBuf[dwLength] = '\0';
		while(ReadFile(hFile, pBufPt, (dwLength - dwTotalRead), &dwRead, NULL))
		{
			if (dwRead == 0)
				break;

			pBufPt = pBufPt + dwRead;
			dwTotalRead += dwRead;
			if (dwTotalRead >= dwLength)
				break;
		}

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		if (dwTotalRead < dwLength)
		{
			delete[] pBuf;
			pBuf = NULL;
			return FALSE;
		}

		bResult = FromXml(pBuf, pObject);
		delete[] pBuf;
		pBuf = NULL;
	}
	catch(...)
	{
		TRACE(_T("[CXmlSerializer::FromXmlFile] ERROR - Exception(%s)\n"), strFile);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}

		if (!pBuf)
		{
			delete[] pBuf;
			pBuf = NULL;
		}

		bResult = FALSE;
	}

	return bResult;
}


void CXmlSerializer::RecursiveMakeInfo(ezxml_t xml, CSerializationInfo* pInfo)
{
	USES_CONVERSION;

	if(!xml->name)
		return;

	CString strKey = A2T(xml->name);
	CString strValue;
	pInfo->SetObjectName(strKey.MakeLower());

	for( xml = xml->child; xml; xml = xml->ordered)
	{
		if (xml->child)
			RecursiveMakeInfo(xml, !pInfo->GetChildInfo() ? pInfo->CreateChildInfo() : pInfo->GetChildInfo()->CreateNextInfo());
		else
		{
			strKey = A2T(xml->name);
			strValue = str_util::XmlEscapeCharsFrom(A2T(xml->txt));
			pInfo->SetValue(strKey.MakeLower(), strValue);
		}
	}
}


LPCTSTR CXmlSerializer::GetEncodingString(ENCODING eEncoding)
{
	switch(eEncoding)
	{
	case ENCODING_ANSI:
		return _T("Windows-1252");
	case ENCODING_ASCII:
		return _T("US-ASCII");
	case ENCODING_UTF8:
		return _T("UTF-8");
	case ENCODING_UTF16:
		return _T("UTF-16");
	default:
		return _T("Windows-1252");
	}
}

#ifdef _USE_XMLLITE
void CXmlSerializer::RecursiveMakeInfo(IXmlReader* pReader, CSerializationInfo* pInfo)
{
	USES_CONVERSION;
	XmlNodeType nodeType;
	const WCHAR* pwszElementName = NULL;
	const WCHAR* pwszValue = NULL;
	CSerializationInfo* pCurInfo = pInfo;

	while(S_OK == pReader->Read(&nodeType))
	{
		switch(nodeType)
		{
		case XmlNodeType_Element:
			if (pReader->IsEmptyElement())
				break;

			if (FAILED(pReader->GetLocalName(&pwszElementName, NULL)))
			{
				pwszElementName = NULL;
				break;
			}

			if (pCurInfo->GetObjectName().GetLength() <= 0)
			{
				pCurInfo->SetObjectName(CW2T(pwszElementName));
			}
			else if (pCurInfo->GetChildInfo() == NULL)
			{
				pCurInfo = pCurInfo->CreateChildInfo();
				pCurInfo->SetObjectName(CW2T(pwszElementName));
			}
			else
			{
				pCurInfo = pCurInfo->GetChildInfo()->CreateNextInfo();
				pCurInfo->SetObjectName(CW2T(pwszElementName));
			}
			break;
		case XmlNodeType_EndElement:
			{
				if (pCurInfo->GetParentInfo())
				{
					CSerializationInfo* pPrevInfo = pCurInfo;
					pCurInfo = pCurInfo->GetParentInfo();

					if (!pPrevInfo->GetChildInfo() && !pPrevInfo->HaveValues())
						pCurInfo->DeleteChildInfo();
				}
			}
			break;
		case XmlNodeType_Text:
			if (FAILED(pReader->GetValue(&pwszValue, NULL)))
				break;

			if (pCurInfo->GetParentInfo())
				pCurInfo->GetParentInfo()->SetValue(CString(CW2T(pwszElementName)).MakeLower(), str_util::XmlEscapeCharsFrom(CString(CW2T(pwszValue))));
			break;
		}
	}
}
#endif