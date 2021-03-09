// TODO : ���� CDBEntity �� Base Ŭ������ �̿�����
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