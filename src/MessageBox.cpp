#include <windows.h>
#include <string>

void vimBox(const std::wstring& title, const std::wstring& message) {
    MessageBoxW(NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::wstring title = L"vimshell";
    std::wstring message = L"hi can u guys hear me ";

    vimBox(title, message);

    return 0;
}
