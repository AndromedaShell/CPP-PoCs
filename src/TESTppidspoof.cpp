#include <windows.h>
#include <TlHelp32.h>
#include <iostream>

int main() {
    STARTUPINFOEX si;
    PROCESS_INFORMATION pi;
    SIZE_T attributeSize;

    ZeroMemory(&si, sizeof(STARTUPINFOEX));

    HANDLE parentProcessHandle = OpenProcess(PROCESS_CREATE_PROCESS, false, 6200); 

    if (parentProcessHandle == NULL) {
        std::cerr << "[\] Failed to open parent process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    InitializeProcThreadAttributeList(NULL, 1, 0, &attributeSize);
    si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attributeSize);
    InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attributeSize);

    if (!UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parentProcessHandle, sizeof(HANDLE), NULL, NULL)) {
        std::cerr << "[\] Couldn't set parent process attribute. Error: " << GetLastError() << std::endl;
        CloseHandle(parentProcessHandle);
        return 1;
    }

    si.StartupInfo.cb = sizeof(STARTUPINFOEX);

    if (!CreateProcessA(NULL, (LPSTR)"notepad", NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &si.StartupInfo, &pi)) {
        std::cerr << "[\] Couldn't create process. Error: " << GetLastError() << std::endl;
        CloseHandle(parentProcessHandle);
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(parentProcessHandle);
    HeapFree(GetProcessHeap(), 0, si.lpAttributeList);

    return 0;
}
