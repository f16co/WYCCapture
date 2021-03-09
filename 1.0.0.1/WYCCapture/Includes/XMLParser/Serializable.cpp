#include "stdafx.h"
#include "Serializable.h"

void CSerializable::GetObjectData(CSerializationInfo& info)
{
	//info.SetObjectName(GetEntityName());
	//CSerializationInfo* pChild = info.CreateChildInfo();
	//std::vector<CValueInfo> vValues = GetValues();
	//std::vector<CValueInfo>::iterator posValue = vValues.begin();
	//for(; posValue != vValues.end(); posValue++)
	//	pChild->SetValue((*posValue).GetName(), (*posValue).GetValue());
}


void CSerializable::SetObjectData(CSerializationInfo& info)
{
	//if (info.GetObjectName().CompareNoCase(GetEntityName()))
	//	return;

	//CSerializationInfo* pChild = info.GetChildInfo();
	//CKeyValueData clsKeyValueData(pChild->GetKeyContainer(), pChild->GetValueContainer());
	//Set(&clsKeyValueData);
}