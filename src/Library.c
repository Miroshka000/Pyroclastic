#define INITGUID
#define _MINAPPMODEL_H_

#define MINECRAFTUWP L"Microsoft.MinecraftUWP_8wekyb3d8bbwe"
#define MINECRAFTWINDOWSBETA L"Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe"

#include <windows.h>
#include <appmodel.h>
#include <winternl.h>

WCHAR g_szPackageFamilyName[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1] = {};

__declspec(dllexport) VOID GameLaunch(VOID)
{
    HANDLE hMutex = CreateMutexW(NULL, FALSE, g_szPackageFamilyName);

    if (!GetLastError())
    {
        HANDLE hObject = {};
        PROCESS_INFORMATION _ = {};

        PRTL_USER_PROCESS_PARAMETERS pParameters = NtCurrentTeb()->ProcessEnvironmentBlock->ProcessParameters;
        PWSTR pszCommandLine = pParameters->CommandLine.Buffer + lstrlenW(pParameters->ImagePathName.Buffer) + 2;

        CreateProcessW(L"Minecraft.Windows.exe", pszCommandLine, NULL, NULL, FALSE, 0, 0, NULL, &(STARTUPINFOW){}, &_);
        CloseHandle(_.hThread);

        DuplicateHandle(GetCurrentProcess(), hMutex, _.hProcess, &hObject, 0, FALSE, DUPLICATE_SAME_ACCESS);
        CloseHandle(hObject);

        WaitForInputIdle(_.hProcess, INFINITE);
        CloseHandle(_.hProcess);
    }
    else
    {
        HWND hWnd = {};
        while ((hWnd = FindWindowExW(NULL, hWnd, L"Bedrock", NULL)))
        {
            DWORD dwProcessId = {};
            GetWindowThreadProcessId(hWnd, &dwProcessId);

            WCHAR szPackageFamilyName[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1] = {};
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);

            if (!GetPackageFamilyName(hProcess, &(UINT32){ARRAYSIZE(szPackageFamilyName)}, szPackageFamilyName) &&
                CompareStringOrdinal(g_szPackageFamilyName, -1, szPackageFamilyName, -1, TRUE) == CSTR_EQUAL)
                SwitchToThisWindow(hWnd, TRUE);

            CloseHandle(hProcess);
        }
    }

    CloseHandle(hMutex);
}

BOOL DllMain(HINSTANCE hInstance, DWORD dwReason, PVOID pReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetCurrentPackageFamilyName(&(UINT){ARRAYSIZE(g_szPackageFamilyName)}, g_szPackageFamilyName))
            ExitProcess(EXIT_FAILURE);

        if (CompareStringOrdinal(MINECRAFTUWP, -1, g_szPackageFamilyName, -1, TRUE) != CSTR_EQUAL &&
            CompareStringOrdinal(MINECRAFTWINDOWSBETA, -1, g_szPackageFamilyName, -1, TRUE) != CSTR_EQUAL)
            ExitProcess(EXIT_FAILURE);

        WCHAR szPathName[MAX_PATH] = {};
        if (GetCurrentPackagePath(&(UINT){ARRAYSIZE(szPathName)}, szPathName) || !SetCurrentDirectoryW(szPathName))
            ExitProcess(EXIT_FAILURE);
    }
    return TRUE;
}