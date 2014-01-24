#include "stdafx.h"
#include "WindowsUser.h"

#pragma comment(lib, "advapi32.lib")


#define DESKTOP_ALL (DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | \
    DESKTOP_CREATEMENU | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALRECORD | \
    DESKTOP_JOURNALPLAYBACK | DESKTOP_ENUMERATE | DESKTOP_WRITEOBJECTS | \
    DESKTOP_SWITCHDESKTOP | STANDARD_RIGHTS_REQUIRED)

#define WINSTA_ALL (WINSTA_ENUMDESKTOPS | WINSTA_READATTRIBUTES | \
    WINSTA_ACCESSCLIPBOARD | WINSTA_CREATEDESKTOP | \
    WINSTA_WRITEATTRIBUTES | WINSTA_ACCESSGLOBALATOMS | \
    WINSTA_EXITWINDOWS | WINSTA_ENUMERATE | WINSTA_READSCREEN | \
    STANDARD_RIGHTS_REQUIRED)

#define GENERIC_ACCESS (GENERIC_READ | GENERIC_WRITE | \
    GENERIC_EXECUTE | GENERIC_ALL)


namespace IMUST
{
    void OutputErrorMsg(const OJString & msg)
    {
        DebugMessage(OJStr("%s : code %d"), msg.c_str(), GetLastError());
    }


    WindowsUser::WindowsUser(void)
        : tokenHandle_(NULL)
        , deskHandle_(NULL)
        , winstaHandle_(NULL)
        , pSid_(NULL)
    {
    }


    WindowsUser::~WindowsUser(void)
    {
        logout();
    }

    bool WindowsUser::login(const OJString & userName, const OJString & domain, const OJString & password)
    {
        assert(tokenHandle_==NULL && "wrong call!");

        // Log the client on to the local computer.

        if (!LogonUser(
            userName.c_str(),
            domain.c_str(),
            password.c_str(),
            LOGON32_LOGON_INTERACTIVE,
            LOGON32_PROVIDER_DEFAULT,
            &tokenHandle_) ) 
        {
            tokenHandle_ = NULL;
            OutputErrorMsg(OJStr("LogonUser failed "));
            return false;
        }

        // Save a handle to the caller's current window station.


        HWINSTA hwinstaSave = GetProcessWindowStation();
        if (hwinstaSave == NULL)
        {
            OutputErrorMsg(OJStr("GetProcessWindowStation failed "));
            return false;
        }

        // Get a handle to the interactive window station.

        winstaHandle_ = OpenWindowStation(
            OJStr("winsta0"),               // the interactive window station 
            FALSE,                       // handle is not inheritable
            READ_CONTROL | WRITE_DAC);   // rights to read/write the DACL

        if (winstaHandle_ == NULL) 
        {
            OutputErrorMsg(OJStr("OpenWindowStation failed!"));
            return false;
        }

        // To get the correct default desktop, set the caller's 
        // window station to the interactive window station.

        if (!SetProcessWindowStation(winstaHandle_))
        {
            OutputErrorMsg(OJStr("SetProcessWindowStation 1 failed "));
            return false;
        }

        // Get a handle to the interactive desktop.

        deskHandle_ = OpenDesktop(
            OJStr("default"),     // the interactive window station 
            0,             // no interaction with other desktop processes
            FALSE,         // handle is not inheritable
            READ_CONTROL | // request the rights to read and write the DACL
            WRITE_DAC | 
            DESKTOP_WRITEOBJECTS | 
            DESKTOP_READOBJECTS);

        // Restore the caller's window station.

        if (!SetProcessWindowStation(hwinstaSave)) 
        {
            OutputErrorMsg(OJStr("SetProcessWindowStation 2 failed "));
            return false;
        }

        if (deskHandle_ == NULL)
        {
            OutputErrorMsg(OJStr("OpenDesktop failed"));
            return false;
        }

        // Get the SID for the client's logon session.

        if (!getLogonSID(tokenHandle_, &pSid_)) 
        {
            OutputErrorMsg(OJStr("GetLogonSID failed "));
            return false;
        }

        // Allow logon SID full access to interactive window station.

        if (! addAceToWindowStation(winstaHandle_, pSid_) ) 
        {
            OutputErrorMsg(OJStr("AddAceToWindowStation failed "));
            return false;
        }

        // Allow logon SID full access to interactive desktop.

        if (! addAceToDesktop(deskHandle_, pSid_) ) 
        {
            OutputErrorMsg(OJStr("AddAceToDesktop failed "));
            return false;
        }

        // Impersonate client to ensure access to executable file.

        if (! ImpersonateLoggedOnUser(tokenHandle_) ) 
        {
            OutputErrorMsg(OJStr("ImpersonateLoggedOnUser failed "));
            return false;
        }

        return true;
    }

    void WindowsUser::logout()
    {
        // Free the buffer for the logon SID.

        if (pSid_)
        {
            freeLogonSID(&pSid_);
        }

        // Close the handles to the interactive window station and desktop.

        if (winstaHandle_)
        {
            CloseWindowStation(winstaHandle_);
            winstaHandle_ = NULL;
        }

        if (deskHandle_)
        {
            CloseDesktop(deskHandle_);
            deskHandle_ = NULL;
        }

        // Close the handle to the client's access token.

        if (tokenHandle_)
        {
            RevertToSelf();
            CloseHandle(tokenHandle_);  
            tokenHandle_ = NULL;
        }
    }


    bool WindowsUser::addAceToWindowStation(HWINSTA hwinsta, PSID psid)
    {
        ACCESS_ALLOWED_ACE   *pace = NULL;
        ACL_SIZE_INFORMATION aclSizeInfo;
        BOOL                 bDaclExist;
        BOOL                 bDaclPresent;
        BOOL                 bSuccess = FALSE;
        DWORD                dwNewAclSize;
        DWORD                dwSidSize = 0;
        DWORD                dwSdSizeNeeded;
        PACL                 pacl;
        PACL                 pNewAcl = NULL;
        PSECURITY_DESCRIPTOR psd = NULL;
        PSECURITY_DESCRIPTOR psdNew = NULL;
        PVOID                pTempAce;
        SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
        unsigned int         i;

        __try
        {
            // Obtain the DACL for the window station.

            if (!GetUserObjectSecurity(
                hwinsta,
                &si,
                psd,
                dwSidSize,
                &dwSdSizeNeeded)
                )
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                {
                    psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
                        GetProcessHeap(),
                        HEAP_ZERO_MEMORY,
                        dwSdSizeNeeded);

                    if (psd == NULL)
                        __leave;

                    psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
                        GetProcessHeap(),
                        HEAP_ZERO_MEMORY,
                        dwSdSizeNeeded);

                    if (psdNew == NULL)
                        __leave;

                    dwSidSize = dwSdSizeNeeded;

                    if (!GetUserObjectSecurity(
                        hwinsta,
                        &si,
                        psd,
                        dwSidSize,
                        &dwSdSizeNeeded)
                        )
                        __leave;
                }
                else
                    __leave;

            // Create a new DACL.

            if (!InitializeSecurityDescriptor(
                psdNew,
                SECURITY_DESCRIPTOR_REVISION)
                )
                __leave;

            // Get the DACL from the security descriptor.

            if (!GetSecurityDescriptorDacl(
                psd,
                &bDaclPresent,
                &pacl,
                &bDaclExist)
                )
                __leave;

            // Initialize the ACL.

            ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
            aclSizeInfo.AclBytesInUse = sizeof(ACL);

            // Call only if the DACL is not NULL.

            if (pacl != NULL)
            {
                // get the file ACL size info
                if (!GetAclInformation(
                    pacl,
                    (LPVOID)&aclSizeInfo,
                    sizeof(ACL_SIZE_INFORMATION),
                    AclSizeInformation)
                    )
                    __leave;
            }

            // Compute the size of the new ACL.

            dwNewAclSize = aclSizeInfo.AclBytesInUse +
                (2*sizeof(ACCESS_ALLOWED_ACE)) + (2*GetLengthSid(psid)) -
                (2*sizeof(DWORD));

            // Allocate memory for the new ACL.

            pNewAcl = (PACL)HeapAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                dwNewAclSize);

            if (pNewAcl == NULL)
                __leave;

            // Initialize the new DACL.

            if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
                __leave;

            // If DACL is present, copy it to a new DACL.

            if (bDaclPresent)
            {
                // Copy the ACEs to the new ACL.
                if (aclSizeInfo.AceCount)
                {
                    for (i=0; i < aclSizeInfo.AceCount; i++)
                    {
                        // Get an ACE.
                        if (!GetAce(pacl, i, &pTempAce))
                            __leave;

                        // Add the ACE to the new ACL.
                        if (!AddAce(
                            pNewAcl,
                            ACL_REVISION,
                            MAXDWORD,
                            pTempAce,
                            ((PACE_HEADER)pTempAce)->AceSize)
                            )
                            __leave;
                    }
                }
            }

            // Add the first ACE to the window station.

            pace = (ACCESS_ALLOWED_ACE *)HeapAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) -
                sizeof(DWORD));

            if (pace == NULL)
                __leave;

            pace->Header.AceType  = ACCESS_ALLOWED_ACE_TYPE;
            pace->Header.AceFlags = CONTAINER_INHERIT_ACE |
                INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;
            pace->Header.AceSize  = LOWORD(sizeof(ACCESS_ALLOWED_ACE) +
                GetLengthSid(psid) - sizeof(DWORD));
            pace->Mask            = GENERIC_ACCESS;

            if (!CopySid(GetLengthSid(psid), &pace->SidStart, psid))
                __leave;

            if (!AddAce(
                pNewAcl,
                ACL_REVISION,
                MAXDWORD,
                (LPVOID)pace,
                pace->Header.AceSize)
                )
                __leave;

            // Add the second ACE to the window station.

            pace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
            pace->Mask            = WINSTA_ALL;

            if (!AddAce(
                pNewAcl,
                ACL_REVISION,
                MAXDWORD,
                (LPVOID)pace,
                pace->Header.AceSize)
                )
                __leave;

            // Set a new DACL for the security descriptor.

            if (!SetSecurityDescriptorDacl(
                psdNew,
                TRUE,
                pNewAcl,
                FALSE)
                )
                __leave;

            // Set the new security descriptor for the window station.

            if (!SetUserObjectSecurity(hwinsta, &si, psdNew))
                __leave;

            // Indicate success.

            bSuccess = TRUE;
        }
        __finally
        {
            // Free the allocated buffers.

            if (pace != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)pace);

            if (pNewAcl != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

            if (psd != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

            if (psdNew != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
        }

        return !!bSuccess;
    }

    bool WindowsUser::addAceToDesktop(HDESK hdesk, PSID psid)
    {
        ACL_SIZE_INFORMATION aclSizeInfo;
        BOOL                 bDaclExist;
        BOOL                 bDaclPresent;
        BOOL                 bSuccess = FALSE;
        DWORD                dwNewAclSize;
        DWORD                dwSidSize = 0;
        DWORD                dwSdSizeNeeded;
        PACL                 pacl;
        PACL                 pNewAcl = NULL;
        PSECURITY_DESCRIPTOR psd = NULL;
        PSECURITY_DESCRIPTOR psdNew = NULL;
        PVOID                pTempAce;
        SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
        unsigned int         i;

        __try
        {
            // Obtain the security descriptor for the desktop object.

            if (!GetUserObjectSecurity(
                hdesk,
                &si,
                psd,
                dwSidSize,
                &dwSdSizeNeeded))
            {
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                {
                    psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
                        GetProcessHeap(),
                        HEAP_ZERO_MEMORY,
                        dwSdSizeNeeded );

                    if (psd == NULL)
                        __leave;

                    psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
                        GetProcessHeap(),
                        HEAP_ZERO_MEMORY,
                        dwSdSizeNeeded);

                    if (psdNew == NULL)
                        __leave;

                    dwSidSize = dwSdSizeNeeded;

                    if (!GetUserObjectSecurity(
                        hdesk,
                        &si,
                        psd,
                        dwSidSize,
                        &dwSdSizeNeeded)
                        )
                        __leave;
                }
                else
                    __leave;
            }

            // Create a new security descriptor.

            if (!InitializeSecurityDescriptor(
                psdNew,
                SECURITY_DESCRIPTOR_REVISION)
                )
                __leave;

            // Obtain the DACL from the security descriptor.

            if (!GetSecurityDescriptorDacl(
                psd,
                &bDaclPresent,
                &pacl,
                &bDaclExist)
                )
                __leave;

            // Initialize.

            ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
            aclSizeInfo.AclBytesInUse = sizeof(ACL);

            // Call only if NULL DACL.

            if (pacl != NULL)
            {
                // Determine the size of the ACL information.

                if (!GetAclInformation(
                    pacl,
                    (LPVOID)&aclSizeInfo,
                    sizeof(ACL_SIZE_INFORMATION),
                    AclSizeInformation)
                    )
                    __leave;
            }

            // Compute the size of the new ACL.

            dwNewAclSize = aclSizeInfo.AclBytesInUse +
                sizeof(ACCESS_ALLOWED_ACE) +
                GetLengthSid(psid) - sizeof(DWORD);

            // Allocate buffer for the new ACL.

            pNewAcl = (PACL)HeapAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                dwNewAclSize);

            if (pNewAcl == NULL)
                __leave;

            // Initialize the new ACL.

            if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
                __leave;

            // If DACL is present, copy it to a new DACL.

            if (bDaclPresent)
            {
                // Copy the ACEs to the new ACL.
                if (aclSizeInfo.AceCount)
                {
                    for (i=0; i < aclSizeInfo.AceCount; i++)
                    {
                        // Get an ACE.
                        if (!GetAce(pacl, i, &pTempAce))
                            __leave;

                        // Add the ACE to the new ACL.
                        if (!AddAce(
                            pNewAcl,
                            ACL_REVISION,
                            MAXDWORD,
                            pTempAce,
                            ((PACE_HEADER)pTempAce)->AceSize)
                            )
                            __leave;
                    }
                }
            }

            // Add ACE to the DACL.

            if (!AddAccessAllowedAce(
                pNewAcl,
                ACL_REVISION,
                DESKTOP_ALL,
                psid)
                )
                __leave;

            // Set new DACL to the new security descriptor.

            if (!SetSecurityDescriptorDacl(
                psdNew,
                TRUE,
                pNewAcl,
                FALSE)
                )
                __leave;

            // Set the new security descriptor for the desktop object.

            if (!SetUserObjectSecurity(hdesk, &si, psdNew))
                __leave;

            // Indicate success.

            bSuccess = TRUE;
        }
        __finally
        {
            // Free buffers.

            if (pNewAcl != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

            if (psd != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

            if (psdNew != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
        }

        return !!bSuccess;
    }

    bool WindowsUser::getLogonSID (HANDLE hToken, PSID *ppsid) 
    {
        BOOL bSuccess = FALSE;
        DWORD dwIndex;
        DWORD dwLength = 0;
        PTOKEN_GROUPS ptg = NULL;

        // Verify the parameter passed in is not NULL.
        if (NULL == ppsid)
            goto Cleanup;

        // Get required buffer size and allocate the TOKEN_GROUPS buffer.

        if (!GetTokenInformation(
            hToken,         // handle to the access token
            TokenGroups,    // get information about the token's groups 
            (LPVOID) ptg,   // pointer to TOKEN_GROUPS buffer
            0,              // size of buffer
            &dwLength       // receives required buffer size
            )) 
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
                goto Cleanup;

            ptg = (PTOKEN_GROUPS)HeapAlloc(GetProcessHeap(),
                HEAP_ZERO_MEMORY, dwLength);

            if (ptg == NULL)
                goto Cleanup;
        }

        // Get the token group information from the access token.

        if (!GetTokenInformation(
            hToken,         // handle to the access token
            TokenGroups,    // get information about the token's groups 
            (LPVOID) ptg,   // pointer to TOKEN_GROUPS buffer
            dwLength,       // size of buffer
            &dwLength       // receives required buffer size
            )) 
        {
            goto Cleanup;
        }

        // Loop through the groups to find the logon SID.

        for (dwIndex = 0; dwIndex < ptg->GroupCount; dwIndex++) 
            if ((ptg->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID)
                ==  SE_GROUP_LOGON_ID) 
            {
                // Found the logon SID; make a copy of it.

                dwLength = GetLengthSid(ptg->Groups[dwIndex].Sid);
                *ppsid = (PSID) HeapAlloc(GetProcessHeap(),
                    HEAP_ZERO_MEMORY, dwLength);
                if (*ppsid == NULL)
                    goto Cleanup;
                if (!CopySid(dwLength, *ppsid, ptg->Groups[dwIndex].Sid)) 
                {
                    HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
                    goto Cleanup;
                }
                break;
            }

            bSuccess = TRUE;

Cleanup: 

            // Free the buffer for the token groups.

            if (ptg != NULL)
                HeapFree(GetProcessHeap(), 0, (LPVOID)ptg);

            return !!bSuccess;
    }

    void WindowsUser::freeLogonSID (PSID *ppsid) 
    {
        HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
    }


    bool WindowsUser::setPrivilege(
        LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
        BOOL bEnablePrivilege   // to enable or disable privilege
        ) 
    {
        TOKEN_PRIVILEGES tp;
        LUID luid;

        if ( !LookupPrivilegeValue( 
            NULL,            // lookup privilege on local system
            lpszPrivilege,   // privilege to lookup 
            &luid ) )        // receives LUID of privilege
        {
            OutputErrorMsg(OJStr("LookupPrivilegeValue error ")); 
            return false; 
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        if (bEnablePrivilege)
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        else
            tp.Privileges[0].Attributes = 0;

        // Enable the privilege or disable all privileges.

        if ( !AdjustTokenPrivileges(
            tokenHandle_, 
            FALSE, 
            &tp, 
            sizeof(TOKEN_PRIVILEGES), 
            (PTOKEN_PRIVILEGES) NULL, 
            (PDWORD) NULL) )
        { 
            OutputErrorMsg(OJStr("AdjustTokenPrivileges failded")); 
            return false; 
        } 

        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
        {
            OutputErrorMsg(OJStr("The token does not have the specified privilege. "));
            return false;
        } 

        return true;
    }

    bool WindowsUser::createProcess(
        LPCTSTR lpApplicationName,
        LPTSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCTSTR lpCurrentDirectory,
        LPSTARTUPINFO lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation)
    {
        if (!CreateProcessAsUser(tokenHandle_,
            lpApplicationName,
            lpCommandLine,
            lpProcessAttributes,
            lpThreadAttributes,
            bInheritHandles,
            dwCreationFlags,
            lpEnvironment,
            lpCurrentDirectory,
            lpStartupInfo,
            lpProcessInformation))
        {
            OutputErrorMsg(OJStr("CreateProcessAsUser faild"));
            return false;
        }

        return true;
    }

}//namespace IMUST