#include "console.h"
#include <ncurses.h>

void Cli::GamePanel::initScreen() {
    setlocale(LC_ALL, "");
    initscr();
    main_window_ = newwin(kGameConsoleWidth, kGameConsoleHeight, 0, 0);
    box(main_window_, 0, 0);
    for (int i = 3; i <= 9; ++i) {
        mvaddwstr(i, 4, kGameMainTitle[i - 3]);
    }
    wrefresh(main_window_);
}

void Cli::GamePanel::getInput() {
    
}

void Cli::GamePanel::run() {
    initScreen();
}


