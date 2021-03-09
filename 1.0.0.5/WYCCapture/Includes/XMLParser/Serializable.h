// TODO : 추후 CDBEntity 의 Base 클래스로 이용하자
#pragma once
#include "SerializationInfo.h"

class CSerializable
{
public:
	virtual LPCTSTR GetEntityName() = 0;

	// Serialize data to info.
	virtual void GetObjectData(CSerializationInfo& info);
	// Load data from ifno.
	virtual void SetObjectData(CSerializationInfo& info);

protected:

};