#include <core/core.h>
#include "console.h"
#include <ncurses.h>

/** 
 * @program:     Cli::GamePanel::initScreen
 * @description: This function is to put the GameMainTitle and GameInfo to the console
 */
void Cli::GamePanel::initScreen() {
    setlocale(LC_ALL, "");
    initscr();
    raw();
    noecho();
    main_window_ = newwin(kGameConsoleHeight, kGameConsoleWidth, 0, 0);
    box(main_window_, 0, 0);

    Core::logMessage("The game console panel has been initalized.", 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);

    int topleft_x = (kGameConsoleWidth - kGameMainTitleWidth) / 2;
    int topleft_y = (kGameConsoleHeight - (kGameMainTitleHeight + kGameInfoHeight + 1)) / 2;

    // gametitle_topleft_x : The x coordinates of topleft corner of MainTitle and GameInfo
    // gametitle_topleft_y : The y coordinates of topleft corner of MainTitle and GameInfo

    for (int i = topleft_y; i <= topleft_y + kGameMainTitleHeight - 1; ++i) {
        mvwaddwstr(main_window_, i, topleft_x, kGameMainTitle[i - topleft_y]);
    }

    Core::logMessage("The game title has been initialized.", 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);

    // notice between game title and game info there is an empty line

    for (int i = topleft_y + kGameMainTitleHeight + 1; i <= topleft_y + kGameMainTitleHeight + kGameInfoHeight; ++i) {
        mvwaddwstr(main_window_, i, topleft_x, kGameInfo[i - topleft_y - kGameMainTitleHeight - 1]);
    }

    Core::logMessage("The game info has been initialized.", 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);

    wrefresh(main_window_);
}


void Cli::GamePanel::run() {
    initScreen();
    while (true) {

    }
}


