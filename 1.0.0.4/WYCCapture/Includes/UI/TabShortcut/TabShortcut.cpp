#include "stdafx.h"
#include "TabShortcut.h"
#include "ShortcutListParser.h"

struct LISTSORTPARAM
{
	int iSortColumn;
	bool bSortDirect;
	CListCtrl* pList;
};

static int CALLBACK SortShortcutListItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	LISTSORTPARAM* list_sort_param = (LISTSORTPARAM*)lParamSort;
	if (!list_sort_param)
		return -1;

	CListCtrl* pList = list_sort_param->pList;
	if (!pList)
		return -1;

	bool bSortDirect = list_sort_param->bSortDirect;
	int iSortColumn = list_sort_param->iSortColumn;

	LVFINDINFO find_info1 = { 0, };
	find_info1.flags = LVFI_PARAM;
	find_info1.lParam = lParam1;

	LVFINDINFO find_info2 = { 0, };
	find_info2.flags = LVFI_PARAM;
	find_info2.lParam = lParam2;

	int row1 = pList->FindItem(&find_info1);
	int row2 = pList->FindItem(&find_info2);

	CString item_text1 = pList->GetItemText(row1, iSortColumn);
	CString item_text2 = pList->GetItemText(row2, iSortColumn);

	return bSortDirect ? _tcscmp(item_text1, item_text2) : -_tcscmp(item_text1, item_text2);
}

IMPLEMENT_DYNAMIC(CTabShortcut, CWnd)

BEGIN_MESSAGE_MAP(CTabShortcut, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TAB_SHORTCUT_SAVE_SHORTCUT_BUTTON, OnBnClickedSaveShortcut)
	ON_BN_CLICKED(IDC_TAB_SHORTCUT_DELETE_SHORTCUT_BUTTON, OnBnClickedDeleteShortcut)
	ON_MESSAGE(UM_WANT_TO_GET_SHORTCUT_LIST, OnWantToGetShortcutList)
END_MESSAGE_MAP()

CTabShortcut::CTabShortcut()
{
	m_shortcut_list_ctrl_index = 0;
	m_last_selected_item = 0;
}

CTabShortcut::~CTabShortcut()
{
}

bool CTabShortcut::Create(CWnd* parent, UINT id, CRect* rect /*= NULL*/)
{
	CRect main_rect(0, 0, 0, 0);
	CRect shortcut_list_ctrl_rect;
	CRect input_shortcut_edit_rect;
	CRect save_shortcut_button_rect;
	CRect delete_shortcut_button_rect;

	if (rect)
		main_rect = rect;

	if (!CWnd::Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, main_rect, parent, id))
		return false;

	if (rect)
	{
		CRect client_rect;
		GetClientRect(client_rect);

		shortcut_list_ctrl_rect = client_rect;
		shortcut_list_ctrl_rect.DeflateRect(5, 5);
		shortcut_list_ctrl_rect.top += 10;
		shortcut_list_ctrl_rect.bottom -= 50;

		input_shortcut_edit_rect = client_rect;
		input_shortcut_edit_rect.DeflateRect(5, 5);
		input_shortcut_edit_rect.top = shortcut_list_ctrl_rect.bottom + 15;
		input_shortcut_edit_rect.right = input_shortcut_edit_rect.left + 150;
		input_shortcut_edit_rect.bottom = input_shortcut_edit_rect.top + 30;

		save_shortcut_button_rect = input_shortcut_edit_rect;
		save_shortcut_button_rect.left = input_shortcut_edit_rect.right + 5;
		save_shortcut_button_rect.right = save_shortcut_button_rect.left + 80;

		delete_shortcut_button_rect = save_shortcut_button_rect;
		delete_shortcut_button_rect.left = save_shortcut_button_rect.right + 5;
		delete_shortcut_button_rect.right = delete_shortcut_button_rect.left + 80;
	}

	if (!m_shortcut_list_ctrl.Create(WS_VISIBLE | WS_CHILD | WS_VSCROLL | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL, shortcut_list_ctrl_rect, this, IDC_TAB_SHORTCUT_SHORTCUT_LIST_CTRL))
		return false;

	if (!m_input_shortcut_edit.Create(WS_VISIBLE | WS_CHILD | ES_LEFT, input_shortcut_edit_rect, this, IDC_TAB_SHORTCUT_INPUT_SHORTCUT_EDIT))
		return false;

	if (!m_save_shortcut_button.Create(_T("Save"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, save_shortcut_button_rect, this, IDC_TAB_SHORTCUT_SAVE_SHORTCUT_BUTTON))
		return false;

	if (!m_delete_shortcut_button.Create(_T("Delete"), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, delete_shortcut_button_rect, this, IDC_TAB_SHORTCUT_DELETE_SHORTCUT_BUTTON))
		return false;

	if (!InitListCtrl())
		return false;

	if (!InitEdit())
		return false;

	if (!InitButton())
		return false;

	return true;
}

void CTabShortcut::InsertShortcutListCtrl(int command_id, CString category, CString name, CString shortcut)
{
	if (!GF()->CheckWindow(m_shortcut_list_ctrl))
		return;

	if (m_shortcut_list_ctrl_index == INT_MAX)
	{
		AfxMessageBox(_T("Shortcut List Control is FULL!"));
		return;
	}

	m_shortcut_list_ctrl.InsertItem(m_shortcut_list_ctrl_index, _T(""));
	m_shortcut_list_ctrl.SetItem(m_shortcut_list_ctrl_index, 0, LVIF_TEXT, CMyString(_T("%d"), command_id), 0, 0, 0, NULL);
	m_shortcut_list_ctrl.SetItem(m_shortcut_list_ctrl_index, 1, LVIF_TEXT, category, 0, 0, 0, NULL);
	m_shortcut_list_ctrl.SetItem(m_shortcut_list_ctrl_index, 2, LVIF_TEXT, name, 0, 0, 0, NULL);
	m_shortcut_list_ctrl.SetItem(m_shortcut_list_ctrl_index, 3, LVIF_TEXT, shortcut, 0, 0, 0, NULL);

	m_shortcut_list_ctrl_index++;
}

void CTabShortcut::ResetShortcutListCtrl()
{
	if (!GF()->CheckWindow(m_shortcut_list_ctrl))
		return;

	m_shortcut_list_ctrl.DeleteAllItems();
	m_shortcut_list_ctrl_index = 0;
}

void CTabShortcut::SortListCtrl()
{
	if (!GF()->CheckWindow(m_shortcut_list_ctrl))
		return;

	int item_count = m_shortcut_list_ctrl.GetItemCount();
	CString** data_string = new CString*[item_count];
	for (int i = 0; i < item_count; i++)
	{
		data_string[i] = new CString(m_shortcut_list_ctrl.GetItemText(i, 1));
		m_shortcut_list_ctrl.SetItemData(i, (DWORD_PTR)data_string[i]);
	}

	LISTSORTPARAM list_sort_param = { 0, };
	list_sort_param.pList = &m_shortcut_list_ctrl;
	list_sort_param.bSortDirect = true; // 오름차순 : false, 내림차순 : true
	list_sort_param.iSortColumn = 1;

	m_shortcut_list_ctrl.SortItems(SortShortcutListItem, (DWORD_PTR)&list_sort_param);

	for (int i = 0; i < item_count; i++)
		delete data_string[i];
	delete[] data_string;
}

BOOL CTabShortcut::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* ph = (NMHDR*)lParam;
	switch (ph->code)
	{
	case LVN_ITEMCHANGED:
	{
		NM_LISTVIEW* list_view = (NM_LISTVIEW*)ph;
		if (list_view->uNewState & LVIS_SELECTED)
			OnShortcutListSelected(ph, pResult);
		break;
	}
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}

BOOL CTabShortcut::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->hwnd == m_input_shortcut_edit.GetSafeHwnd())
		{
			BYTE key = (BYTE)pMsg->wParam;
			EnterShortcutEdit(key);
			return TRUE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

bool CTabShortcut::InitListCtrl()
{
	CRect shortcut_list_ctrl_rect;
	m_shortcut_list_ctrl.GetClientRect(shortcut_list_ctrl_rect);
	m_shortcut_list_ctrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_shortcut_list_ctrl.InsertColumn(0, _T(""), LVCFMT_LEFT, 0);
	m_shortcut_list_ctrl.InsertColumn(1, _T("Category"), LVCFMT_CENTER, shortcut_list_ctrl_rect.Width() * 1 / 4);
	m_shortcut_list_ctrl.InsertColumn(2, _T("Name"), LVCFMT_CENTER, shortcut_list_ctrl_rect.Width() * 3 / 8);
	m_shortcut_list_ctrl.InsertColumn(3, _T("Shortcut"), LVCFMT_CENTER, shortcut_list_ctrl_rect.Width() * 3 / 8);

	ResetShortcutListCtrl();
	
	CString category, name, shortcut;
	CShortcutListNode* shortcut_list_node = CShortcutListParser::GetSingletone()->GetShortcutListNode();

	std::vector<CShortcutInfoNode> shortcut_info_list = shortcut_list_node->GetShortcutInfoNodes();
	std::vector<CShortcutInfoNode>::iterator pos = shortcut_info_list.begin();
	while (pos != shortcut_info_list.end())
	{
		bool ctrl = (*pos).GetCtrl();
		bool alt = (*pos).GetAlt();
		bool shift = (*pos).GetShift();
		BYTE key = (*pos).GetKey();
		CString shortcut_string = CShortcutManager::GetSingletone()->MakeShortcutStrinig(ctrl, alt, shift, key);
		CString receiver_name = (*pos).GetReceiverName();
		CString command_name = (*pos).GetCommandName();
		int command_id = (*pos).GetCommandID();

		category = receiver_name;

		int start = command_name.Find(_T("(&"));
		if (start >= 0)
			name = command_name.Left(start);
		else
		{
			start = command_name.Find(_T("..."));
			if (start >= 0)
				name = command_name.Left(start);
		}

		shortcut = shortcut_string;

		InsertShortcutListCtrl(command_id, category, name, shortcut);
		pos++;
	}
	SortListCtrl();

	m_shortcut_list_ctrl.SetItemState(m_last_selected_item, LVIS_SELECTED, LVIS_SELECTED);
	m_shortcut_list_ctrl.EnsureVisible(m_last_selected_item, FALSE);
	m_shortcut_list_ctrl.SetFocus();

	ChangeEdit();

	return true;
}

bool CTabShortcut::InitEdit()
{
	return true;
}

bool CTabShortcut::InitButton()
{
	return true;
}

int CTabShortcut::GetSelectedShortcutList()
{
	int selected_item = -1;
	if (GF()->CheckWindow(m_shortcut_list_ctrl))
	{
		int item_count = m_shortcut_list_ctrl.GetItemCount();
		for (int i = 0; i < item_count; i++)
		{
			if (m_shortcut_list_ctrl.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			{
				selected_item = i;
				break;
			}
		}
	}

	return selected_item;
}

void CTabShortcut::ChangeEdit()
{
	if (!GF()->CheckWindow(m_input_shortcut_edit))
		return;

	CString shortcut = m_shortcut_list_ctrl.GetItemText(m_last_selected_item, 3);
	m_input_shortcut_edit.SetWindowText(shortcut);
}

void CTabShortcut::EnterShortcutEdit(BYTE key)
{
	bool ctrl = false;
	bool alt = false;
	bool shift = false;
	if (GF()->CheckVirtualKey(VK_CONTROL))
		ctrl = true;
	if (GF()->CheckVirtualKey(VK_MENU))
		alt = true;
	if (GF()->CheckVirtualKey(VK_SHIFT))
		shift = true;

	CString shortcut_string = CShortcutManager::GetSingletone()->MakeShortcutStrinig(ctrl, alt, shift, key);
	m_input_shortcut_edit.SetWindowText(shortcut_string);
}

void CTabShortcut::OnPaint()
{
	if (!GF()->CheckWindow(this))
		return;

	CPaintDC dc(this);

	CRect client_rect;
	GetClientRect(client_rect);

	CRect shortcut_list_ctrl_rect = GF()->GetScreenRect(this, m_shortcut_list_ctrl);
	CRect input_shortcut_edit_rect = GF()->GetScreenRect(this, m_input_shortcut_edit);

	shortcut_list_ctrl_rect.InflateRect(1, 1);
	input_shortcut_edit_rect.InflateRect(1, 1);

	CDC mem_dc;
	CBitmap mem_bmp;

	mem_dc.CreateCompatibleDC(&dc);
	mem_bmp.CreateCompatibleBitmap(&dc, client_rect.Width(), client_rect.Height());

	mem_dc.SelectObject(mem_bmp);
	mem_dc.SetBkMode(TRANSPARENT);

	COLORREF white_color = RGB(255, 255, 255);
	CBrush white_brush(white_color);
	mem_dc.SelectObject(white_brush);

	mem_dc.FillSolidRect(client_rect, white_color);

	mem_dc.Rectangle(shortcut_list_ctrl_rect);
	mem_dc.Rectangle(input_shortcut_edit_rect);

	dc.BitBlt(
		0, 0, client_rect.Width(), client_rect.Height(),
		&mem_dc,
		0, 0,
		SRCCOPY);
}

void CTabShortcut::OnDestroy()
{
	CWnd::OnDestroy();

}

void CTabShortcut::OnBnClickedSaveShortcut()
{
	if (!GF()->CheckWindow(m_input_shortcut_edit) ||
		!GF()->CheckWindow(m_shortcut_list_ctrl))
		return;

	CString shortcut;
	m_input_shortcut_edit.GetWindowText(shortcut);
	m_shortcut_list_ctrl.SetItem(m_last_selected_item, 3, LVIF_TEXT, shortcut, 0, 0, 0, NULL);
}

void CTabShortcut::OnBnClickedDeleteShortcut()
{
	if (!GF()->CheckWindow(m_input_shortcut_edit) ||
		!GF()->CheckWindow(m_shortcut_list_ctrl))
		return;

	m_shortcut_list_ctrl.SetItem(m_last_selected_item, 3, LVIF_TEXT, _T(""), 0, 0, 0, NULL);
	ChangeEdit();
}

void CTabShortcut::OnShortcutListSelected(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE selected = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_last_selected_item = selected->iItem;
	ChangeEdit();
}

LRESULT CTabShortcut::OnWantToGetShortcutList(WPARAM wParam, LPARAM lParam)
{
	if (GF()->CheckWindow(m_shortcut_list_ctrl))
	{
		CListCtrl** pplist_ctrl = (CListCtrl**)wParam;
		if (pplist_ctrl)
			*pplist_ctrl = &m_shortcut_list_ctrl;
	}

	return 0;
}