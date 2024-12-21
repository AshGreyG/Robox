#include <core/core.h>
#include "console.h"

#include <ncurses.h>

#include <cmath>
#include <fstream>
#include <string>

unsigned int Cli::GamePanel::kPauseTimes = 0;

/** 
 * @program:     Cli::GamePanel::initScreen
 * @description: This function is to put the GameMainTitle and GameInfo to the MainWindow
 */
void Cli::GamePanel::initScreen() {
    setlocale(LC_ALL, "");
    initscr();
    raw();
    noecho();
    cbreak();

    target_window_ = newwin(kGameTargetWindowHeight,
                            kGameTargetWindowWidth,
                            0,
                            0);

    box(target_window_, 0, 0);

    main_window_ = newwin(kGameMainWindowHeight, 
                          kGameMainWindowWidth, 
                          kGameTargetWindowHeight, 
                          0);

    box(main_window_, 0, 0);

    Core::logMessage("The game MainWindow panel has been initialized.", 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);

    command_window_ = newwin(kGameCommandWindowHeight,
                             kGameCommandWindowWidth,
                             0,
                             kGameMainWindowWidth + 1);

    box(command_window_, 0, 0);

    Core::logMessage("The game CommandWindow panel has been initialized.", 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);

    status_window_ = newwin(kGameStatusWindowHeight,
                            kGameStatusWindowWidth,
                            kGameTargetWindowHeight + kGameMainWindowHeight,
                            0);

    box(status_window_, 0, 0);

    Core::logMessage("The game StatusWindow panel has been initialized.", 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);

    int topleft_x = (kGameMainWindowWidth - kGameMainTitleWidth) / 2;
    int topleft_y = (kGameMainWindowHeight - (kGameMainTitleHeight + kGameInfoHeight + 1)) / 2;

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

    wrefresh(target_window_);
    wrefresh(main_window_);
    wrefresh(command_window_);
    wrefresh(status_window_);
}

/**
 * @program:     Cli::GamePanel::showSelect
 * @description: This function is to show the select level panel
 */
void Cli::GamePanel::showSelect() {
    Core::logMessage("showSelect function clears the MainWindow content.", 
                     Core::LogLocation::kCli,
                     Core::LogType::kInfo);

    werase(main_window_);
    box(main_window_, 0, 0);
 
    unsigned int width = kTotalLevel > kMaxLevelOneLine
                       ? kMaxLevelOneLine * kGameSelectLevelWidth + kMaxLevelOneLine - 1
                       : kTotalLevel * kGameSelectLevelWidth + kTotalLevel - 1;

    // if kTotalLevel > kMaxLevelOneLine, then there will be over two lines of levels
    // the width of select panel is 
    //
    // +------+   +------+   +------+   +------+   +------+ 
    // |      |   |      |   |      |   |      |   |      | 
    // +------+   +------+   +------+   +------+   +------+ 
    //
    // but if kTotalLevel <= kMaxLevel, then there will be only one line of level

    unsigned int height = (kTotalLevel / kMaxLevelOneLine + 1) * kGameSelectLevelHeight + kTotalLevel / kMaxLevelOneLine;

    // the height of select panel is
    //
    // +------+   +------+   +------+   +------+   +------+ 
    // |      |   |      |   |      |   |      |   |      | 
    // +------+   +------+   +------+   +------+   +------+ 
    // 
    // +------+ 
    // |      | 
    // +------+ 

    unsigned int topleft_x = (kGameMainWindowWidth - width) / 2;
    unsigned int topleft_y = (kGameMainWindowHeight - height) / 2;

    // calculate the topleft corner coordinate of select level panel

    Core::logMessage("The width and height of select panel have been initialized, "
                     "width = " + std::to_string(width) + ", "
                     "height = " + std::to_string(height), 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);

    for (int i = 1; i <= kTotalLevel; ++i) {
        unsigned int current_top_left_x = topleft_x + ((i - 1) % kMaxLevelOneLine) * (kGameSelectLevelWidth + 1);
        unsigned int current_top_left_y = topleft_y + ((i - 1) / kMaxLevelOneLine) * (kGameSelectLevelHeight + 1);

        // calculate the topleft corner coordinate of currently drawing level

        for (int j = 1; j <= kGameSelectLevelHeight; ++j) {
        mvwaddwstr(main_window_, 
                   current_top_left_y + j - 1, 
                   current_top_left_x,
                   kGameSelectLevel[j - 1]);
        }

        wchar_t is_level_completed = (i <= kCurrentLevel) ? kCompleted : kUncompleted; 

        // if the level drawing now player has completed, then use '@' to wrap it, if not, use 'X'

        std::wstring level_str = std::wstring(1, is_level_completed)
                               + L" "
                               + std::to_wstring(i)
                               + L" "
                               + std::wstring(1, is_level_completed);

        wchar_t* level_wchar = level_str.data();

        mvwaddwstr(main_window_,
                   current_top_left_y + 1,
                   current_top_left_x + kGameSelectLevelWidth / 2 - level_str.length() / 2,
                   level_wchar);

        Core::logMessage("level panel " + std::to_string(i) + 
                         " has been drawn successfully, it's " +
                         ((is_level_completed == kCompleted) ? "completed" : "uncompleted") + ".", 
                         Core::LogLocation::kCli, 
                         Core::LogType::kInfo);
    }



    wrefresh(main_window_);

}

/**
 * @program:    
 * @description: This function is to show the 
 */
void Cli::GamePanel::showMain() {
    
}

/**
 * @program:     Cli::GamePanel::showPaused
 * @description: This function is to show the pause tab.
 */
void Cli::GamePanel::showPaused() {
    Core::logMessage("showPaused function clears the MainWindow content.", 
                     Core::LogLocation::kCli,
                     Core::LogType::kInfo);

    werase(main_window_);
    box(main_window_, 0, 0);

    int topleft_x = (kGameMainWindowWidth - kGamePausedTitleWidth) / 2;
    int topleft_y = (kGameMainWindowHeight - kGamePausedTitleHeight) / 2;

    // gametitle_topleft_x : The x coordinates of topleft corner of PausedTitle
    // gametitle_topleft_y : The y coordinates of topleft corner of PausedTitle
    
    for (int i = topleft_y; i <= topleft_y + kGamePausedTitleHeight - 1; ++i) {
        mvwaddwstr(main_window_, i, topleft_x, kGamePausedTitle[i - topleft_y]);
    }

    wrefresh(main_window_);

    Core::logMessage("The paused title has been initialized", 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);
}

void Cli::GamePanel::run() {
    std::filesystem::create_directory(config);
    std::ifstream current_config_file(config / kCurentConfigFile);

    if (current_config_file.is_open()) {
        current_config_file >> kCurrentLevel;
        Core::logMessage("File config/current.config has been loaded successfully. "
                         "And the current level is " + std::to_string(kCurrentLevel), 
                         Core::LogLocation::kCli, 
                         Core::LogType::kInfo);
    } else {
        Core::logMessage("Fail to load file config/current.config", 
                         Core::LogLocation::kCli, 
                         Core::LogType::kError);
    }

    initScreen();

    char ch;

    if ((ch = wgetch(command_window_)) != '\0') {
        showSelect();
    }

    // Press any key to continue

    while ((ch = wgetch(command_window_)) != 'q') {

        // notice here needs to use wgetch(WINDOW *) rather than getch(), otherwise
        // the window will get empty. All w* functions mean window*.

        switch (ch) {
        case 'p' :
            if (kPauseTimes % 2 == 0) {
                showPaused();   // When kPauseTimes is even, we need to show the paused title
            } else {
                showMain();
            }
            wrefresh(main_window_);
            break;
        }
    }

    Core::logMessage("Player press q to quit.", 
                     Core::LogLocation::kCli, 
                     Core::LogType::kInfo);

    endwin();
}