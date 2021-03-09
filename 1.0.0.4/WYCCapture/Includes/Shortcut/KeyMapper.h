#pragma once

enum
{
	VK_0 = 0x30,
	VK_1,
	VK_2,
	VK_3,
	VK_4,
	VK_5,
	VK_6,
	VK_7,
	VK_8,
	VK_9,
	VK_A = 0x41,
	VK_B,
	VK_C,
	VK_D,
	VK_E,
	VK_F,
	VK_G,
	VK_H,
	VK_I,
	VK_J,
	VK_K,
	VK_L,
	VK_M,
	VK_N,
	VK_O,
	VK_P,
	VK_Q,
	VK_R,
	VK_S,
	VK_T,
	VK_U,
	VK_V,
	VK_W,
	VK_X,
	VK_Y,
	VK_Z,
};

class CKeyMapper
{
public:
	static CString MakeKeyString(bool ctrl, bool alt, bool shift, BYTE key);
	static bool ParseKeyString(CString key_string, bool* ctrl, bool* alt, bool* shift, BYTE* key);

	static CString KeyToString(BYTE key);
	static BYTE StringToKey(CString key_string);

	static bool CheckCtrl(CString key_string);
	static bool CheckAlt(CString key_string);
	static bool CheckShift(CString key_string);

	static bool IsEsc(CString key_string);
	static bool IsTab(CString key_string);
	static bool IsReturn(CString key_string);
	static bool IsCapsLock(CString key_string);
	static bool IsSpace(CString key_string);
	static bool IsPageUp(CString key_string);
	static bool IsPageDown(CString key_string);
	static bool IsEnd(CString key_string);
	static bool IsHome(CString key_string);
	static bool IsLeft(CString key_string);
	static bool IsRight(CString key_string);
	static bool IsUp(CString key_string);
	static bool IsDown(CString key_string);
	static bool IsPrintScreen(CString key_string);
	static bool IsInsert(CString key_string);
	static bool IsDelete(CString key_string);
	static bool IsNumpad0(CString key_string);
	static bool IsNumpad1(CString key_string);
	static bool IsNumpad2(CString key_string);
	static bool IsNumpad3(CString key_string);
	static bool IsNumpad4(CString key_string);
	static bool IsNumpad5(CString key_string);
	static bool IsNumpad6(CString key_string);
	static bool IsNumpad7(CString key_string);
	static bool IsNumpad8(CString key_string);
	static bool IsNumpad9(CString key_string);
	static bool IsMultiply(CString key_string);
	static bool IsAdd(CString key_string);
	static bool IsSubtract(CString key_string);
	static bool IsDot(CString key_string);
	static bool IsDivide(CString key_string);
	static bool IsF1(CString key_string);
	static bool IsF2(CString key_string);
	static bool IsF3(CString key_string);
	static bool IsF4(CString key_string);
	static bool IsF5(CString key_string);
	static bool IsF6(CString key_string);
	static bool IsF7(CString key_string);
	static bool IsF8(CString key_string);
	static bool IsF9(CString key_string);
	static bool IsF10(CString key_string);
	static bool IsF11(CString key_string);
	static bool IsF12(CString key_string);
	static bool IsNumLock(CString key_string);
	static bool IsScrollLock(CString key_string);
	static bool IsSemicolon(CString key_string);
	static bool IsEqual(CString key_string);
	static bool IsComma(CString key_string);
	static bool IsUpperDot(CString key_string);
	static bool IsSquareBracketStart(CString key_string);
	static bool IsSquareBracketEnd(CString key_string);
	static bool IsQuotationMarks(CString key_string);
	static bool IsDollar(CString key_string);
	static bool Is0(CString key_string);
	static bool Is1(CString key_string);
	static bool Is2(CString key_string);
	static bool Is3(CString key_string);
	static bool Is4(CString key_string);
	static bool Is5(CString key_string);
	static bool Is6(CString key_string);
	static bool Is7(CString key_string);
	static bool Is8(CString key_string);
	static bool Is9(CString key_string);
	static bool IsA(CString key_string);
	static bool IsB(CString key_string);
	static bool IsC(CString key_string);
	static bool IsD(CString key_string);
	static bool IsE(CString key_string);
	static bool IsF(CString key_string);
	static bool IsG(CString key_string);
	static bool IsH(CString key_string);
	static bool IsI(CString key_string);
	static bool IsJ(CString key_string);
	static bool IsK(CString key_string);
	static bool IsL(CString key_string);
	static bool IsM(CString key_string);
	static bool IsN(CString key_string);
	static bool IsO(CString key_string);
	static bool IsP(CString key_string);
	static bool IsQ(CString key_string);
	static bool IsR(CString key_string);
	static bool IsS(CString key_string);
	static bool IsT(CString key_string);
	static bool IsU(CString key_string);
	static bool IsV(CString key_string);
	static bool IsW(CString key_string);
	static bool IsX(CString key_string);
	static bool IsY(CString key_string);
	static bool IsZ(CString key_string);
};