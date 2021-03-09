#pragma once
#include <afxwin.h>
#include <resource.h>

class CDlgProductInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProductInfo)

public:
	CDlgProductInfo(CWnd* pParent = NULL);
	virtual ~CDlgProductInfo();

	void Show();
	void Hide();

protected:
	enum { IDD = IDD_PRODUCT_INFO };

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();

	HICON m_hIcon;

private:
	bool Create();

	CStatic m_product_info_label;
	CButton m_ok_button;
};
