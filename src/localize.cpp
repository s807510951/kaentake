#include "pch.h"
#include "hook.h"
#include "constants.h"
#include "ztl/ztl.h"
#include "localize_data.h"
#include "wvs/util.h"
#include <imm.h>
#include <cmath>
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

// ============ FixHighVersionFaceHair ============
// Support high-version face/hair types (2-6)
DWORD faceRtn = 0x005C95BF;
DWORD hairRtn = 0x005C958D;
DWORD faceHairCaveRtn = 0x005C9505;

__declspec(naked) void faceHairCave() {
    __asm {
        cmp eax, 2
        jz label_face
        cmp eax, 3
        jz label_hair
        cmp eax, 4
        jz label_hair
        cmp eax, 5
        jz label_face
        cmp eax, 6
        jz label_hair
        jmp faceHairCaveRtn
    label_face:
        jmp faceRtn
    label_hair:
        jmp hairRtn
    }
}

DWORD faceHairCave2Rtn = 0x009ACAA0;
DWORD faceHairCave2Jmp = 0x009ACAA4;

__declspec(naked) void faceHairCave2() {
    __asm {
        cmp eax, 2
        jz label
        cmp eax, 5
        jz label
        jmp faceHairCave2Jmp
    label:
        jmp faceHairCave2Rtn
    }
}

DWORD faceHairCave3Rtn = 0x009ACAAC;

__declspec(naked) void faceHairCave3() {
    __asm {
        cmp eax, 3
        jz label
        cmp eax, 4
        jz label
        cmp eax, 6
    label:
        setnz cl
        jmp faceHairCave3Rtn
    }
}

static void ApplyFixHighVersionFaceHair() {
    PatchCodeCave(0x005C94F3, faceHairCave, 18);   // Remove display restriction
    PatchCodeCave(0x009ACA9B, faceHairCave2, 5);    // Remove NPC dialogue restriction
    PatchCodeCave(0x009ACAA6, faceHairCave3, 6);    // Remove NPC dialogue restriction
}

// ============ 脚本消息框放大 ============
// 放大NPC脚本对话框宽度以适配中文字符显示
static void ApplyFixScriptDlgSize() {
    Patch1(0x0068DE1F + 1, 0x86);  // 对话框宽度1
    Patch1(0x0068DFBD + 1, 0x86);  // 对话框宽度2
    Patch1(0x0068E0E7 + 1, 0x86);  // 对话框宽度3
    Patch1(0x0068E534 + 1, 0x86);  // 对话框宽度4
    Patch1(0x0068E65D + 1, 0x86);  // 对话框宽度5
    Patch1(0x0068E709 + 1, 0x86);  // 对话框宽度6
    Patch4(0x009A3D81, 480);       // UI元素宽度
}

// ============ 1. 伤害/属性上限调整 ============
static void ApplyDamageCap() {
    // 物理攻击PAD上限去除限制（默认1999 → 2147483646）
    Patch4(0x0077E055 + 1, 2147483646);
    Patch4(0x0077E12F + 1, 2147483646);
    // 伤害上限调整（默认199999 → 1999999）
    Patch4(0x008C3304 + 1, 1999999);
    // 魔攻上限调整（默认1999 → 1999）
    Patch4(0x0077E215 + 1, 1999);
    Patch4(0x00780620 + 1, 1999);
    // 命中上限调整（默认999）
    Patch4(0x007806D0 + 1, 999);
    // 回避上限调整（默认999）
    Patch4(0x00780702 + 1, 999);
    // 反伤PDamage去除限制（默认1999 → 2147483646）
    Patch4(0x0078FF5F + 1, 2147483646);
    Patch4(0x0078E061 + 1, 2147483646);
    Patch4(0x0078E67D + 1, 2147483646);
    // 魔攻MDamage去除限制（默认1999 → 2147483646）
    Patch4(0x0079166C + 1, 2147483646);
    Patch4(0x00791CD5 + 1, 2147483646);
    Patch4(0x007918FC + 1, 2147483646);
    // 伤害显示上限（double 1999999）
    double atkOutCap = 1999999.0;
    PatchMemory(TO_PVOID(0x00AFE8A0), &atkOutCap, sizeof(double));
}

// ============ 4. 技能提示框中文字符宽度修复 ============
static int g_nCharLen = 55;

static void CalcCharLen(const char* word) {
    const std::string str = std::string(word);
    auto firstByte = static_cast<unsigned char>(str[0]);
    if (str.length() < 55) {
        g_nCharLen = 55;
        return;
    }
    for (int i = 0; i < 60; i++) {
        firstByte = static_cast<unsigned char>(str[i]);
        if (firstByte >= 0x81 && firstByte <= 0xFE) {
            i++; // 中文字符占双字节，跳过
            continue;
        }
        if (i >= 55) {
            g_nCharLen = i;
            break;
        }
    }
}

constexpr DWORD skillToolTipNewRtn = 0x008F3844;
__declspec(naked) void skillToolTipNew() {
    __asm {
        mov eax, [ebp + 0Ch]
        push eax
        call CalcCharLen
        pop eax
        mov eax, g_nCharLen
        mov[ebp - 1Ch], eax
        lea eax, [ebp - 30h]
        jmp skillToolTipNewRtn
    }
}

static void ApplyFixSkillToolTip() {
    PatchNop(0x008E4252, 0x008E4254);  // 修复技能边界检查
    PatchCodeCave(0x008F383E, skillToolTipNew, 6);  // 修复技能描述中文字符宽度计算
}

// ============ 8. 更多消息显示（MoreGainMsgs） ============
static int g_nMsgAmount = 26;  // 消息显示数量
static int g_nMoreGainMsgsOffset = 6;
static int g_nMoreGainMsgsFadeOffset = 0;
static int g_nMoreGainMsgsFade1Offset = 0;

DWORD dwMoreGainMsgsRetn = 0x0089B18B;
__declspec(naked) void ccMoreGainMsgs() {
    __asm {
        mov    eax, DWORD PTR[edi + 0x10]
        cmp    eax, g_nMoreGainMsgsOffset
        jmp    dwMoreGainMsgsRetn
    }
}

DWORD dwMoreGainMsgsFadeRetn = 0x0089B56A;
__declspec(naked) void ccMoreGainMsgsFade() {
    __asm {
        add    eax, g_nMoreGainMsgsFadeOffset
        push   3
        jmp    dwMoreGainMsgsFadeRetn
    }
}

DWORD dwMoreGainMsgsFade1Retn = 0x0089B4EB;
__declspec(naked) void ccMoreGainMsgsFade1() {
    __asm {
        push   g_nMoreGainMsgsFade1Offset
        jmp    dwMoreGainMsgsFade1Retn
    }
}

static void ApplyMoreGainMsgs() {
    int msgAmnt = g_nMsgAmount;
    int msgAmntOffset = msgAmnt * 14;

    Patch4(0x0089AEE2 + 3, msgAmnt);           // moregainmsgs part 1
    g_nMoreGainMsgsOffset = msgAmnt;            // param for ccmoregainmsgs
    PatchCodeCave(0x0089B185, ccMoreGainMsgs, 6);  // moregainmsgs part 2
    g_nMoreGainMsgsFadeOffset = 15000;          // param for ccmoregainmsgsFade
    PatchCodeCave(0x0089B563, ccMoreGainMsgsFade, 7);  // moregainmsgsFade
    g_nMoreGainMsgsFade1Offset = 255 * 4 / 3;   // param for ccmoregainmsgsFade1
    PatchCodeCave(0x0089B4E6, ccMoreGainMsgsFade1, 5);  // moregainmsgsFade1
}

// ============ 15. 聊天窗口位置修复 ============
const DWORD chatTextPosRtn = 0x008DD075;
__declspec(naked) void chatTextPos() {
    __asm {
        add eax, [edi + 0CFCh]
        cmp[edi + 0D00h], 3
        jz label_type3
        cmp[edi + 0D00h], 2
        jz label_type2

    label_type1:        // 状态1 正常
        sub eax, 1
        jmp label_rtn

    label_type2:        // 状态2 正常 + 展开
        jmp label_rtn

    label_type3:        // 状态3 扩展
        sub eax, 2

    label_rtn:
        jmp chatTextPosRtn
    }
}

static void ApplyFixChatPos() {
    PatchCodeCave(0x008DD06F, chatTextPos, 6);
}

// ============ 23. 高伤害显示时属性窗口加宽 ============
DWORD apDetailBtnRtn = 0x008C4E22;
__declspec(naked) void apDetailBtn() {
    __asm {
        push    144h
        push    99h
        jmp apDetailBtnRtn
    }
}

static void ApplyWideStatWindow() {
    // 当伤害显示上限 > 999999 时，加宽属性窗口各元素位置
    Patch4(0x008C485A + 1, 192);   // 属性关闭按钮x
    Patch4(0x008C4AB3 + 1, 210);   // 属性列
    Patch4(0x008C510A + 1, 218);   // 属性详细列
    Patch4(0x008C4EA2 + 1, 210);   // 属性详情起始x
    Patch4(0x008C5760 + 1, 210);   // 属性详情切换x
    Patch4(0x008C7AD9 + 1, 185);   // 属性详情按钮x
    Patch4(0x008C2754 + 1, 195);   // 属性详情关闭按钮x
    Patch4(0x008C6C72 + 1, 210);   // 移动时属性详情x
    PatchCodeCave(0x008C4E1B, apDetailBtn, 7);  // 详情按钮
}

// ============ 25. 通知弹窗位置修复 ============
static void ApplyNotifyPopupPos() {
    int nWidth = get_screen_width();
    int nHeight = get_screen_height();
    Patch4(0x0049D218 + 1, nWidth - 16);    // 通知弹窗位置边界 x
    Patch4(0x0049D268 + 1, nHeight - 16);   // 通知弹窗位置边界 y
}

// ============ 26. 截图功能高分辨率适配 ============
static void ApplyScreenshotFix() {
    int nWidth = get_screen_width();
    int nHeight = get_screen_height();
    Patch4(0x00744EB4 + 1, nWidth);                     // 截图宽度
    Patch4(0x00744EB9 + 1, nHeight);                    // 截图高度
    Patch4(0x00744E2A + 1, 3 * nWidth * nHeight);      // 截图缓冲区3x
    Patch4(0x00744E43 + 1, nWidth * nHeight);           // 截图缓冲区1x
    Patch4(0x00744DA6 + 1, 4 * nWidth * nHeight);      // 截图缓冲区4x
}

// ============ 27. 相机移动适配 ============
static void ApplyCameraMovementFix() {
    int nWidth = get_screen_width();
    int nHeight = get_screen_height();
    Patch4(0x00641F61 + 1, (unsigned int)floor(nWidth / 2.0));    // VRleft
    Patch4(0x00641FC8 + 1, (unsigned int)floor(nHeight / 2.0));   // VRTop
    Patch4(0x0064208F + 1, (unsigned int)floor(nHeight / 2.0));   // VRbottom
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

    ApplyFixHighVersionFaceHair();
    ApplyFixScriptDlgSize();

    // IME support
    if (g_nImeType == 0) {
        ImeHookOld();
    } else {
        ImeHookNew();
    }

    // 新增功能
    ApplyDamageCap();           // 1. 伤害/属性上限调整
    ApplyFixSkillToolTip();     // 4. 技能提示框中文字符宽度修复
    ApplyMoreGainMsgs();        // 8. 更多消息显示
    ApplyFixChatPos();          // 15. 聊天窗口位置修复
    ApplyWideStatWindow();      // 23. 高伤害显示时属性窗口加宽
    ApplyNotifyPopupPos();      // 25. 通知弹窗位置修复
    ApplyScreenshotFix();       // 26. 截图功能高分辨率适配
    ApplyCameraMovementFix();   // 27. 相机移动适配
}
