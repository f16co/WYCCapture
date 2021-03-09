#pragma once
#include "Serializable.h"

class CGlobalSettingNode : public CSerializable
{
public:
	CGlobalSettingNode() { Initialize(); }
	virtual ~CGlobalSettingNode() { Uninitialize(); }
	CGlobalSettingNode(const CGlobalSettingNode& r);

	static LPCTSTR GetEntityNameS() { return _T("GlobalSetting"); }
	virtual LPCTSTR GetEntityName() override { return GetEntityNameS(); }

	virtual void GetObjectData(CSerializationInfo& info) override;
	virtual void SetObjectData(CSerializationInfo& info) override;

	CGlobalSettingNode& operator=(const CGlobalSettingNode& r);
	bool operator==(const CGlobalSettingNode& r);
	bool operator!=(const CGlobalSettingNode& r);

	void SetDebugLogUse(bool is_debug_log_use) { m_is_debug_log_use = is_debug_log_use; }
	bool GetDebugLogUse() const { return m_is_debug_log_use; }

protected:
	void Initialize();
	void Uninitialize();

private:
	bool m_is_debug_log_use;
};