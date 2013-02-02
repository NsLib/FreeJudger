#include <windows.h>
#include "detours.h"

int (WINAPI *OriFun)(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption,UINT uType) = MessageBoxW;  
int WINAPI ZwNHookFun(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption,UINT uType)  
{  
    return OriFun(hWnd, L"Hook This!",L"My hook",uType);  
}  

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    (void)hinst;
    (void)reserved;

    if (DetourIsHelperProcess())
        return TRUE;

    if (dwReason == DLL_PROCESS_ATTACH) 
    {
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)OriFun, ZwNHookFun);
        DetourTransactionCommit();
    }
    else if (dwReason == DLL_PROCESS_DETACH) 
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)OriFun, ZwNHookFun);
        DetourTransactionCommit();
    }
    return TRUE;
}
