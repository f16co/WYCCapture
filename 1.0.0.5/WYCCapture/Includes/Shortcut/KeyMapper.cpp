#include "stdafx.h"
#include "KeyMapper.h"
#include <vector>


CString CKeyMapper::MakeKeyString(bool ctrl, bool alt, bool shift, BYTE key)
{
	CString key_string;

	if (ctrl)
		key_string += _T("Ctrl+");

	if (alt)
		key_string += _T("Alt+");

	if (shift)
		key_string += _T("Shift+");

	key_string += KeyToString(key);

	return key_string;
}

bool CKeyMapper::ParseKeyString(CString key_string, bool* ctrl, bool* alt, bool* shift, BYTE* key)
{
	if (!ctrl || !alt || !shift || !key || key_string.IsEmpty())
		return false;

	key_string.Replace(_T(" "), _T(""));

	*ctrl = CheckCtrl(key_string);
	*alt = CheckAlt(key_string);
	*shift = CheckShift(key_string);
	*key = StringToKey(key_string);

	return true;
}

CString CKeyMapper::KeyToString(BYTE key)
{
	CString key_string;

	switch (key)
	{
	case VK_ESCAPE:
		key_string = _T("Esc");
		break;
	case VK_TAB:
		key_string = _T("Tab");
		break;
	case VK_RETURN:
		key_string = _T("Retyrb");
		break;
	case VK_CAPITAL:
		key_string = _T("Capslock");
		break;
	case VK_SPACE:
		key_string = _T("Space");
		break;
	case VK_PRIOR:
		key_string = _T("PageUp");
		break;
	case VK_NEXT:
		key_string = _T("PageDown");
		break;
	case VK_END:
		key_string = _T("End");
		break;
	case VK_HOME:
		key_string = _T("Home");
		break;
	case VK_LEFT:
		key_string = _T("Left");
		break;
	case VK_RIGHT:
		key_string = _T("Right");
		break;
	case VK_UP:
		key_string = _T("Up");
		break;
	case VK_DOWN:
		key_string = _T("Down");
		break;
	case VK_SNAPSHOT:
		key_string = _T("PrintScreen");
		break;
	case VK_INSERT:
		key_string = _T("Inser");
		break;
	case VK_DELETE:
		key_string = _T("Delete");
		break;
	case VK_NUMPAD0:
		key_string = _T("NumPad0");
		break;
	case VK_NUMPAD1:
		key_string = _T("NumPad1");
		break;
	case VK_NUMPAD2:
		key_string = _T("NumPad2");
		break;
	case VK_NUMPAD3:
		key_string = _T("NumPad3");
		break;
	case VK_NUMPAD4:
		key_string = _T("NumPad4");
		break;
	case VK_NUMPAD5:
		key_string = _T("NumPad5");
		break;
	case VK_NUMPAD6:
		key_string = _T("NumPad6");
		break;
	case VK_NUMPAD7:
		key_string = _T("NumPad7");
		break;
	case VK_NUMPAD8:
		key_string = _T("NumPad8");
		break;
	case VK_NUMPAD9:
		key_string = _T("NumPad9");
		break;
	case VK_MULTIPLY:
		key_string = _T("*");
		break;
	case VK_ADD:
		key_string = _T("+");
		break;
	case VK_SUBTRACT:
		key_string = _T("-");
		break;
	case VK_DECIMAL:
	case VK_OEM_PERIOD:
	case VK_DECIMAL | VK_OEM_PERIOD:
		key_string = _T(".");
		break;
	case VK_DIVIDE:
	case VK_OEM_2:
	case VK_DIVIDE | VK_OEM_2:
		key_string = _T("/");
		break;
	case VK_F1:
		key_string = _T("F1");
		break;
	case VK_F2:
		key_string = _T("F2");
		break;
	case VK_F3:
		key_string = _T("F3");
		break;
	case VK_F4:
		key_string = _T("F4");
		break;
	case VK_F5:
		key_string = _T("F5");
		break;
	case VK_F6:
		key_string = _T("F6");
		break;
	case VK_F7:
		key_string = _T("F7");
		break;
	case VK_F8:
		key_string = _T("F8");
		break;
	case VK_F9:
		key_string = _T("F9");
		break;
	case VK_F10:
		key_string = _T("F10");
		break;
	case VK_F11:
		key_string = _T("F11");
		break;
	case VK_F12:
		key_string = _T("F12");
		break;
	case VK_NUMLOCK:
		key_string = _T("NumberLock");
		break;
	case VK_SCROLL:
		key_string = _T("ScrollLock");
		break;
	case VK_OEM_1:
		key_string = _T(";");
		break;
	case VK_OEM_PLUS:
		key_string = _T("=");
		break;
	case VK_OEM_3:
		key_string = _T("`");
		break;
	case VK_OEM_4:
		key_string = _T("[");
		break;
	case VK_OEM_6:
		key_string = _T("]");
		break;
	case VK_OEM_7:
		key_string = _T("\'");
		break;
	case VK_OEM_5:
		key_string = _T("\\");
		break;
	case VK_0:
		key_string = _T("0");
		break;
	case VK_1:
		key_string = _T("1");
		break;
	case VK_2:
		key_string = _T("2");
		break;
	case VK_3:
		key_string = _T("3");
		break;
	case VK_4:
		key_string = _T("4");
		break;
	case VK_5:
		key_string = _T("5");
		break;
	case VK_6:
		key_string = _T("6");
		break;
	case VK_7:
		key_string = _T("7");
		break;
	case VK_8:
		key_string = _T("8");
		break;
	case VK_9:
		key_string = _T("9");
		break;
	case VK_A:
		key_string = _T("A");
		break;
	case VK_B:
		key_string = _T("B");
		break;
	case VK_C:
		key_string = _T("C");
		break;
	case VK_D:
		key_string = _T("D");
		break;
	case VK_E:
		key_string = _T("E");
		break;
	case VK_F:
		key_string = _T("F");
		break;
	case VK_G:
		key_string = _T("G");
		break;
	case VK_H:
		key_string = _T("H");
		break;
	case VK_I:
		key_string = _T("I");
		break;
	case VK_J:
		key_string = _T("J");
		break;
	case VK_K:
		key_string = _T("K");
		break;
	case VK_L:
		key_string = _T("L");
		break;
	case VK_M:
		key_string = _T("M");
		break;
	case VK_N:
		key_string = _T("N");
		break;
	case VK_O:
		key_string = _T("O");
		break;
	case VK_P:
		key_string = _T("P");
		break;
	case VK_Q:
		key_string = _T("Q");
		break;
	case VK_R:
		key_string = _T("R");
		break;
	case VK_S:
		key_string = _T("S");
		break;
	case VK_T:
		key_string = _T("T");
		break;
	case VK_U:
		key_string = _T("U");
		break;
	case VK_V:
		key_string = _T("V");
		break;
	case VK_W:
		key_string = _T("W");
		break;
	case VK_X:
		key_string = _T("X");
		break;
	case VK_Y:
		key_string = _T("Y");
		break;
	case VK_Z:
		key_string = _T("Z");
		break;
	}

	return key_string;
}

BYTE CKeyMapper::StringToKey(CString key_string)
{
	if (key_string.IsEmpty())
		return 0;

	BYTE key = 0;

	std::vector<CString> key_vector;
	int start = 0;
	while (true)
	{
		CString string_to_push = key_string.Tokenize(_T("+"), start);
		if (start < 0)
			break;
		key_vector.push_back(string_to_push);
	}

	int key_vector_size = (int)key_vector.size();
	if (key_vector_size <= 0)
		return 0;

	CString last_key = key_vector[key_vector_size - 1];

	if (IsEsc(last_key))
		key = VK_ESCAPE;
	else if (IsTab(last_key))
		key = VK_TAB;
	else if (IsReturn(last_key))
		key = VK_RETURN;
	else if (IsCapsLock(last_key))
		key = VK_CAPITAL;
	else if (IsSpace(last_key))
		key = VK_SPACE;
	else if (IsPageUp(last_key))
		key = VK_PRIOR;
	else if (IsPageDown(last_key))
		key = VK_NEXT;
	else if (IsEnd(last_key))
		key = VK_END;
	else if (IsHome(last_key))
		key = VK_HOME;
	else if (IsLeft(last_key))
		key = VK_LEFT;
	else if (IsRight(last_key))
		key = VK_RIGHT;
	else if (IsUp(last_key))
		key = VK_UP;
	else if (IsDown(last_key))
		key = VK_DOWN;
	else if (IsPrintScreen(last_key))
		key = VK_SNAPSHOT;
	else if (IsInsert(last_key))
		key = VK_INSERT;
	else if (IsDelete(last_key))
		key = VK_DELETE;
	else if (IsNumpad0(last_key))
		key = VK_NUMPAD0;
	else if (IsNumpad1(last_key))
		key = VK_NUMPAD1;
	else if (IsNumpad2(last_key))
		key = VK_NUMPAD2;
	else if (IsNumpad3(last_key))
		key = VK_NUMPAD3;
	else if (IsNumpad4(last_key))
		key = VK_NUMPAD4;
	else if (IsNumpad5(last_key))
		key = VK_NUMPAD5;
	else if (IsNumpad6(last_key))
		key = VK_NUMPAD6;
	else if (IsNumpad7(last_key))
		key = VK_NUMPAD7;
	else if (IsNumpad8(last_key))
		key = VK_NUMPAD8;
	else if (IsNumpad9(last_key))
		key = VK_NUMPAD9;
	else if (IsMultiply(last_key))
		key = VK_MULTIPLY;
	else if (IsAdd(last_key))
		key = VK_ADD;
	else if (IsSubtract(last_key))
		key = VK_SUBTRACT;
	else if (IsDot(last_key))
		key = VK_DECIMAL | VK_OEM_PERIOD;
	else if (IsDivide(last_key))
		key = VK_DIVIDE | VK_OEM_2;
	else if (IsF1(last_key))
		key = VK_F1;
	else if (IsF2(last_key))
		key = VK_F2;
	else if (IsF3(last_key))
		key = VK_F3;
	else if (IsF4(last_key))
		key = VK_F4;
	else if (IsF5(last_key))
		key = VK_F5;
	else if (IsF6(last_key))
		key = VK_F6;
	else if (IsF7(last_key))
		key = VK_F7;
	else if (IsF8(last_key))
		key = VK_F8;
	else if (IsF9(last_key))
		key = VK_F9;
	else if (IsF10(last_key))
		key = VK_F10;
	else if (IsF11(last_key))
		key = VK_F11;
	else if (IsF12(last_key))
		key = VK_F12;
	else if (IsNumLock(last_key))
		key = VK_NUMLOCK;
	else if (IsScrollLock(last_key))
		key = VK_SCROLL;
	else if (IsSemicolon(last_key))
		key = VK_OEM_1;
	else if (IsEqual(last_key))
		key = VK_OEM_PLUS;
	else if (IsComma(last_key))
		key = VK_OEM_COMMA;
	else if (IsUpperDot(last_key))
		key = VK_OEM_3;
	else if (IsSquareBracketStart(last_key))
		key = VK_OEM_4;
	else if (IsSquareBracketEnd(last_key))
		key = VK_OEM_6;
	else if (IsQuotationMarks(last_key))
		key = VK_OEM_7;
	else if (IsDollar(last_key))
		key = VK_OEM_5;
	else if (Is0(last_key))
		key = VK_0;
	else if (Is1(last_key))
		key = VK_1;
	else if (Is2(last_key))
		key = VK_2;
	else if (Is3(last_key))
		key = VK_3;
	else if (Is4(last_key))
		key = VK_4;
	else if (Is5(last_key))
		key = VK_5;
	else if (Is6(last_key))
		key = VK_6;
	else if (Is7(last_key))
		key = VK_7;
	else if (Is8(last_key))
		key = VK_8;
	else if (Is9(last_key))
		key = VK_9;
	else if (IsA(last_key))
		key = VK_A;
	else if (IsB(last_key))
		key = VK_B;
	else if (IsC(last_key))
		key = VK_C;
	else if (IsD(last_key))
		key = VK_D;
	else if (IsE(last_key))
		key = VK_E;
	else if (IsF(last_key))
		key = VK_F;
	else if (IsG(last_key))
		key = VK_G;
	else if (IsH(last_key))
		key = VK_H;
	else if (IsI(last_key))
		key = VK_I;
	else if (IsJ(last_key))
		key = VK_J;
	else if (IsK(last_key))
		key = VK_K;
	else if (IsL(last_key))
		key = VK_L;
	else if (IsM(last_key))
		key = VK_M;
	else if (IsN(last_key))
		key = VK_N;
	else if (IsO(last_key))
		key = VK_O;
	else if (IsP(last_key))
		key = VK_P;
	else if (IsQ(last_key))
		key = VK_Q;
	else if (IsR(last_key))
		key = VK_R;
	else if (IsS(last_key))
		key = VK_S;
	else if (IsT(last_key))
		key = VK_T;
	else if (IsU(last_key))
		key = VK_U;
	else if (IsV(last_key))
		key = VK_V;
	else if (IsW(last_key))
		key = VK_W;
	else if (IsX(last_key))
		key = VK_X;
	else if (IsY(last_key))
		key = VK_Y;
	else if (IsZ(last_key))
		key = VK_Z;

	return key;
}

bool CKeyMapper::CheckCtrl(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("CTRL")) >= 0);
}

bool CKeyMapper::CheckAlt(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("ALT")) >= 0);
}

bool CKeyMapper::CheckShift(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("SHIFT")) >= 0);
}

bool CKeyMapper::IsEsc(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("ESCAPE")) >= 0 ||
			key_string.Find(_T("ESC")) >= 0);
}

bool CKeyMapper::IsTab(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("TAB")) >= 0);
}

bool CKeyMapper::IsReturn(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("RETURN")) >= 0);
}

bool CKeyMapper::IsCapsLock(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("CAPSLOCK")) >= 0);
}

bool CKeyMapper::IsSpace(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("SPACE")) >= 0);
}

bool CKeyMapper::IsPageUp(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("PAGEUP")) >= 0 ||
			key_string.Find(_T("PGUP")) >= 0);
}

bool CKeyMapper::IsPageDown(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("PAGEDOWN")) >= 0 ||
			key_string.Find(_T("PGDN")) >= 0);
}

bool CKeyMapper::IsEnd(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("END")) >= 0);
}

bool CKeyMapper::IsHome(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("HOME")) >= 0);
}

bool CKeyMapper::IsLeft(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("LEFT")) >= 0);
}

bool CKeyMapper::IsRight(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("RIGHT")) >= 0);
}

bool CKeyMapper::IsUp(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("UP")) >= 0);
}

bool CKeyMapper::IsDown(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("DOWN")) >= 0);
}

bool CKeyMapper::IsPrintScreen(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("PRINTSCREEN")) >= 0);
}

bool CKeyMapper::IsInsert(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("INSERT")) >= 0 ||
			key_string.Find(_T("INS")) >= 0);
}

bool CKeyMapper::IsDelete(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("DELETE")) >= 0 ||
			key_string.Find(_T("DEL")) >= 0);
}

bool CKeyMapper::IsNumpad0(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD0")) >= 0 ||
			key_string.Find(_T("NUM0")) >= 0);
}

bool CKeyMapper::IsNumpad1(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD1")) >= 0 ||
			key_string.Find(_T("NUM1")) >= 0);
}

bool CKeyMapper::IsNumpad2(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD2")) >= 0 ||
			key_string.Find(_T("NUM2")) >= 0);
}

bool CKeyMapper::IsNumpad3(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD3")) >= 0 ||
			key_string.Find(_T("NUM3")) >= 0);
}

bool CKeyMapper::IsNumpad4(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD4")) >= 0 ||
			key_string.Find(_T("NUM4")) >= 0);
}

bool CKeyMapper::IsNumpad5(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD5")) >= 0 ||
			key_string.Find(_T("NUM5")) >= 0);
}

bool CKeyMapper::IsNumpad6(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD6")) >= 0 ||
			key_string.Find(_T("NUM6")) >= 0);
}

bool CKeyMapper::IsNumpad7(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD7")) >= 0 ||
			key_string.Find(_T("NUM7")) >= 0);
}

bool CKeyMapper::IsNumpad8(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD8")) >= 0 ||
			key_string.Find(_T("NUM8")) >= 0);
}

bool CKeyMapper::IsNumpad9(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMPAD9")) >= 0 ||
			key_string.Find(_T("NUM9")) >= 0);
}

bool CKeyMapper::IsMultiply(CString key_string)
{
	return (key_string.Find(_T("*")) >= 0);
}

bool CKeyMapper::IsAdd(CString key_string)
{
	return (key_string.Find(_T("+")) >= 0);
}

bool CKeyMapper::IsSubtract(CString key_string)
{
	return (key_string.Find(_T("-")) >= 0);
}

bool CKeyMapper::IsDot(CString key_string)
{
	return (key_string.Find(_T(".")) >= 0);
}

bool CKeyMapper::IsDivide(CString key_string)
{
	return (key_string.Find(_T("/")) >= 0);
}

bool CKeyMapper::IsF1(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F1")) >= 0);
}

bool CKeyMapper::IsF2(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F2")) >= 0);
}

bool CKeyMapper::IsF3(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F3")) >= 0);
}

bool CKeyMapper::IsF4(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F4")) >= 0);
}

bool CKeyMapper::IsF5(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F5")) >= 0);
}

bool CKeyMapper::IsF6(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F6")) >= 0);
}

bool CKeyMapper::IsF7(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F7")) >= 0);
}

bool CKeyMapper::IsF8(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F8")) >= 0);
}

bool CKeyMapper::IsF9(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F9")) >= 0);
}

bool CKeyMapper::IsF10(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F10")) >= 0);
}

bool CKeyMapper::IsF11(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F11")) >= 0);
}

bool CKeyMapper::IsF12(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F12")) >= 0);
}

bool CKeyMapper::IsNumLock(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("NUMBERLOCK")) >= 0 ||
			key_string.Find(_T("NUMLOCK")) >= 0);
}

bool CKeyMapper::IsScrollLock(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("SCROLLLOCK")) >= 0);
}

bool CKeyMapper::IsSemicolon(CString key_string)
{
	return (key_string.Find(_T(";")) >= 0);
}

bool CKeyMapper::IsEqual(CString key_string)
{
	return (key_string.Find(_T("=")) >= 0);
}

bool CKeyMapper::IsComma(CString key_string)
{
	return (key_string.Find(_T(",")) >= 0);
}

bool CKeyMapper::IsUpperDot(CString key_string)
{
	return (key_string.Find(_T("`")) >= 0);
}

bool CKeyMapper::IsSquareBracketStart(CString key_string)
{
	return (key_string.Find(_T("[")) >= 0);
}

bool CKeyMapper::IsSquareBracketEnd(CString key_string)
{
	return (key_string.Find(_T("]")) >= 0);
}

bool CKeyMapper::IsQuotationMarks(CString key_string)
{
	return (key_string.Find(_T("\'")) >= 0);
}

bool CKeyMapper::IsDollar(CString key_string)
{
	return (key_string.Find(_T("\\")) >= 0);
}

bool CKeyMapper::Is0(CString key_string)
{
	return (key_string.Find(_T("0")) >= 0);
}

bool CKeyMapper::Is1(CString key_string)
{
	return (key_string.Find(_T("1")) >= 0);
}

bool CKeyMapper::Is2(CString key_string)
{
	return (key_string.Find(_T("2")) >= 0);
}

bool CKeyMapper::Is3(CString key_string)
{
	return (key_string.Find(_T("3")) >= 0);
}

bool CKeyMapper::Is4(CString key_string)
{
	return (key_string.Find(_T("4")) >= 0);
}

bool CKeyMapper::Is5(CString key_string)
{
	return (key_string.Find(_T("5")) >= 0);
}

bool CKeyMapper::Is6(CString key_string)
{
	return (key_string.Find(_T("6")) >= 0);
}

bool CKeyMapper::Is7(CString key_string)
{
	return (key_string.Find(_T("7")) >= 0);
}

bool CKeyMapper::Is8(CString key_string)
{
	return (key_string.Find(_T("8")) >= 0);
}

bool CKeyMapper::Is9(CString key_string)
{
	return (key_string.Find(_T("9")) >= 0);
}

bool CKeyMapper::IsA(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("A")) >= 0);
}

bool CKeyMapper::IsB(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("B")) >= 0);
}

bool CKeyMapper::IsC(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("C")) >= 0);
}

bool CKeyMapper::IsD(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("D")) >= 0);
}

bool CKeyMapper::IsE(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("E")) >= 0);
}

bool CKeyMapper::IsF(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("F")) >= 0);
}

bool CKeyMapper::IsG(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("G")) >= 0);
}

bool CKeyMapper::IsH(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("H")) >= 0);
}

bool CKeyMapper::IsI(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("I")) >= 0);
}

bool CKeyMapper::IsJ(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("J")) >= 0);
}

bool CKeyMapper::IsK(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("K")) >= 0);
}

bool CKeyMapper::IsL(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("L")) >= 0);
}

bool CKeyMapper::IsM(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("M")) >= 0);
}

bool CKeyMapper::IsN(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("N")) >= 0);
}

bool CKeyMapper::IsO(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("O")) >= 0);
}

bool CKeyMapper::IsP(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("P")) >= 0);
}

bool CKeyMapper::IsQ(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("Q")) >= 0);
}

bool CKeyMapper::IsR(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("R")) >= 0);
}

bool CKeyMapper::IsS(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("S")) >= 0);
}

bool CKeyMapper::IsT(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("T")) >= 0);
}

bool CKeyMapper::IsU(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("U")) >= 0);
}

bool CKeyMapper::IsV(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("V")) >= 0);
}

bool CKeyMapper::IsW(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("W")) >= 0);
}

bool CKeyMapper::IsX(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("X")) >= 0);
}

bool CKeyMapper::IsY(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("Y")) >= 0);
}

bool CKeyMapper::IsZ(CString key_string)
{
	key_string.MakeUpper();
	return (key_string.Find(_T("Z")) >= 0);
}
