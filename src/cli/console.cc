#include "console.h"
#include <windows.h>

Cli::GamePanel::GamePanel(Core::Game* g) : game_(g) {

    // Change the output code page to UTF-8
    SetConsoleOutputCP(CP_UTF8);

    // Get the standard output handle
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // Get console info
    GetConsoleScreenBufferInfo(hOut, &bInfo);

    if (bInfo.dwSize.X < kGameConsoleWidth) bInfo.dwSize.X = kGameConsoleWidth;

    COORD bufSize = { kGameConsoleWidth, kGameConsoleHeight };
    SetConsoleScreenBufferSize(hOut, bufSize);
    printEntranceMenu();
}

Cli::GamePanel::~GamePanel() {
    CloseHandle(hOut);
}

void Cli::GamePanel::setCursorPos(short x, short y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(hOut, pos);
}

void Cli::GamePanel::clearScreen() {
    DWORD buffer;
    for (short i = 0; i <= kGameConsoleHeight; ++i) {
        COORD pos = { 0, i };
        short default_color = (static_cast<short>(kDarkGrey) << 4)+ static_cast<short>(kWhite);
        FillConsoleOutputAttribute(hOut, default_color, kGameConsoleWidth, pos, &buffer);
        FillConsoleOutputCharacter(hOut, ' ', kGameConsoleWidth, pos, &buffer);
    }
    setCursorPos(0, 0);
}

void Cli::GamePanel::printEntranceMenu() {
    SetConsoleTitle(L"Welcome to Robox");
}