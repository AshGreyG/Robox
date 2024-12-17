#ifndef TERMINAL_H
#define TERMINAL_H

#include <core/core.h>
#include <ncurses.h>

namespace Cli {

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

    constexpr static wchar_t kGameInfo[][53] = {
        LR"*( Press `?` to display this info-panel again.        )*",
        LR"*( Press `i` to insert the next command.              )*",
        LR"*( Press `o` to open a command file from local.       )*",
        LR"*( Press `p` to pause the game.                       )*",
        LR"*( Press `w` to write the commands to a new file.     )*",
        LR"*( Press `r` to restart the game.                     )*",
        LR"*( Press `d` to delete the target command and restart.)*",
        LR"*( Press **ANY KEY** to start the game...             )*"
    };
    constexpr static short kGameInfoWidth  = 53;
    constexpr static short kGameInfoHeight = 8;
    
    constexpr static short kGameConsoleWidth = 80;
    constexpr static short kGameCenterX = kGameConsoleWidth / 2;
    constexpr static short kGameConsoleHeight = 40;

    Core::Game* game_;
    WINDOW* main_window_;
    char input_key_;

    void initScreen();
    void getInput();
    void showPaused();
    void showHelp();

  public:
    GamePanel(Core::Game* g) : game_(g) {}
    ~GamePanel() = default;
    void run();
};

}

#endif