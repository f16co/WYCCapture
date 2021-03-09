#pragma once
#include "Serializable.h"
#include "ShortcutInfoNode.h"
#include <vector>

class CShortcutListNode : public CSerializable
{
public:
	CShortcutListNode() { Initialize(); }
	virtual ~CShortcutListNode() { Uninitialize(); }
	CShortcutListNode(const CShortcutListNode& r);

	static LPCTSTR GetEntityNameS() { return _T("ShortcutList"); }
	virtual LPCTSTR GetEntityName() override { return GetEntityNameS(); }

	virtual void GetObjectData(CSerializationInfo& info) override;
	virtual void SetObjectData(CSerializationInfo& info) override;

	CShortcutListNode& operator=(const CShortcutListNode& r);
	bool operator==(const CShortcutListNode& r);
	bool operator!=(const CShortcutListNode& r);

	void SetShortcutInfoNodes(std::vector<CShortcutInfoNode>& shortcut_info_nodes) { m_shortcut_info_nodes = shortcut_info_nodes; }
	std::vector<CShortcutInfoNode> GetShortcutInfoNodes() const { return m_shortcut_info_nodes; }

	CString GetCommandName(UINT64 shortcut_id);
	CString GetReceiverName(UINT64 shortcut_id);

protected:
	void Initialize();
	void Uninitialize();

private:
	std::vector<CShortcutInfoNode> m_shortcut_info_nodes;
};