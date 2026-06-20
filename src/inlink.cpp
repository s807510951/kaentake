#include "pch.h"
#include "hook.h"
#include "wvs/util.h"
#include "ztl/ztl.h"
#include <cstdio>
#include <cstdarg>

static void InlinkLog(const char* sFormat, ...) {
    char msg[1024];
    va_list args;
    va_start(args, sFormat);
    _vsnprintf_s(msg, sizeof(msg), _TRUNCATE, sFormat, args);
    va_end(args);

    SYSTEMTIME st;
    GetLocalTime(&st);
    char line[1200];
    _snprintf_s(line, sizeof(line), _TRUNCATE, "[%02d:%02d:%02d.%03d] [inlink] %s\r\n",
                st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msg);

    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    char* slash = strrchr(path, '\\');
    if (slash) { *(slash + 1) = '\0'; }
    strcat_s(path, MAX_PATH, "beauty_debug.txt");

    HANDLE h = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
        SetFilePointer(h, 0, nullptr, FILE_END);
        DWORD written = 0;
        WriteFile(h, line, static_cast<DWORD>(strlen(line)), &written, nullptr);
        CloseHandle(h);
    }
}


class CWzCanvas : public IWzCanvas {
public:
    typedef HRESULT(__stdcall* raw_Serialize_t)(IWzCanvas*, IWzArchive*);
    inline static raw_Serialize_t raw_Serialize_orig;

    HRESULT __stdcall raw_Serialize_hook(IWzArchive* pArchive) {
        HRESULT hr = raw_Serialize_orig(this, pArchive);
        if (FAILED(hr)) {
            return hr;
        }
        Ztl_variant_t vInlink = this->property->item[L"_inlink"];
        if (V_VT(&vInlink) == VT_BSTR) {
            ZXString<wchar_t> sFilePath(pArchive->absoluteUOL);
            sFilePath.ReleaseBuffer(sFilePath.Find(L".img") + 5);
            sFilePath.Cat(V_BSTR(&vInlink));
            this->property->item[L"_inlink"] = static_cast<const wchar_t*>(sFilePath);
        }
        return hr;
    }
};

void HandleLinkProperty(IWzCanvasPtr pCanvas) {
    // Check for link property
    const wchar_t* asLinkProperty[] = {
        L"_inlink",
        L"_outlink",
        L"source",
    };
    size_t nLinkProperty = sizeof(asLinkProperty) / sizeof(asLinkProperty[0]);
    for (size_t i = 0; i < nLinkProperty; ++i) {
        Ztl_variant_t vLink;
        try {
            vLink = pCanvas->property->item[asLinkProperty[i]];
        } catch (...) {
            InlinkLog("HandleLinkProperty: EXCEPTION reading property[%ls]", asLinkProperty[i]);
            continue;
        }
        if (V_VT(&vLink) != VT_BSTR) {
            continue;
        }

        InlinkLog("HandleLinkProperty: resolving %ls=\"%ls\"", asLinkProperty[i], V_BSTR(&vLink));

        // Try resolving source canvas
        IWzCanvasPtr pSource;
        IUnknownPtr pUnknown;
        try {
            Ztl_variant_t vObj = get_rm()->GetObjectA(V_BSTR(&vLink));
            InlinkLog("HandleLinkProperty: GetObjectA returned VT=%d for \"%ls\"", V_VT(&vObj), V_BSTR(&vLink));
            if (V_VT(&vObj) == VT_EMPTY || V_VT(&vObj) == VT_ERROR) {
                InlinkLog("HandleLinkProperty: path not found \"%ls\"", V_BSTR(&vLink));
                continue;
            }
            pUnknown = vObj.GetUnknown();
        } catch (...) {
            InlinkLog("HandleLinkProperty: EXCEPTION in GetObjectA/GetUnknown for \"%ls\"", V_BSTR(&vLink));
            continue;
        }
        if (!pUnknown) {
            InlinkLog("HandleLinkProperty: GetUnknown returned null for \"%ls\"", V_BSTR(&vLink));
            continue;
        }
        HRESULT hr = pUnknown->QueryInterface(&pSource);
        if (FAILED(hr)) {
            InlinkLog("HandleLinkProperty: QueryInterface failed hr=0x%08X for \"%ls\"", (unsigned)hr, V_BSTR(&vLink));
            continue;
        }

        // Create target canvas
        int nWidth, nHeight, nFormat, nMagLevel;
        pSource->GetSnapshot(&nWidth, &nHeight, nullptr, nullptr, (CANVAS_PIXFORMAT*)&nFormat, &nMagLevel);
        pCanvas->Create(nWidth, nHeight, nMagLevel, nFormat);
        pCanvas->AddRawCanvas(0, 0, pSource->rawCanvas[0][0]);

        // Set target origin
        IWzVector2DPtr pOrigin = pCanvas->property->item[L"origin"].GetUnknown();
        pCanvas->cx = pOrigin->x;
        pCanvas->cy = pOrigin->y;
        InlinkLog("HandleLinkProperty: resolved OK %ls=\"%ls\" (%dx%d)", asLinkProperty[i], V_BSTR(&vLink), nWidth, nHeight);
        break;
    }
}

static auto get_unknown_orig = reinterpret_cast<IUnknownPtr*(__cdecl*)(IUnknownPtr*, Ztl_variant_t&)>(0x00414ADA);
IUnknownPtr* __cdecl get_unknown_hook(IUnknownPtr* result, Ztl_variant_t& v) {
    try {
        get_unknown_orig(result, v);
    } catch (...) {
        InlinkLog("get_unknown_hook: EXCEPTION in get_unknown_orig VT=%d", V_VT(&v));
        return result;
    }
    if (!result) {
        return result;
    }
    IWzCanvasPtr pCanvas;
    HRESULT hr = result->QueryInterface(__uuidof(IWzCanvas), &pCanvas);
    if (SUCCEEDED(hr) && pCanvas) {
        HandleLinkProperty(pCanvas);
    }
    return result;
}

static auto Ztl_variant_t__GetUnknown = reinterpret_cast<IUnknown*(__thiscall*)(Ztl_variant_t*, bool, bool)>(0x004032B2);
IUnknown* __fastcall Ztl_variant_t__GetUnknown_hook(Ztl_variant_t* pThis, void* _EDX, bool fAddRef, bool fTryChangeType) {
    IUnknownPtr result;
    try {
        result = pThis->GetUnknown(fAddRef, fTryChangeType);
    } catch (...) {
        InlinkLog("Ztl_variant_t__GetUnknown_hook: EXCEPTION VT=%d", V_VT(pThis));
        return nullptr;
    }
    if (!result) {
        return result;
    }
    IWzCanvasPtr pCanvas;
    HRESULT hr = result.QueryInterface(__uuidof(IWzCanvas), &pCanvas);
    if (SUCCEEDED(hr) && pCanvas) {
        HandleLinkProperty(pCanvas);
    }
    return result;
}


void AttachClientInlink() {
    CWzCanvas::raw_Serialize_orig = reinterpret_cast<CWzCanvas::raw_Serialize_t>(GetAddressByPattern("CANVAS.DLL", "B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 EC 6C"));
    ATTACH_HOOK(CWzCanvas::raw_Serialize_orig, CWzCanvas::raw_Serialize_hook);
    ATTACH_HOOK(get_unknown_orig, get_unknown_hook);
    ATTACH_HOOK(Ztl_variant_t__GetUnknown, Ztl_variant_t__GetUnknown_hook); // for cases where nexon uses this instead of get_unknown
}
