#pragma once
#include <map>
#include <vector>
#include <list>

// 다음과 같이 노드 정보를 구성한다.
// [ROOT]
//   /-----[CHILD]
//            /-----Values
//         [NEXT]
//            /-----[CHILD]
//                     /-------Values
//         [NEXT]
//            /-----[CHILD]
//                     /-------Values
class CSerializationInfo
{
public:
	CSerializationInfo();
	virtual ~CSerializationInfo();

	CString& GetObjectName() { return m_strObjectName; };
	void SetObjectName(LPCTSTR lpstrName) { m_strObjectName = lpstrName; };

	int GetInt(const LPCTSTR lpstrKey, int nDefValue = 0, int nKeyIndex = 0);
	void SetInt(const LPCTSTR lpstrKey, int nValue);

	DWORD GetDword(const LPCTSTR lpstrKey, DWORD dwDefValue = 0, int nKeyIndex = 0);
	void SetDword(const LPCTSTR lpstrKey, DWORD dwValue);

	INT64 GetInt64(const LPCTSTR lpstrKey, INT64 i64DefValue = 0, int nKeyIndex = 0);
	void SetInt64(const LPCTSTR lpstrKey, INT64 i64Value);

	DOUBLE GetDouble(const LPCTSTR lpstrKey, DOUBLE dDefValue = 0, int nKeyIndex = 0);
	void SetDouble(const LPCTSTR lpstrKey, DOUBLE dValue);

	float GetFloat(const LPCTSTR lpstrKey, float fDefValue = 0, int nKeyIndex = 0);
	void SetFloat(const LPCTSTR lpstrKey, float fValue);

	BOOL GetBool(const LPCTSTR lpstrKey, BOOL bDefValue = FALSE, int nKeyIndex = 0);
	void SetBool(const LPCTSTR lpstrKey, BOOL bValue);

	BYTE GetByte(const LPCTSTR lpstrKey, BYTE bDefValue = 0, int nKeyIndex = 0);
	void SetByte(const LPCTSTR lpstrKey, BYTE bValue);

	CString GetString(const LPCTSTR lpstrKey, LPCTSTR lpstrValue = _T(""), int nKeyIndex = 0);
	void SetString(const LPCTSTR lpstrKey, const LPCTSTR lpstrValue);

	void SetValue(const LPCTSTR lpstrKey, const LPCTSTR lpstrValue);
	BOOL HaveValues() { return m_mapKeys.size() > 0; };
	BOOL HaveValue(const LPCTSTR lpstrKey, int nKeyIndex = 0);

	BOOL GetIsList() { return m_bIsList; }
	void SetIsList(BOOL bValue) { m_bIsList = bValue; }

	template<typename T>
	void GetList(CString strListName, CString strValueName, std::list<T>* pList, CSerializationInfo** ppInfo)
	{
		CSerializationInfo* pNext = NULL;
		*ppInfo = (*ppInfo)->GetNextInfo();
		while(*ppInfo)
		{
			pNext = *ppInfo;
			if (pNext->GetObjectName() == strListName)
			{
				CSerializationInfo* pItem = NULL;
				CSerializationInfo* pValue = NULL;

				pList->clear();
				pItem = pNext->GetChildInfo();
				while(pItem)
				{
					if (pItem->GetObjectName() != strValueName)
						continue;

					pValue = pItem->GetChildInfo();
					pList->push_back(pValue->GetByte(_T("Value")));
					pItem = pItem->GetNextInfo();
				}
			}

			*ppInfo = (*ppInfo)->GetNextInfo();
		}
	};

	template<typename T>
	void SetList(CString strListName, CString strValueName, std::list<T>* pList, CSerializationInfo** ppInfo)
	{
		if (pList->size() > 0)
		{
			CSerializationInfo* pListInfo = (*ppInfo)->CreateNextInfo();
			CSerializationInfo* pItem = NULL;
			CSerializationInfo* pValue = NULL;
			pListInfo->SetObjectName(strListName);
			std::list<T>::iterator pos= pList->begin();
			while(pos != pList->end())
			{
				if (pos == pList->begin())
					pItem = pListInfo->CreateChildInfo();
				else
					pItem = pItem->CreateNextInfo();

				pItem->SetObjectName(strValueName);
				pValue = pItem->CreateChildInfo();
				pValue->SetInt(_T("Value"), (*pos));
				pos++;
			}

			*ppInfo = pListInfo;
		}
	};

	CSerializationInfo* CreateNextInfo()
	{
		if (m_pNextInfo)
		{
			(*m_ppLastNextInfo)->m_pNextInfo = new CSerializationInfo;
			(*m_ppLastNextInfo)->m_pNextInfo->m_pPrevInfo = (*m_ppLastNextInfo);
			(*m_ppLastNextInfo)->m_pNextInfo->m_pParentInfo = m_pParentInfo;
			(*m_ppLastNextInfo)->m_pNextInfo->m_nDepth = m_nDepth;
			m_ppLastNextInfo = &((*m_ppLastNextInfo)->m_pNextInfo);
			return *m_ppLastNextInfo;
		}
		else
		{
			m_pNextInfo = new CSerializationInfo;
			m_pNextInfo->m_pPrevInfo = this;
			m_pNextInfo->m_pParentInfo = m_pParentInfo;
			m_pNextInfo->m_nDepth = m_nDepth;
			m_ppLastNextInfo = &m_pNextInfo;
			return m_pNextInfo;
		}
	};

	CSerializationInfo* CreateChildInfo()
	{
		if (m_pChildInfo)
			return m_pChildInfo;

		m_pChildInfo = new CSerializationInfo;
		m_pChildInfo->m_pParentInfo = this;
		m_pChildInfo->m_nDepth = m_nDepth + 1;
		return m_pChildInfo;
	};

	void DeleteChildInfo()
	{
		if (!m_pChildInfo)
			return;

		delete m_pChildInfo;
		m_pChildInfo = NULL;
	}

	CSerializationInfo* GetNextInfo()
	{
		return m_pNextInfo;
	};


	CSerializationInfo* GetPrevInfo()
	{
		return m_pPrevInfo;
	};

	CSerializationInfo* GetParentInfo()
	{
		return m_pParentInfo;
	};

	CSerializationInfo* GetChildInfo()
	{
		return m_pChildInfo;
	};

	void GetKeyValues(std::vector<std::pair<CString, CString>>* pBuf);
	void ClearKeyValues()
	{
		m_mapKeys.clear();
		m_vValues.clear();
	};

	int GetDepth() { return m_nDepth; };

	std::map<CString, std::vector<int>>* GetKeyContainer() { return &m_mapKeys; }
	std::vector<CString>* GetValueContainer() { return &m_vValues; }

protected:
	CString m_strObjectName;
	// Value 태그명을 별도의 리스트 태그를 두지 않고 사용하는 경우가 있어
	// 처리를 위해 키값에 대한 Value 인덱스 리스트로 관리하도록 수정함
	std::map<CString, std::vector<int>> m_mapKeys;
	std::vector<CString> m_vValues;
	CSerializationInfo* m_pPrevInfo;
	CSerializationInfo* m_pNextInfo;
	CSerializationInfo** m_ppLastNextInfo;
	CSerializationInfo* m_pChildInfo;
	CSerializationInfo* m_pParentInfo;
	int m_nDepth;
	BOOL m_bIsList;

	void SetValueCore(const LPCTSTR lpstrKey, const LPCTSTR lpstrValue);
	int GetKeyIndex(const LPCTSTR lpstrKey, int nKeyIndex = 0);
};