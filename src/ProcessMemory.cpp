#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <iomanip>
#include <vector>
#include <algorithm>

const int PROCESS_VM_READ = 0x0010;

void handle_error(const char *message) {
    std::cerr << "Error: " << message << std::endl;
    exit(EXIT_FAILURE);
}

void dump_memory(HANDLE processHandle, DWORD_PTR baseAddress, SIZE_T size, bool hexDump = true) {
    const int CHUNK_SIZE = 16;
    BYTE buffer[CHUNK_SIZE];

    SIZE_T bytesRead = 0;
    for (DWORD_PTR offset = 0; offset < size; offset += CHUNK_SIZE) {
        ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(baseAddress + offset), buffer, sizeof(buffer), &bytesRead);
        if (bytesRead == 0)
            break;

        std::cout << std::hex << std::setw(8) << std::setfill('0') << baseAddress + offset << "   ";
        if (hexDump) {
            for (size_t i = 0; i < bytesRead; ++i)
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]) << " ";
        } else {
            std::cout << std::hex << std::setfill(' ') << std::setw(CHUNK_SIZE * 3);
            for (size_t i = 0; i < bytesRead; ++i) {
                if (isprint(buffer[i]))
                    std::cout << buffer[i];
                else
                    std::cout << '.';
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    DWORD processId;
    std::cout << "Enter process ID: ";
    std::cin >> processId;

    HANDLE processHandle = OpenProcess(PROCESS_VM_READ, FALSE, processId);
    if (processHandle == NULL)
        handle_error("Failed to open process");

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    Module32First(snapshot, &moduleEntry);
    CloseHandle(snapshot);

    std::cout << "Dumping memory of process '" << moduleEntry.szModule << "' (ID: " << processId << ")" << std::endl;
    std::cout << "Base address: " << std::hex << moduleEntry.modBaseAddr << std::endl;
    std::cout << "Module size: " << std::dec << moduleEntry.modBaseSize << " bytes" << std::endl;

    dump_memory(processHandle, reinterpret_cast<DWORD_PTR>(moduleEntry.modBaseAddr), moduleEntry.modBaseSize);

    CloseHandle(processHandle);

    return 0;
}
