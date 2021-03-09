#pragma once
#include "Serializable.h"

class CShortcutInfoNode : public CSerializable
{
public:
	CShortcutInfoNode() { Initialize(); }
	virtual ~CShortcutInfoNode() { Uninitialize(); }
	CShortcutInfoNode(const CShortcutInfoNode& r);

	static LPCTSTR GetEntityNameS() { return _T("ShortcutInfo"); }
	virtual LPCTSTR GetEntityName() override { return GetEntityNameS(); }

	virtual void GetObjectData(CSerializationInfo& info) override;
	virtual void SetObjectData(CSerializationInfo& info) override;

	CShortcutInfoNode& operator=(const CShortcutInfoNode& r);
	bool operator==(const CShortcutInfoNode& r);
	bool operator!=(const CShortcutInfoNode& r);

	void SetReceiverName(CString receiver_name) { m_receiver_name = receiver_name; }
	CString GetReceiverName() const { return m_receiver_name; }

	void SetCommandName(CString command_name) { m_command_name = command_name; }
	CString GetCommandName() const { return m_command_name; }

	void SetCommandID(int command_id) { m_command_id = command_id; }
	int GetCommandID() const { return m_command_id; }

	void SetCtrl(bool ctrl) { m_ctrl = ctrl; }
	bool GetCtrl() const { return m_ctrl; }

	void SetAlt(bool alt) { m_alt = alt; }
	bool GetAlt() const { return m_alt; }

	void SetShift(bool shift) { m_shift = shift; }
	bool GetShift() const { return m_shift; }

	void SetKey(BYTE key) { m_key = key; }
	BYTE GetKey() const { return m_key; }

protected:
	void Initialize();
	void Uninitialize();

private:
	CString m_receiver_name;
	CString m_command_name;
	int m_command_id;
	bool m_ctrl;
	bool m_alt;
	bool m_shift;
	BYTE m_key;
};