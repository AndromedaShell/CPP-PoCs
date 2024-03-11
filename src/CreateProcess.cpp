#include <iostream>
#include <Windows.h>

int main() {
    STARTUPINFOA startupInfo = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION processInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    LPCSTR command = "C:\\Windows\\System32\\notepad.exe";
    BOOL success = CreateProcessA(
        NULL,
        (LPSTR)command,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &startupInfo,
        &processInfo
    );

    if (success != 0) {
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    } else {
        std::cout << "Failed to create process." << std::endl;
    }

    return 0;
}
