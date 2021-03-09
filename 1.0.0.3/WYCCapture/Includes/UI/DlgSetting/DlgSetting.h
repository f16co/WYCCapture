#pragma once
#include <afxwin.h>
#include <resource.h>
#include "TabShortcut.h"

class CDlgSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetting)

public:
	CDlgSetting(CWnd* pParent = NULL);
	virtual ~CDlgSetting();

	int GetLastSelectedItem() const { return m_tab_shortcut.GetLastSelectedItem(); }
	void SetLastSelectedItem(int last_selected_item) { m_tab_shortcut.SetLastSelectedItem(last_selected_item); }

protected:
	enum { IDD = IDD_SETTING };

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();

	HICON m_hIcon;

private:
	bool Create();

	CTabCtrl m_tab_ctrl;
	CTabShortcut m_tab_shortcut;
	CButton m_ok_button;
};
