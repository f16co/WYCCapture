#include "stdafx.h"
#include "SerializationInfo.h"
#include <algorithm>


CSerializationInfo::CSerializationInfo()
{
	m_pPrevInfo	= NULL;
	m_pNextInfo = NULL;
	m_ppLastNextInfo = NULL;
	m_pChildInfo = NULL;
	m_pParentInfo = NULL;
	m_nDepth = 0;
	m_bIsList = FALSE;
}

CSerializationInfo::~CSerializationInfo()
{
	if (m_pChildInfo)
		delete m_pChildInfo;

	if (m_pNextInfo)
		delete m_pNextInfo;
}

void CSerializationInfo::SetValueCore(const LPCTSTR lpstrKey, const LPCTSTR lpstrValue)
{
	m_mapKeys[lpstrKey].push_back((int)m_vValues.size());
	m_vValues.push_back(lpstrValue);
}


int CSerializationInfo::GetInt(const LPCTSTR lpstrKey, int nDefValue/* = 0*/, int nKeyIndex/* = 0*/)
{
	int nIndex = GetKeyIndex(lpstrKey, nKeyIndex);
	if (nIndex < 0)
		return nDefValue;

	return _ttoi(m_vValues[nIndex]);
}


void CSerializationInfo::SetInt(const LPCTSTR lpstrKey, int nValue)
{
	CString strValue;
	strValue.Format(_T("%d"), nValue);
	SetValueCore(lpstrKey, strValue);
}

DWORD CSerializationInfo::GetDword(const LPCTSTR lpstrKey, DWORD dwDefValue/* = 0*/, int nKeyIndex/* = 0*/)
{
	int nIndex = GetKeyIndex(lpstrKey, nKeyIndex);
	if (nIndex < 0)
		return dwDefValue;

	return (DWORD)_ttoi(m_vValues[nIndex]);
}

void CSerializationInfo::SetDword(const LPCTSTR lpstrKey, DWORD dwValue)
{
	CString strValue;
	strValue.Format(_T("%u"), dwValue);
	SetValueCore(lpstrKey, strValue);
}

INT64 CSerializationInfo::GetInt64(const LPCTSTR lpstrKey, INT64 i64DefValue/* = 0*/, int nKeyIndex/* = 0*/)
{
	int nIndex = GetKeyIndex(lpstrKey, nKeyIndex);
	if (nIndex < 0)
		return i64DefValue;

	return _ttoi64(m_vValues[nIndex]);
}

void CSerializationInfo::SetInt64(const LPCTSTR lpstrKey, INT64 i64Value)
{
	CString strValue;
	strValue.Format(_T("%I64d"), i64Value);
	SetValueCore(lpstrKey, strValue);
}

DOUBLE CSerializationInfo::GetDouble(const LPCTSTR lpstrKey, DOUBLE dDefValue/* = 0*/, int nKeyIndex/* = 0*/)
{
	int nindex = GetKeyIndex(lpstrKey, nKeyIndex);
	if (nindex < 0)
		return dDefValue;

	return _tstof(m_vValues[nindex]);
}


void CSerializationInfo::SetDouble(const LPCTSTR lpstrKey, DOUBLE dValue)
{
	CString strValue;
	strValue.Format(_T("%f"), dValue);
	SetValueCore(lpstrKey, strValue);
}


float CSerializationInfo::GetFloat(const LPCTSTR lpstrKey, float fDefValue/* = 0*/, int nKeyIndex/* = 0*/)
{
	int nIndex = GetKeyIndex(lpstrKey, nKeyIndex);
	if (nIndex < 0)
		return fDefValue;

	return (float)_tstof(m_vValues[nIndex]);
}

void CSerializationInfo::SetFloat(const LPCTSTR lpstrKey, float fValue)
{
	CString strValue;
	strValue.Format(_T("%f"), fValue);
	SetValueCore(lpstrKey, strValue);
}


BOOL CSerializationInfo::GetBool(const LPCTSTR lpstrKey, BOOL bDefValue/* = FALSE*/, int nKeyIndex/* = 0*/)
{
	int nIndex = GetKeyIndex(lpstrKey, nKeyIndex);
	if (nIndex < 0)
		return bDefValue;

	return (m_vValues[nIndex] == _T("1"));
}

void CSerializationInfo::SetBool(const LPCTSTR lpstrKey, BOOL bValue)
{
	SetValueCore(lpstrKey, bValue ? _T("1") : _T("0"));
}


BYTE CSerializationInfo::GetByte(const LPCTSTR lpstrKey, BYTE bDefValue/* = 0*/, int nKeyIndex/* = 0*/)
{
	int nIndex = GetKeyIndex(lpstrKey, nKeyIndex);
	if (nIndex < 0)
		return bDefValue;

	return (_ttoi(m_vValues[nIndex]) & 0xFF);
}

void CSerializationInfo::SetByte(const LPCTSTR lpstrKey, BYTE bValue)
{
	CString strValue;
	strValue.Format(_T("%d"), bValue);
	SetValueCore(lpstrKey, strValue);
}


CString CSerializationInfo::GetString(const LPCTSTR lpstrKey, LPCTSTR lpstrDefValue/* = _T("")*/, int nKeyIndex/* = 0*/)
{
	int nIndex = GetKeyIndex(lpstrKey, nKeyIndex);
	if (nIndex < 0)
		return lpstrDefValue;

	return m_vValues[nIndex];
}

void CSerializationInfo::SetString(const LPCTSTR lpstrKey, const LPCTSTR lpstrValue)
{
	SetValueCore(lpstrKey, lpstrValue);
}


void CSerializationInfo::SetValue(const LPCTSTR lpstrKey, const LPCTSTR lpstrValue)
{
	SetValueCore(lpstrKey, lpstrValue);
}

BOOL CSerializationInfo::HaveValue(const LPCTSTR lpstrKey, int nKeyIndex/* = 0*/)
{
	return GetKeyIndex(lpstrKey, nKeyIndex) >= 0;
}


int CSerializationInfo::GetKeyIndex(const LPCTSTR lpstrKey, int nKeyIndex/* = 0*/)
{
	std::map<CString, std::vector<int>>::iterator pos = m_mapKeys.find(lpstrKey);
	if (pos == m_mapKeys.end())
		return -1;

	if (nKeyIndex >= (int)pos->second.size())
		return -1;

	return pos->second[nKeyIndex];
}



void CSerializationInfo::GetKeyValues(std::vector<std::pair<CString, CString>>* pBuf)
{
	std::map<int, CString> mapOrderedKeys;
	std::map<CString, std::vector<int>>::iterator posKey = m_mapKeys.begin();
	for(; posKey != m_mapKeys.end(); posKey++)
	{
		std::vector<int>::iterator posIndex = posKey->second.begin();
		for(; posIndex != posKey->second.end(); posIndex++)
			mapOrderedKeys[*posIndex] = posKey->first;
	}

	std::map<int, CString>::iterator pos = mapOrderedKeys.begin();
	for(; pos != mapOrderedKeys.end(); pos++)
		pBuf->push_back(std::pair<CString, CString>(pos->second, m_vValues[pos->first]));
}

