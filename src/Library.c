#define _MINAPPMODEL_H_
#include <windows.h>
#include <appmodel.h>
#include <winternl.h>
#include <wtsapi32.h>

__declspec(dllexport) VOID GameLaunch(VOID)
{
    WCHAR pfn[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1] = {};
    if (GetCurrentPackageFamilyName(&(UINT){ARRAYSIZE(pfn)}, pfn))
        return;

    WCHAR path[MAX_PATH] = {};
    if (GetCurrentPackagePath(&(UINT){MAX_PATH}, path) || !SetCurrentDirectoryW(path))
        return;

    HANDLE mutex = CreateMutexW(NULL, FALSE, mutex);

    if (!mutex || GetLastError())
    {
        CloseHandle(mutex);
        return;
    }

    DWORD count = {};
    HANDLE process = {};
    WTS_PROCESS_INFOW *items = {};

    if (WTSEnumerateProcessesW(WTS_CURRENT_SERVER, 0, 1, &items, &count))
    {
        for (DWORD index = {}; index < count; index++)
        {
            WTS_PROCESS_INFOW item = items[index];

            if (CompareStringOrdinal(L"Minecraft.Windows.exe", -1, item.pProcessName, -1, TRUE) != CSTR_EQUAL)
                continue;

            WCHAR buffer[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1] = {};
            process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, item.ProcessId);

            if (!GetPackageFamilyName(process, &(UINT){ARRAYSIZE(buffer)}, buffer) &&
                CompareStringOrdinal(pfn, -1, buffer, -1, TRUE) == CSTR_EQUAL)
                break;

            CloseHandle(process);
        }
        WTSFreeMemory(items);
    }

    if (WaitForInputIdle(process, INFINITE))
    {
        PRTL_USER_PROCESS_PARAMETERS params = NtCurrentTeb()->ProcessEnvironmentBlock->ProcessParameters;
        PWSTR args = params->CommandLine.Buffer + lstrlenW(params->ImagePathName.Buffer) + 2;

        PROCESS_INFORMATION info = {};
        CreateProcessW(L"Minecraft.Windows.exe", args, NULL, NULL, FALSE, 0, 0, NULL, &(STARTUPINFOW){}, &info);
        CloseHandle(info.hThread);

        WaitForInputIdle(info.hProcess, INFINITE);
        CloseHandle(info.hProcess);
    }
    CloseHandle(process);

    HWND wnd = {};
    while ((wnd = FindWindowExW(NULL, wnd, L"Bedrock", NULL)))
    {
        DWORD processId = {};
        GetWindowThreadProcessId(wnd, &processId);

        WCHAR buffer[PACKAGE_FAMILY_NAME_MAX_LENGTH + 1] = {};
        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);

        if (!GetPackageFamilyName(process, &(UINT){ARRAYSIZE(buffer)}, buffer) &&
            CompareStringOrdinal(pfn, -1, buffer, -1, TRUE) == CSTR_EQUAL)
            SwitchToThisWindow(wnd, TRUE);

        CloseHandle(process);
    }

    CloseHandle(mutex);
}

BOOL DllMain(HINSTANCE instance, DWORD reason, PVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
        DisableThreadLibraryCalls(instance);
    return TRUE;
}