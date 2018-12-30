#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID lpvReserved)
{
    if (reason == DLL_PROCESS_ATTACH) {
        MessageBoxA(0, "This fix brought to you by reading the documentation!", "Age Of Empires KeyState fix", 0);
    }
    return TRUE;
}

SHORT __stdcall PROXY_GetAsyncKeyState(int vKey) {
    return GetAsyncKeyState(vKey) & 0x8000;
}

SHORT __stdcall PROXY_GetKeyState(int vKey) {
    return GetKeyState(vKey) & 0x8000;
}

BOOL __stdcall PROXY_GetKeyboardState(PBYTE lpKeyState) {
    BOOL ret = GetKeyboardState(lpKeyState);
    if (ret) {
        for (size_t i = 0; i < 256; i++)
            lpKeyState[i] &= 0x80;
    }
    return ret;
}
