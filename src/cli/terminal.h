#ifndef TERMINAL_H
#define TERMINAL_H

#include "../core/core.h"

namespace Cli {

class GamePanel {
  private:
    constexpr static wchar_t game_title_[7][53] = {
        LR"*(  ______    _____     _____      ____    __      __ )*",
        LR"*( |  ___ \  / ____ \  |  __ \   / ____ \  \ \    / / )*",
        LR"*( | |  / / | /    \ | | |  \ \ | /    \ |  \ \  / /  )*",
        LR"*( | |_/ /  | |    | | | |__/ / | |    | |   \ \/ /   )*",
        LR"*( |  __ \  | |    | | |  __  \ | |    | |   / __ \   )*",
        LR"*( | |  \ \ | \____/ | | |__/ | | \____/ |  / /  \ \  )*",
        LR"*( |_|   \_\ \______/  |_____/   \______/  /_/    \_\ )*"
    };
    Core::Game* game_;
    char input_key_;

    void printEntranceMenu();

  public:
    GamePanel(Core::Game* g) : game_(g) {}
    ~GamePanel() = default;
    void run();
};

}

#endif