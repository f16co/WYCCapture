#include "stdafx.h"
#include "DlgSetting.h"
#include "afxdialogex.h"
#include "ShortcutListParser.h"

IMPLEMENT_DYNAMIC(CDlgSetting, CDialogEx)

BEGIN_MESSAGE_MAP(CDlgSetting, CDialogEx)
	ON_BN_CLICKED(IDC_DLG_SETTING_OK_BUTTON, OnBnClickedOk)
END_MESSAGE_MAP()

CDlgSetting::CDlgSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDlgSetting::~CDlgSetting()
{
}

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CDlgSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	if (!Create())
		return FALSE;

	return TRUE;
}

void CDlgSetting::OnBnClickedOk()
{
	if (GF()->CheckWindow(m_tab_shortcut))
	{
		CListCtrl* shortcut_list_ctrl = NULL;
		m_tab_shortcut.SendMessage(UM_WANT_TO_GET_SHORTCUT_LIST, (WPARAM)&shortcut_list_ctrl);

		if (shortcut_list_ctrl)
		{
			CShortcutListNode* shortcut_list_node = CShortcutListParser::GetSingletone()->GetShortcutListNode();
			std::vector<CShortcutInfoNode> shortcut_info_nodes = shortcut_list_node->GetShortcutInfoNodes();

			int item_count = shortcut_list_ctrl->GetItemCount();
			for (int i = 0; i < item_count; i++)
			{
				int command_id = _ttoi(shortcut_list_ctrl->GetItemText(i, 0));
				CString category = shortcut_list_ctrl->GetItemText(i, 1);
				CString name = shortcut_list_ctrl->GetItemText(i, 2);
				CString shortcut = shortcut_list_ctrl->GetItemText(i, 3);

				SHORTCUT_DATA shortcut_data = CShortcutManager::GetSingletone()->ParseShortcutString(shortcut);

				std::vector<CShortcutInfoNode>::iterator pos = shortcut_info_nodes.begin();
				while (pos != shortcut_info_nodes.end())
				{
					if ((*pos).GetReceiverName().Find(category) >= 0 &&
						(*pos).GetCommandName().Find(name) >= 0 &&
						(*pos).GetCommandID() == command_id)
					{
						(*pos).SetCtrl(shortcut_data.ctrl);
						(*pos).SetAlt(shortcut_data.alt);
						(*pos).SetShift(shortcut_data.shift);
						(*pos).SetKey(shortcut_data.key);

						break;
					}
					pos++;
				}
			}

			shortcut_list_node->SetShortcutInfoNodes(shortcut_info_nodes);
			//if (!CShortcutListParser::GetSingletone()->SaveDataToFile(GI()->GetShortcutPath()).IsEmpty())
			//	WM_POST_MESSAGE(UM_SHORTCUT_CHANGED);
			if (!CShortcutListParser::GetSingletone()->SaveDataToFile(GI()->GetShortcutPath()).IsEmpty())
				WM_SEND_MESSAGE(UM_SHORTCUT_CHANGED);
		}
	}
	CDialogEx::OnOK();
}

bool CDlgSetting::Create()
{
	SetWindowText(_T("Setting"));

	CRect desktop_rect;
	GetDesktopWindow()->GetClientRect(desktop_rect);

	int w = desktop_rect.Width() / 3;
	int h = desktop_rect.Height() / 4;
	int x = desktop_rect.left + w;
	int y = desktop_rect.top + h;

	CRect main_rect(x, y, x + w, y + h * 2);
	MoveWindow(main_rect);
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

	CRect client_rect;
	GetClientRect(client_rect);

	CRect tab_control_rect(client_rect);
	tab_control_rect.DeflateRect(5, 5);
	tab_control_rect.bottom -= 35;
	CRect tab_shortcut_rect(tab_control_rect);
	tab_shortcut_rect.DeflateRect(2, 2);
	tab_shortcut_rect.right -= 15;
	tab_shortcut_rect.bottom -= 10;
	tab_shortcut_rect.top += 20;
	CRect ok_button_rect;
	ok_button_rect.right = tab_control_rect.right;
	ok_button_rect.left = ok_button_rect.right - 70;
	ok_button_rect.top = tab_control_rect.bottom + 5;
	ok_button_rect.bottom = ok_button_rect.top + 25;

	if (!m_tab_ctrl.Create(WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TCS_TABS, tab_control_rect, this, IDC_DLG_SETTING_TAB_CTRL))
		return false;

	TCITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = _T("Shortcut");
	m_tab_ctrl.InsertItem(0, &item);

	if (!m_tab_shortcut.Create(&m_tab_ctrl, IDC_DLG_SETTING_TAB_SHORTCUT, &tab_shortcut_rect))
		return false;

	if (!m_ok_button.Create(_T("Ok"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, ok_button_rect, this, IDC_DLG_SETTING_OK_BUTTON))
		return false;

	return true;
}
