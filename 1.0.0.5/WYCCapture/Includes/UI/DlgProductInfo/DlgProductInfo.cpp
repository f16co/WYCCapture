#include "stdafx.h"
#include "DlgProductInfo.h"
#include "afxdialogex.h"

#define DEFAULT_PRODUCT_INFO_WND_WIDTH 400
#define DEFAULT_PRODUCT_INFO_WND_HEIGHT 200

IMPLEMENT_DYNAMIC(CDlgProductInfo, CDialogEx)

BEGIN_MESSAGE_MAP(CDlgProductInfo, CDialogEx)
	ON_BN_CLICKED(IDC_DLG_PRODUCT_INFO_OK_BUTTON, OnBnClickedOk)
END_MESSAGE_MAP()

CDlgProductInfo::CDlgProductInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDlgProductInfo::~CDlgProductInfo()
{
}

void CDlgProductInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CDlgProductInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	if (!Create())
		return FALSE;

	return TRUE;
}

void CDlgProductInfo::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

bool CDlgProductInfo::Create()
{
	SetWindowText(_T("Product Info"));

	HWND hwnd_desktop = ::GetDesktopWindow();
	CRect rect_desktop;
	::GetClientRect(hwnd_desktop, rect_desktop);

	CPoint center_point = rect_desktop.CenterPoint();

	MoveWindow(
		center_point.x - DEFAULT_PRODUCT_INFO_WND_WIDTH / 2,
		center_point.y - DEFAULT_PRODUCT_INFO_WND_HEIGHT / 2,
		DEFAULT_PRODUCT_INFO_WND_WIDTH,
		DEFAULT_PRODUCT_INFO_WND_HEIGHT);

	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	CRect client_rect;
	GetClientRect(client_rect);

	CRect product_info_label_rect(client_rect);
	product_info_label_rect.DeflateRect(10, 10);
	product_info_label_rect.top = client_rect.CenterPoint().y - 40;
	product_info_label_rect.bottom -= 30;
	CString product_info_label_string;
	product_info_label_string.Format(_T("%s (%s)"), GI()->GetProductName(), GF()->GetVersion());
	if (!m_product_info_label.Create(product_info_label_string, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SS_CENTER, product_info_label_rect, this, IDC_DLG_PRODUCT_INFO_PRODUCT_INFO_LABEL))
		return false;

	CRect ok_button_rect(client_rect);
	ok_button_rect.DeflateRect(10, 10);
	ok_button_rect.top = product_info_label_rect.bottom;
	ok_button_rect.left = ok_button_rect.right - 80;
	if (!m_ok_button.Create(_T("Ok"), WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, ok_button_rect, this, IDC_DLG_PRODUCT_INFO_OK_BUTTON))
		return false;

	return true;
}

void CDlgProductInfo::Show()
{
	if (!GF()->CheckWindow(this) ||
		!GF()->CheckWindow(m_ok_button) ||
		!GF()->CheckWindow(m_product_info_label))
		return;

	ShowWindow(SW_SHOW);
	m_ok_button.ShowWindow(SW_SHOW);
	m_product_info_label.ShowWindow(SW_SHOW);
}

void CDlgProductInfo::Hide()
{
	if (!GF()->CheckWindow(this) ||
		!GF()->CheckWindow(m_ok_button) ||
		!GF()->CheckWindow(m_product_info_label))
		return;

	ShowWindow(SW_HIDE);
	m_ok_button.ShowWindow(SW_HIDE);
	m_product_info_label.ShowWindow(SW_HIDE);
}
