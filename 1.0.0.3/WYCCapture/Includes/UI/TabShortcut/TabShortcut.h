#pragma once
#include <afxwin.h>

class CTabShortcut : public CWnd
{
	DECLARE_DYNAMIC(CTabShortcut)

public:
	CTabShortcut();
	virtual ~CTabShortcut();

	bool Create(CWnd* parent, UINT id, CRect* rect = NULL);
	void InsertShortcutListCtrl(int command_id, CString category, CString name, CString shortcut);
	void ResetShortcutListCtrl();
	void SortListCtrl();

	int GetLastSelectedItem() const { return m_last_selected_item; }
	void SetLastSelectedItem(int last_selected_item) { m_last_selected_item = last_selected_item; }

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedSaveShortcut();
	afx_msg void OnBnClickedDeleteShortcut();
	afx_msg void OnShortcutListSelected(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnWantToGetShortcutList(WPARAM wParam, LPARAM lParam);

private:
	bool InitListCtrl();
	bool InitEdit();
	bool InitButton();

	int GetSelectedShortcutList();
	void ChangeEdit();
	void EnterShortcutEdit(BYTE key);

	CListCtrl m_shortcut_list_ctrl;
	CEdit m_input_shortcut_edit;
	CButton m_save_shortcut_button;
	CButton m_delete_shortcut_button;

	int m_shortcut_list_ctrl_index;
	int m_last_selected_item;
};
