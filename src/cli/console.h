#ifndef TERMINAL_H
#define TERMINAL_H

#include <core/core.h>

#include <ncurses.h>

#include <filesystem>
#include <string>

namespace Cli {

constexpr static std::string config_directory = "config";
constexpr static std::string level_config_name = "current.config";
const static std::filesystem::path config(config_directory);

class GamePanel {
  private:

    constexpr static wchar_t kGameMainTitle[7][53] = {
        LR"*(  ______    _____     _____      ____    __      __ )*",
        LR"*( |  ___ \  / ____ \  |  __ \   / ____ \  \ \    / / )*",
        LR"*( | |  / / | /    \ | | |  \ \ | /    \ |  \ \  / /  )*",
        LR"*( | |_/ /  | |    | | | |__/ / | |    | |   \ \/ /   )*",
        LR"*( |  __ \  | |    | | |  __  \ | |    | |   / __ \   )*",
        LR"*( | |  \ \ | \____/ | | |__/ | | \____/ |  / /  \ \  )*",
        LR"*( |_|   \_\ \______/  |_____/   \______/  /_/    \_\ )*"
    };
    constexpr static short kGameMainTitleWidth  = 53;
    constexpr static short kGameMainTitleHeight = 7;

    // kGameMainTitle is to show when the game starts

    constexpr static wchar_t kGamePausedTitle[7][50] = {
        LR"*(  _____                                          )*",
        LR"*( |  __ \                                         )*",
        LR"*( | |  \ \                                     _  )*",
        LR"*( | |__/ / _____   _    _   _____   ____    __| | )*",
        LR"*( |  ___/ / __  \ | |  | | / ____| / __ \  / _  | )*",
        LR"*( | |    | (__| | | |__| | \____ \ \  __/ | (_| | )*",
        LR"*( |_|     \_____|  \____/  |_____/  \___|  \____| )*"
    };
    constexpr static short kGamePausedTitleWidth  = 50;
    constexpr static short kGamePausedTitleHeight = 7;

    // kGamePausedTitle is to show when the game is paused by player

    constexpr static wchar_t kGameInfo[9][53] = {
        LR"*( Press `?` to display this info-panel again.        )*",
        LR"*( Press `i` to insert the next command.              )*",
        LR"*( Press `o` to open a command file from local.       )*",
        LR"*( Press `p` to pause the game.                       )*",
        LR"*( Press `w` to write the commands to a new file.     )*",
        LR"*( Press `r` to restart the game.                     )*",
        LR"*( Press `d` to delete the target command and restart.)*",
        LR"*( Press `q` to quit the game.                        )*",
        LR"*( Press **ANY KEY** to start the game...             )*"
    };
    constexpr static short kGameInfoWidth  = 53;
    constexpr static short kGameInfoHeight = 9;

    constexpr static unsigned int kTotalLevel = 9;
    constexpr static unsigned int kMaxLevelOneLine = 6;
    constexpr static wchar_t kGameSelectLevel[3][15] = {
      LR"*( +----------+ )*",
      LR"*( |          | )*",
      LR"*( +----------+ )*",
    };
    constexpr static short kGameSelectLevelWidth  = 15;
    constexpr static short kGameSelectLevelHeight = 3;

    constexpr static short kGameTargetWindowWidth  = 110;
    constexpr static short kGameTargetWindowHeight = 3;
    
    constexpr static short kGameMainWindowWidth  = 110;
    constexpr static short kGameMainWindowHeight = 30;

    constexpr static short kGameCommandWindowWidth  = 20;
    constexpr static short kGameCommandWindowHeight = 30;

    static unsigned int kPauseTimes;
    unsigned int kCurrentLevel;
    
    // Pause times is used to judge to show pause title or game panel
    // When kPauseTimes is even, we need to show pause title
    // When lPauseTimes is odd, we need to show game panel

    Core::Game* game_;
    WINDOW* target_window_;
    WINDOW* main_window_;
    WINDOW* command_window_;
    WINDOW* status_window_;

    char input_key_;

    void initScreen();
    void showSelect();
    void showMain();
    void showPaused();
    void showHelp();

  public:
    GamePanel(Core::Game* g) : game_(g) {}
    ~GamePanel() = default;
    void run();
};

}

#endif