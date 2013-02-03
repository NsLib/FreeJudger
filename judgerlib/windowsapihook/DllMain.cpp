#include <windows.h>
#include "detours.h"

//int (WINAPI *OriFun)(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption,UINT uType) = MessageBoxW;
INT_PTR (WINAPI *OriFun)(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent,DLGPROC lpDialogFunc, LPARAM dwInitParam) = DialogBoxParamW; 
INT_PTR WINAPI ZwNHookFun(HINSTANCE hInstance, LPCWSTR lpTemplateName, HWND hWndParent,DLGPROC lpDialogFunc, LPARAM dwInitParam)  
{  
    return NULL;  
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
