#include "pch.h"
#include "hook.h"
#include "constants.h"
#include "ztl/ztl.h"
#include "localize_data.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

// Config flags
static bool g_bSwitchChinese = true;
static int g_nImeType = 1;

// Helper: fill memory with a byte value
template <typename T>
void PatchFill(T pAddress, unsigned char uValue, size_t uSize) {
    void* pBuf = malloc(uSize);
    memset(pBuf, uValue, uSize);
    PatchMemory(TO_PVOID(pAddress), pBuf, uSize);
    free(pBuf);
}

// Helper: code cave = JMP + NOP fill
template <typename T, typename U>
void PatchCodeCave(T pAddress, U pDestination, size_t uSize) {
    PatchJmp(pAddress, pDestination);
    if (uSize > 5) {
        PatchFill(pAddress + 5, 0x90, uSize - 5);
    }
}

// ============ StringPool::GetString Hook ============
typedef ZXString<char>* (__fastcall* StringPool_GetString_t)(void* pThis, void* edx, ZXString<char>* result, unsigned int nIdx, char formal);
inline auto StringPool_GetString = reinterpret_cast<StringPool_GetString_t>(0x0079E993);

ZXString<char>* __fastcall StringPool_GetString_hook(void* pThis, void* edx, ZXString<char>* result, unsigned int nIdx, char formal) {
    auto ret = StringPool_GetString(pThis, edx, result, nIdx, formal);
    if (g_bSwitchChinese) {
        for (const auto& pair : newKeyValuePairs) {
            if (nIdx == pair.key) {
                *ret = pair.value.c_str();
                break;
            }
        }
    }
    return ret;
}

// ============ FixItemType Codecaves ============
DWORD getItemType2Addr = 0x005CFAC2;
__declspec(naked) void getItemType1() {
    __asm {
        jmp getItemType2Addr
    }
}

DWORD getItemType2ErrRtnAddr = 0x005CFAA8;
DWORD getItemType2RtnAddr = 0x005CFADD;
__declspec(naked) void getItemType2() {
    __asm {
        dec eax
        jz label_eqp
        dec eax
        jz label_use
        dec eax
        jz label_ins
        dec eax
        jz label_etc
        dec eax
        jz label_cash
        jmp getItemType2ErrRtnAddr
    label_cash:
        push 0x159C
        jmp getItemType2RtnAddr
    label_etc:
        push 0x6DD
        jmp getItemType2RtnAddr
    label_ins:
        push 0x0B
        jmp getItemType2RtnAddr
    label_use:
        push 0x6E3
        jmp getItemType2RtnAddr
    label_eqp:
        push 0x6D9
        jmp getItemType2RtnAddr
    }
}

// ============ FixDateFormat Codecaves ============
DWORD fixDateFormatRtnAddr = 0x008EBF65;
__declspec(naked) void fixDateFormat() {
    __asm {
        movzx   ecx, word ptr[ebp - 16h]
        push    ecx
        movzx   ecx, word ptr[ebp - 1Ah]
        push    ecx
        movzx   ecx, word ptr[ebp - 1Ch]
        jmp fixDateFormatRtnAddr
    }
}

DWORD fixDateFormat2RtnAddr = 0x008EBFAF;
__declspec(naked) void fixDateFormat2() {
    __asm {
        movzx   ecx, word ptr[ebp - 16h]
        push    ecx
        movzx   ecx, word ptr[ebp - 1Ah]
        push    ecx
        movzx   ecx, word ptr[ebp - 1Ch]
        jmp fixDateFormat2RtnAddr
    }
}

DWORD fixDateFormat3RtnAddr = 0x008EC328;
__declspec(naked) void fixDateFormat3() {
    __asm {
        movzx   ecx, word ptr[ebp - 1Eh]
        push    ecx
        movzx   ecx, word ptr[ebp - 22h]
        push    ecx
        movzx   ecx, word ptr[ebp - 24h]
        jmp fixDateFormat3RtnAddr
    }
}

DWORD fixDateFormat4RtnAddr = 0x008EBF13;
__declspec(naked) void fixDateFormat4() {
    __asm {
        movzx   ecx, word ptr[ebp - 16h]
        push    ecx
        movzx   ecx, word ptr[ebp - 1Ah]
        push    ecx
        movzx   ecx, word ptr[ebp - 1Ch]
        jmp fixDateFormat4RtnAddr
    }
}

// ============ FixIme ============
void EnableIme() {
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        HIMC hImc = ImmGetContext(hwnd);
        if (hImc) {
            ImmAssociateContext(hwnd, hImc);
            ImmReleaseContext(hwnd, hImc);
        }
    }
}

void DisableIme() {
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        HIMC hImc = ImmGetContext(hwnd);
        if (hImc) {
            ImmAssociateContext(hwnd, NULL);
            ImmReleaseContext(hwnd, hImc);
        }
    }
}

static BYTE g_imeEnabled = 1;
DWORD funcEnableImeAddr = 0x009E85F3;

DWORD setOnFocusFirstJudgementRtnAddr = 0x004CA061;
DWORD switchImeAddr = 0x004CA078;
__declspec(naked) void setOnFocusFirstJudgement() {
    __asm {
        cmp [esp + 0Ch], edi
        jz label_jmp_switch_ime
        jmp setOnFocusFirstJudgementRtnAddr

    label_jmp_switch_ime:
        jmp switchImeAddr
    }
}

DWORD enableRtnAddr = 0x004CA08F;
DWORD disableRtnAddr = 0x004CA091;
__declspec(naked) void switchIme() {
    __asm {
        cmp [esp + 0Ch], edi
        jz  label_jz
        xor eax, eax
        cmp [esi + 0x80], eax
        setz al
        push eax
        call funcEnableImeAddr
        mov g_imeEnabled, 1
        jmp  enableRtnAddr

    label_jz:
        push 0
        call funcEnableImeAddr
        jmp  disableRtnAddr
    }
}

DWORD enableMLRtnAddr = 0x004D32E0;
DWORD disableMLRtnAddr = 0x004D32E2;
__declspec(naked) void switchMLIme() {
    __asm {
        cmp  dword ptr[esp + 8], 0
        jz   label_jz
        push 1
        call funcEnableImeAddr
        mov g_imeEnabled, 1
        jmp  enableMLRtnAddr

    label_jz:
        push 0
        call funcEnableImeAddr
        jmp  disableMLRtnAddr
    }
}

DWORD newSwitchImeRtnAddr = 0x004CA08F;
__declspec(naked) void newSwitchIme() {
    __asm {
        cmp [esi + 0x80], 1
        jz label_disable
        push 1
        call funcEnableImeAddr
        mov g_imeEnabled, 1
        jmp newSwitchImeRtnAddr

    label_disable:
        call DisableIme
        jmp newSwitchImeRtnAddr
    }
}

DWORD destroyWindowRtnAddr = 0x004DFEAD;
DWORD destroyWindowFuncAddr = 0x0041FE69;
__declspec(naked) void destroyWindow() {
    __asm {
        call destroyWindowFuncAddr
        or dword ptr[esi + 14h], 0FFFFFFFFh

        cmp g_imeEnabled, 0
        jz label_return

        call DisableIme
        mov g_imeEnabled, 0

    label_return:
        jmp destroyWindowRtnAddr
    }
}

DWORD newSwitchMLImeRtnAddr = 0x004D32EE;
__declspec(naked) void newSwitchMLIme() {
    __asm {
        push 1
        call funcEnableImeAddr
        mov g_imeEnabled, 1
        jmp  newSwitchMLImeRtnAddr
    }
}

static void ImeGeneralHook() {
    PatchFill(0x008D54A6, 0x90, 9);  // Key
    PatchFill(0x00937225, 0x90, 9);  // Chat
    PatchFill(0x00531EE8, 0x90, 9);  // Group Message
    PatchFill(0x004CAE7D, 0x90, 2);  // Support Chinese input
    Patch1(0x004CAE8F, 0xEB);
    PatchFill(0x007A015D, 0x90, 2);  // Character name filter
}

static void ImeHookOld() {
    ImeGeneralHook();
    PatchCodeCave(0x004CA05B, setOnFocusFirstJudgement, 6);
    PatchCodeCave(0x004CA089, switchIme, 6);
    PatchFill(0x004D32C6, 0x90, 2);
    PatchCodeCave(0x004D32D9, switchMLIme, 7);
    PatchCodeCave(0x004DFEA4, destroyWindow, 9);
}

static void ImeHookNew() {
    ImeGeneralHook();
    PatchCodeCave(0x004CA089, newSwitchIme, 6);
    PatchCodeCave(0x004DFEA4, destroyWindow, 9);
    PatchCodeCave(0x004D32D9, newSwitchMLIme, 7);
}

// ============ Direct Memory Writes ============
static void ApplyDirectMemoryWrites() {
    // Gender labels: female / male
    PatchStr(0x00AF6D1C, "  女  ");
    PatchStr(0x00AF6D24, " 男 ");
    // Channel selection text
    PatchStr(0x00AF2B28, "选择频道     ");
    // Font size adjustments for Chinese
    Patch1(0x008E55ED + 1, 0x0B);
    Patch1(0x008E557A + 1, 0x0B);
    Patch1(0x008E565E + 1, 0x0B);
    // Character card job name size and position
    Patch1(0x0090142E + 1, 0x5E);
    Patch1(0x00901400 + 1, 1);
}

// ============ FixItemType ============
static void ApplyFixItemType() {
    PatchCodeCave(0x005CFA99, getItemType1, 15);
    PatchCodeCave(0x005CFAC2, getItemType2, 27);
}

// ============ FixDateFormat ============
static void ApplyFixDateFormat() {
    PatchCodeCave(0x008EBF57, fixDateFormat, 14);
    PatchCodeCave(0x008EBFA1, fixDateFormat2, 14);
    PatchCodeCave(0x008EC31A, fixDateFormat3, 14);
    PatchCodeCave(0x008EBF05, fixDateFormat4, 14);
}

// ============ FixEquipJobNamePos ============
// Fix class name X-offsets in equipment tooltip for Chinese character alignment
static void ApplyFixEquipJobNamePos() {
    Patch1(0x008EC4A7 + 1, 0x23);  // Warrior
    Patch1(0x008EC53C + 1, 0x4D);  // Magician
    Patch1(0x008EC5D1 + 1, 0x7A);  // Bowman
    Patch1(0x008EC660 + 1, 0xA9);  // Thief
    Patch1(0x008EC6CF + 1, 0xC8);  // Pirate
}

// ============ Main Attach Function ============
void AttachLocalizeMod() {
    // Read config
    char sBuffer[16];
    if (GetPrivateProfileStringA("config", "SwitchChinese", "1", sBuffer, sizeof(sBuffer), ".\\" CONSTANTS_CONFIG_NAME) > 0) {
        g_bSwitchChinese = atoi(sBuffer) != 0;
    }
    if (GetPrivateProfileStringA("config", "imeType", "1", sBuffer, sizeof(sBuffer), ".\\" CONSTANTS_CONFIG_NAME) > 0) {
        g_nImeType = atoi(sBuffer);
    }

    if (g_bSwitchChinese) {
        ATTACH_HOOK(StringPool_GetString, StringPool_GetString_hook);
        ApplyDirectMemoryWrites();
        ApplyFixItemType();
        ApplyFixDateFormat();
        ApplyFixEquipJobNamePos();
    }

    // IME support
    if (g_nImeType == 0) {
        ImeHookOld();
    } else {
        ImeHookNew();
    }
}
