#ifndef CORE_H
#define CORE_H

#include <vector>
#include <string>
#include <array>
#include <deque>

namespace Core {

enum LogType {
    kInfo,
    kError
};

enum LogLocation {
    kCore,
    kCli,
    kGui
};

void logMessage(const std::string& message, LogLocation loc, LogType type);

class Robot;
class Game;

class Input {
  public:
    std::deque<int> seq_;
};

class Output {
  public:
    std::deque<int> seq_;
};

class Vacant {
  public:
    std::vector<bool> seq_empty_;
    std::vector<int> seq_;
};

class Command {
  public:
    class SingleCommand {
      public:
        static constexpr int kNullVacant = -1;
        std::string cmd_name_;
        int target_index_;
        SingleCommand(const std::string& cn, int vi = kNullVacant)
            : cmd_name_(cn), target_index_(vi) {}
        ~SingleCommand() = default;
    };
    static unsigned int kCmdCount;
    static std::array<std::string, 8> kAllCmd;

    Command(Game* g, Robot* o, Input* in, Output* out, Vacant* vac)
        : game_(g), owner_(o), input_(in), output_(out), vacant_(vac) {}
    ~Command() = default;
    void runRefCommand();
    void appendToList(const std::string& name, int index);
    int getRef() { return ref_; }
    void setRef(int r) { ref_ = r; }

  private:
    
    std::vector<SingleCommand> list_;
    unsigned int ref_ = 1;
    Game* game_;
    Robot* owner_;
    Input* input_;
    Output* output_;
    Vacant* vacant_;

    bool checkOpindexSurplus();
    bool checkOpindexInvalid();
    bool checkHandboxEmpty();
    bool checkCmindexInvalid();
    bool checkInputEmpty();
    
    // `Opindex` stands for `Operated Index`
    // `Cmindex` stands for `Command Index`

};

class Robot {
  private:
    Command cmd_;
    int handbox_;
    bool handbox_state_;
  public:
    static constexpr int kEmptyHandbox = 0;
    
    Robot(Game* g, Input* in, Output* out, Vacant* vac, bool hs = true)
        : handbox_(kEmptyHandbox), handbox_state_(hs), cmd_(g, this, in, out, vac) {}
    
    // Use `true`  to denote robot doesn't take any box in hand.
    // Use `false` to denote robot take a box.

    ~Robot() = default;

    // These functions are designed to interact with Game

    int getValue() { return handbox_; }
    void setValue(int v) { handbox_ = v; }
    bool isEmpty() { return handbox_state_; }
    void setState(bool s) { handbox_state_ = s; }

    // These functions are designed to pass the information of command list

    void runRefCommand() { cmd_.runRefCommand(); }
    int getRef() { return cmd_.getRef(); }
    void setRef(int r) { cmd_.setRef(r); }

    void initCommandList(const std::string& name, int index);
};

class Game {
  private:
    bool game_state_ = true;
    bool error_state_ = false;
    int game_gap_ = 0;
    std::vector<std::string> available_cmd_;
    std::vector<int> provided_seq_, needed_seq_;
    int vac_size_;
    Robot game_robot_;
    Input game_input_;
    Output game_output_;
    Vacant game_vacant_;

    void check();

  public:
    Game() : game_robot_(this, &game_input_, &game_output_, &game_vacant_) {}
    void initialize(std::vector<std::string>& a,
                    std::vector<int>& ps,
                    std::vector<int>& ns,
                    std::vector<std::pair<std::string, int>>& cmd,
                    int vs);
    bool getGameState() { return game_state_; }
    void setGameState(bool s) { game_state_ = s; }
    bool getErrorState() { return error_state_; }
    void setErrorState(bool e) { error_state_ = e; }
    int getGap() { return game_gap_; }
    void setGap(int v) { game_gap_ = v; }
    void runAll();
    void runTo(int target_ref);
    void pause() { game_state_ = false; }
    void restart();
};

}

#endif