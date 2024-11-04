#ifndef CORE_H
#define CORE_H

#include <queue>
#include <vector>
#include <string>
#include <array>

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
    std::queue<int> seq_;
};

class Output {
  public:
    std::queue<int> seq_;
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
        int vacant_index_;
        unsigned int id_;
        SingleCommand(const std::string& cn, unsigned int id, int vi = kNullVacant)
            : cmd_name_(cn), id_(id), vacant_index_(vi) {}
        ~SingleCommand() = default;
    };
    static unsigned int kCmdCount;
    static std::array<std::string, 8> kAllCmd;

    Command(Game* g, Robot* o, Input* in, Output* out, Vacant* vac)
        : game_(g), owner_(o), input_(in), output_(out), vacant_(vac) {}
    ~Command() = default;
    void runRefCommand();
    void appendToList(const std::string& name, int index);

  private:
    
    std::vector<SingleCommand> list_;
    unsigned int ref_;
    Game* game_;
    Robot* owner_;
    Input* input_;
    Output* output_;
    Vacant* vacant_;

    bool checkOpindexSurplus();
    bool checkOpindexInvalid();
    bool checkHandboxEmpty();
    bool checkCmindexInvalid();
    
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
    int getValue() { return handbox_; }
    void setValue(int v) { handbox_ = v; }
    bool isEmpty() { return handbox_state_; }
    void setState(bool s) { handbox_state_ = s; }
    void initCommandList(const std::string& name, int index);
};

class Game {
  private:
    bool game_state_ = true;
    std::vector<std::string> available_cmd_;
    std::vector<int> provided_seq_, needed_seq_;
    int vac_size_;
    Robot game_robot_;
    Input game_input_;
    Output game_output_;
    Vacant game_vacant_;
  public:
    Game() : game_robot_(this, &game_input_, &game_output_, &game_vacant_) {}
    void initialize(std::vector<std::string>& a,
                    std::vector<int>& ps,
                    std::vector<int>& ns,
                    std::vector<std::pair<std::string, int>>& cmd,
                    int vs);
    bool getState() { return game_state_; }
    void setState(bool s) { game_state_ = s; }
};

}

#endif