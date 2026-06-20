#include "pch.h"
#include "hook.h"

int get_screen_width();

unsigned char Array_aDefaultQKM[] = {
	42, 0, 0, 0,   // col0 top:  Shift
	29, 0, 0, 0,   // col0 bot:  Ctrl
	82, 0, 0, 0,   // col1 top:  Insert
	83, 0, 0, 0,   // col1 bot:  Delete
	71, 0, 0, 0,   // col2 top:  Home
	79, 0, 0, 0,   // col2 bot:  End
	73, 0, 0, 0,   // col3 top:  PgUp
	81, 0, 0, 0,   // col3 bot:  PgDn
	2,  0, 0, 0,   // col4 top:  1
	16, 0, 0, 0,   // col4 bot:  Q
	3,  0, 0, 0,   // col5 top:  2
	17, 0, 0, 0,   // col5 bot:  W
	4,  0, 0, 0,   // col6 top:  3
	18, 0, 0, 0,   // col6 bot:  E
	5,  0, 0, 0,   // col7 top:  4
	19, 0, 0, 0,   // col7 bot:  R
	6,  0, 0, 0,   // col8 top:  5
	20, 0, 0, 0,   // col8 bot:  T
	30, 0, 0, 0,   // col9 top:  A
	21, 0, 0, 0,   // col9 bot:  Y
	31, 0, 0, 0,   // col10 top: S
	44, 0, 0, 0,   // col10 bot: Z
	32, 0, 0, 0,   // col11 top: D
	45, 0, 0, 0,   // col11 bot: X
	33, 0, 0, 0,   // col12 top: F
	46, 0, 0, 0,   // col12 bot: C
	34, 0, 0, 0,   // col13 top: G
	47, 0, 0, 0,   // col13 bot: V
	35, 0, 0, 0,   // col14 top: H
	48, 0, 0, 0    // col14 bot: B
};

unsigned char Array_ptShortKeyPos[] = {
	7,   0, 0, 0,   8,  0, 0, 0,
	7,   0, 0, 0,   41, 0, 0, 0,
	42,  0, 0, 0,   8,  0, 0, 0,
	42,  0, 0, 0,   41, 0, 0, 0,
	77,  0, 0, 0,   8,  0, 0, 0,
	77,  0, 0, 0,   41, 0, 0, 0,
	112, 0, 0, 0,   8,  0, 0, 0,
	112, 0, 0, 0,   41, 0, 0, 0,
	147, 0, 0, 0,   8,  0, 0, 0,
	147, 0, 0, 0,   41, 0, 0, 0,
	182, 0, 0, 0,   8,  0, 0, 0,
	182, 0, 0, 0,   41, 0, 0, 0,
	217, 0, 0, 0,   8,  0, 0, 0,
	217, 0, 0, 0,   41, 0, 0, 0,
	252, 0, 0, 0,   8,  0, 0, 0,
	252, 0, 0, 0,   41, 0, 0, 0,
	31,  1, 0, 0,   8,  0, 0, 0,
	31,  1, 0, 0,   41, 0, 0, 0,
	66,  1, 0, 0,   8,  0, 0, 0,
	66,  1, 0, 0,   41, 0, 0, 0,
	101, 1, 0, 0,   8,  0, 0, 0,
	101, 1, 0, 0,   41, 0, 0, 0,
	136, 1, 0, 0,   8,  0, 0, 0,
	136, 1, 0, 0,   41, 0, 0, 0,
	171, 1, 0, 0,   8,  0, 0, 0,
	171, 1, 0, 0,   41, 0, 0, 0,
	206, 1, 0, 0,   8,  0, 0, 0,
	206, 1, 0, 0,   41, 0, 0, 0,
	241, 1, 0, 0,   8,  0, 0, 0,
	241, 1, 0, 0,   41, 0, 0, 0
};

unsigned char Array_aDefaultQKM_0[] = {
	42, 0, 0, 0,   // col0 top:  Shift
	29, 0, 0, 0,   // col0 bot:  Ctrl
	82, 0, 0, 0,   // col1 top:  Insert
	83, 0, 0, 0,   // col1 bot:  Delete
	71, 0, 0, 0,   // col2 top:  Home
	79, 0, 0, 0,   // col2 bot:  End
	73, 0, 0, 0,   // col3 top:  PgUp
	81, 0, 0, 0,   // col3 bot:  PgDn
	2,  0, 0, 0,   // col4 top:  1
	16, 0, 0, 0,   // col4 bot:  Q
	3,  0, 0, 0,   // col5 top:  2
	17, 0, 0, 0,   // col5 bot:  W
	4,  0, 0, 0,   // col6 top:  3
	18, 0, 0, 0,   // col6 bot:  E
	5,  0, 0, 0,   // col7 top:  4
	19, 0, 0, 0,   // col7 bot:  R
	6,  0, 0, 0,   // col8 top:  5
	20, 0, 0, 0,   // col8 bot:  T
	30, 0, 0, 0,   // col9 top:  A
	21, 0, 0, 0,   // col9 bot:  Y
	31, 0, 0, 0,   // col10 top: S
	44, 0, 0, 0,   // col10 bot: Z
	32, 0, 0, 0,   // col11 top: D
	45, 0, 0, 0,   // col11 bot: X
	33, 0, 0, 0,   // col12 top: F
	46, 0, 0, 0,   // col12 bot: C
	34, 0, 0, 0,   // col13 top: G
	47, 0, 0, 0,   // col13 bot: V
	35, 0, 0, 0,   // col14 top: H
	48, 0, 0, 0    // col14 bot: B
};

unsigned char Array_Expanded[360] = {
	4, 4,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 0,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 1,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 2,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 3,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 5,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 6,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 7,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 8,  0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 10, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 11, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 12, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 13, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 14, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 15, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 16, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 17, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 23, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 24, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 25, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 26, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	4, 27, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	5, 50, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	5, 51, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	5, 52, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	5, 53, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	5, 54, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	5, 55, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	5, 56, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
	5, 57, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0
};

unsigned char cooldown_Array[124] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
};

DWORD Array_aDefaultQKM_Address = (DWORD)&Array_aDefaultQKM;
DWORD Array_Expanded_Address    = (DWORD)&Array_Expanded;

DWORD CompareValidate_Retn      = 0x008DD8BD;
DWORD sub_9FA0CB_cave_retn_1    = 0x009FA0E1;

__declspec(naked) void CompareValidateFuncKeyMappedInfo_cave() {
	_asm {
		push 0x168
		push 0x0
		push eax
		pushad
		popad
		jmp CompareValidate_Retn
	}
}

__declspec(naked) void sub_9FA0CB_cave() {
	_asm {
		test eax, eax
		jne  label
		push 0xF4
		pushad
		popad
		jmp  sub_9FA0CB_cave_retn_1
	label:
		push 0x168
		push 0x0
		push eax
		pushad
		popad
		jmp  CompareValidate_Retn
	}
}

__declspec(naked) void sDefaultQuickslotKeyMap_cave() {
	_asm {
		push ebx
		push esi
		push edi
		xor  edx, edx
		mov  ebx, ecx
		call label
		nop
		lea  edi, dword ptr ds:[ebx + 0x4]
		mov  ecx, 0x1E
		mov  esi, Array_aDefaultQKM_Address
		rep  movsd
		lea  edi, dword ptr ds:[ebx + 0x7C]
		mov  ecx, 0x1E
		mov  esi, Array_aDefaultQKM_Address
		rep  movsd
		pop  edi
		pop  esi
		pop  ebx
		ret
	label:
		push esi
		mov  esi, ecx
		lea  eax, dword ptr ds:[esi + 0x4]
		push 0x0072B7C2
		ret
	}
}

__declspec(naked) void DefaultQuickslotKeyMap_cave() {
	_asm {
		push esi
		push edi
		lea  eax, dword ptr ds:[ecx + 0x4]
		mov  esi, Array_aDefaultQKM_Address
		mov  ecx, 0x1E
		mov  edi, eax
		rep  movsd
		pop  edi
		pop  esi
		ret
	}
}

__declspec(naked) void Restore_Array_Expanded_cave() {
	_asm {
		push edi
		push ecx
		xor  eax, eax
		mov  edi, Array_Expanded_Address
		mov  ecx, 0x5A
		rep  stosd
		pop  ecx
		pop  edi
		lea  eax, [esi + 0xD7C]
		push 0x008CFE03
		ret
	}
}

#define SP_QUICKSLOT_INDEX 2504

static const char** g_ppStringPool = reinterpret_cast<const char**>(0x00BDC9D4);

typedef void* (__cdecl* StringPool_GetInstance_t)();
static StringPool_GetInstance_t StringPool_GetInstance = reinterpret_cast<StringPool_GetInstance_t>(0x0079E805);

typedef int (__thiscall* Layer_GetDim_t)(void* pLayer);
static Layer_GetDim_t Layer_GetWidth  = reinterpret_cast<Layer_GetDim_t>(0x00440C00);
static Layer_GetDim_t Layer_GetHeight = reinterpret_cast<Layer_GetDim_t>(0x00440C2A);

typedef int (__stdcall* Canvas_Clear_t)(void* pCanvas, int x, int y, int w, int h, int color);

static char* g_pQuickSlotS_encoded   = nullptr;
static char* g_pQuickSlotM_encoded   = nullptr;
static const char* g_pQuickSlot_stock = nullptr;
static int g_nLastQuickSlotProfile   = -1;

void __cdecl RefreshQuickSlotArt() {
	int nWidth = get_screen_width();
	int nProfile = (nWidth == 800) ? 0 : (nWidth == 1024) ? 1 : 2;
	if (nProfile == g_nLastQuickSlotProfile) {
		return;
	}
	g_nLastQuickSlotProfile = nProfile;

	const char* pFrame;
	int nSlots;
	switch (nProfile) {
	case 0:  pFrame = g_pQuickSlotS_encoded; nSlots = 8;  break;
	case 1:  pFrame = g_pQuickSlotM_encoded; nSlots = 12; break;
	default: pFrame = g_pQuickSlot_stock;    nSlots = 30; break;
	}

	g_ppStringPool[SP_QUICKSLOT_INDEX] = pFrame;

	int* pInstance = static_cast<int*>(StringPool_GetInstance());
	if (pInstance && pInstance[0]) {
		reinterpret_cast<int*>(pInstance[0])[SP_QUICKSLOT_INDEX] = 0;
	}

	Patch1(0x008DE75E + 3, static_cast<unsigned char>(4 + nSlots * 4));
	Patch1(0x008E099F + 3, static_cast<unsigned char>(nSlots));
}

void __cdecl ClearQuickSlotFrame(void* pThis, void* pFrameCanvas) {
	if (!pThis || !pFrameCanvas) {
		return;
	}
	void* pLayer = *reinterpret_cast<void**>(reinterpret_cast<char*>(pThis) + 0xCC4);
	if (!pLayer) {
		return;
	}
	int w = Layer_GetWidth(pLayer);
	int h = Layer_GetHeight(pLayer);
	void** vtbl = *reinterpret_cast<void***>(pFrameCanvas);
	reinterpret_cast<Canvas_Clear_t>(vtbl[0x8C / 4])(pFrameCanvas, 0, 0, w, h, 0xFFFFFF);
}

DWORD QuickSlotArt_Retn = 0x008DDE79;

__declspec(naked) void QuickSlotArt_cave() {
	_asm {
		pushad
		call RefreshQuickSlotArt
		mov  eax, [ebp - 0x18]
		push eax
		mov  eax, [ebp - 0x3C]
		push eax
		call ClearQuickSlotFrame
		add  esp, 8
		popad
		push 0x9C8
		jmp  QuickSlotArt_Retn
	}
}

void AttachQuickSlotsMod() {
	Patch1(0x008D155C + 1, 0xF0);
	Patch1(0x008D155C + 2, 0x03);
	Patch1(0x008D182E + 1, 0xF0);
	Patch1(0x008D182E + 2, 0x03);
	Patch1(0x008D1AC0 + 1, 0xF0);
	Patch1(0x008D1AC0 + 2, 0x03);

	Patch4(0x0072B7CE + 1, (DWORD)&Array_aDefaultQKM_0);
	Patch4(0x0072B8EB + 1, (DWORD)&Array_aDefaultQKM_0);

	Patch1(0x008DD916, 0x1E);
	Patch1(0x008DD8AD, 0x1E);
	Patch1(0x008DD8FD, 0xBB);
	Patch4(0x008DD8FD + 1, (DWORD)&Array_Expanded);
	Patch1(0x008DD8FD + 5, 0x90);
	Patch1(0x008DD898, 0xB8);
	Patch4(0x008DD898 + 1, (DWORD)&Array_Expanded);
	Patch1(0x008DD898 + 5, 0x90);

	Patch1(0x008DE75E + 3, 0x7C);
	Patch1(0x008DDF99, 0xB8);
	Patch4(0x008DDF99 + 1, (DWORD)&Array_Expanded);
	PatchNop(0x008DDF99 + 5, 0x008DDF99 + 8);

	Patch1(0x008D7F1E + 1, 0x34);
	Patch1(0x008D7F1E + 2, 0x85);
	Patch4(0x008D7F1E + 3, (DWORD)&Array_Expanded);

	Patch4(0x008DE94D + 2, (DWORD)&Array_ptShortKeyPos);
	Patch4(0x008DE955 + 2, (DWORD)&Array_ptShortKeyPos + 4);
	Patch1(0x008DE941 + 2, 0x1E);

	Patch4(0x008DE8F4 + 1, (DWORD)&Array_ptShortKeyPos + 4);
	Patch4(0x008DE926 + 2, (DWORD)&Array_ptShortKeyPos + 4 + 30 * 8);

	Patch1(0x008E099F + 3, 0x1E);
	Patch1(0x008E069D, 0xBE);
	Patch4(0x008E069D + 1, (DWORD)&cooldown_Array);
	Patch1(0x008E069D + 5, 0x90);
	Patch1(0x008E06A3, 0xBF);
	Patch4(0x008E06A3 + 1, (DWORD)&Array_Expanded + 1);
	Patch1(0x008E06A3 + 5, 0x90);

	Patch1(0x004F928A + 2, 0x1E);
	Patch1(0x004F93F9 + 2, 0x1E);

	Patch1(0x00833797 + 2, 0x7C);
	Patch1(0x00833841 + 2, 0x7C);
	Patch1(0x00833791 + 1, 0x78);
	Patch1(0x0083383B + 1, 0x78);

	Patch1(0x0083287F + 2, 0x7C);
	Patch1(0x00832882 + 1, 0x78);

	Patch1(0x0072B8C0 + 2, 0x7C);
	Patch1(0x0072B8A0 + 1, 0x78);
	Patch1(0x0072B8BD + 1, 0x78);

	Patch1(0x0072B83E + 1, 0x78);
	Patch1(0x0072B861 + 1, 0x78);
	Patch1(0x0072B867 + 2, 0x7C);

	Patch1(0x00836A1E + 1, 0x78);
	Patch1(0x00836A21 + 2, 0x7C);

	g_pQuickSlotS_encoded = EncodeStringAlloc("UI/StatusBar.img/base/quickSlotS");
	g_pQuickSlotM_encoded = EncodeStringAlloc("UI/StatusBar.img/base/quickSlotM");
	g_pQuickSlot_stock    = g_ppStringPool[SP_QUICKSLOT_INDEX];

	PatchJmp(0x008DDE74, &QuickSlotArt_cave);
	PatchJmp(0x008DD8B8, &CompareValidateFuncKeyMappedInfo_cave);
	PatchJmp(0x009FA0DB, &sub_9FA0CB_cave);
	PatchJmp(0x0072B7BC, &sDefaultQuickslotKeyMap_cave);
	PatchJmp(0x0072B8E6, &DefaultQuickslotKeyMap_cave);
	PatchJmp(0x008CFDFD, &Restore_Array_Expanded_cave);
	PatchNop(0x008CFDFD + 5, 0x008CFDFD + 6);
}
