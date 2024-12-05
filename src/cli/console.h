#ifndef TERMINAL_H
#define TERMINAL_H

#include <core/core.h>
#include <ncurses.h>

namespace Cli {

class GamePanel {
  private:
    // enum Color {
    //     kBlack,         // 0
    //     kBlue,          // 1
    //     kGreen,         // 2
    //     kCyan,          // 3
    //     kRed,           // 4
    //     kMagenta,       // 5
    //     kBrown,         // 6
    //     kLightGrey,     // 7
    //     kDarkGrey,      // 8
    //     kLightBlue,     // 9
    //     kLightGreen,    // 10 A
    //     kLightCyan,     // 11 B
    //     kLightRed,      // 12 C
    //     kLightMagenta,  // 13 D
    //     kYellow,        // 14 E
    //     kWhite          // 15 F
    // };
    constexpr static wchar_t kGameMainTitle[7][53] = {
        LR"*(  ______    _____     _____      ____    __      __ )*",
        LR"*( |  ___ \  / ____ \  |  __ \   / ____ \  \ \    / / )*",
        LR"*( | |  / / | /    \ | | |  \ \ | /    \ |  \ \  / /  )*",
        LR"*( | |_/ /  | |    | | | |__/ / | |    | |   \ \/ /   )*",
        LR"*( |  __ \  | |    | | |  __  \ | |    | |   / __ \   )*",
        LR"*( | |  \ \ | \____/ | | |__/ | | \____/ |  / /  \ \  )*",
        LR"*( |_|   \_\ \______/  |_____/   \______/  /_/    \_\ )*"
    };

    constexpr static wchar_t kGamePausedTitle[7][50] = {
        LR"*(  _____                                          )*",
        LR"*( |  __ \                                         )*",
        LR"*( | |  \ \                                     _  )*",
        LR"*( | |__/ / _____   _    _   _____   ____    __| | )*",
        LR"*( |  ___/ / __  \ | |  | | / ____| / __ \  / _  | )*",
        LR"*( | |    | (__| | | |__| | \____ \ \  __/ | (_| | )*",
        LR"*( |_|     \_____|  \____/  |_____/  \___|  \____| )*"
    };

    constexpr static wchar_t kGameInfo[][] = {

    };
    
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