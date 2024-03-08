#include <iostream>
#include <windows.h>
#include <tlhelp32.h>

void procList() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Couldn't create process Snapshot." << std::endl;
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        std::cerr << "Failed to fetch process information" << std::endl;
        CloseHandle(hSnapshot);
        return;
    }

    std::cout << "Process ID\tProcess Name" << std::endl;
    do {
        std::cout << pe32.th32ProcessID << "\t\t" << pe32.szExeFile << std::endl;
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}

int main() {
    procList();
    return 0;
}
