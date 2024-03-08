#include <Windows.h>
#include <iostream>
#include <string>

void cAtt(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

std::string Processor(WORD type) {
    switch (type) {
        case PROCESSOR_ARCHITECTURE_INTEL:
            return "x86";
        case PROCESSOR_ARCHITECTURE_ARM:
            return "ARM";
        case PROCESSOR_ARCHITECTURE_AMD64:
            return "x64";
        default:
            return "wtf is your processor nigga ? ";
    }
}

int main() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    system("cls");

    cAtt(9);
    std::cout << "[+] Processor : " << Processor(sysInfo.wProcessorArchitecture) << std::endl;

    cAtt(4);
    std::cout << "[=] Number of Processors : " << sysInfo.dwNumberOfProcessors << std::endl;

    cAtt(7);
    return 0;
}
