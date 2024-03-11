#include <iostream>
#include <vector>
#include <memory>
#include <Windows.h>

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    UNICODE_STRING ImageName;
    ULONG BasePriority;
    HANDLE UniqueProcessId;
    PVOID Reserved2;
    ULONG HandleCount;
    ULONG SessionId;
    PVOID Reserved3;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    PVOID Reserved5;
    SIZE_T QuotaPagedPoolUsage;
    PVOID Reserved6;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER Reserved7[6];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef NTSTATUS(WINAPI* PNtQuerySystemInformation)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

class HandleWrapper {
private:
    HANDLE handle;

public:
    explicit HandleWrapper(HANDLE h) : handle(h) {}
    ~HandleWrapper() {
        if (handle && handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }
    operator HANDLE() const { return handle; }
};

template<typename T>
class HeapAllocWrapper {
private:
    T* ptr;

public:
    explicit HeapAllocWrapper(size_t size) : ptr(static_cast<T*>(HeapAlloc(GetProcessHeap(), 0, size))) {}
    ~HeapAllocWrapper() {
        if (ptr) {
            HeapFree(GetProcessHeap(), 0, ptr);
        }
    }
    operator T*() const { return ptr; }
};

int main() {
    HandleWrapper hNtDll(LoadLibrary(L"ntdll.dll"));
    if (!hNtDll) {
        std::cerr << "Unable to load ntdll.dll" << std::endl;
        return 1;
    }

    PNtQuerySystemInformation NtQuerySystemInformation = reinterpret_cast<PNtQuerySystemInformation>(
        GetProcAddress(hNtDll, "NtQuerySystemInformation"));

    if (!NtQuerySystemInformation) {
        std::cerr << "Unable to locate NtQuerySystemInformation func" << std::endl;
        return 1;
    }

    ULONG returnLength = 0;
    NTSTATUS status;

    status = NtQuerySystemInformation(5, nullptr, 0, &returnLength);
    if (status != STATUS_INFO_LENGTH_MISMATCH) {
        std::cerr << "NtQuerySystemInformation failed" << status << std::endl;
        return 1;
    }

    HeapAllocWrapper<SYSTEM_PROCESS_INFORMATION> processInfo(returnLength);
    if (!processInfo) {
        std::cerr << "Memory allocation failed" << std::endl;
        return 1;
    }

    status = NtQuerySystemInformation(5, processInfo, returnLength, &returnLength);
    if (!NT_SUCCESS(status)) {
        std::cerr << "NtQuerySystemInformation failed " << status << std::endl;
        return 1;
    }

    std::vector<SYSTEM_PROCESS_INFORMATION*> processes;
    PSYSTEM_PROCESS_INFORMATION pCurr = processInfo;
    while (pCurr != nullptr) {
        processes.push_back(pCurr);
        if (pCurr->NextEntryOffset == 0) break;
        pCurr = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(reinterpret_cast<BYTE*>(pCurr) + pCurr->NextEntryOffset);
    }

    for (const auto& process : processes) {
        std::wcout << "PID: " << reinterpret_cast<DWORD>(process->UniqueProcessId) << "\tName: " << process->ImageName.Buffer << std::endl;
    }

    return 0;
}
