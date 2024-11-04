#ifndef CORE_H
#define CORE_H

#include <queue>
#include <vector>
#include <string>

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
    std::vector<int> seq_;
};

class Command {
  private:
    class SingleCommand {
      public:
        static const int kNullVacant = -1;
        std::string cmd_name_;
        int vacant_index_;
        unsigned int id_;
        SingleCommand(const std::string& cn, unsigned int id, int vi = kNullVacant)
            : cmd_name_(cn), id_(id), vacant_index_(vi) {}
        ~SingleCommand() = default;
    };

    std::vector<SingleCommand> list_;
    unsigned int ref_;
    Robot* owner_;
    Input* input_;
    Output* output_;
    Vacant* vacant_;

  public:
    static unsigned int kCmdCount;
    static std::vector<std::string> kAllCmd;

    Command(Robot* o, Input* in, Output* out, Vacant* vac)
        : owner_(o), input_(in), output_(out), vacant_(vac) {}
    ~Command() = default;
    void runRefCommand();
    void appendToList(const std::string& name, int index);
};

class Robot {
  private:
    Command cmd_;
    int handbox_;
    bool handbox_state_;
  public:
    static const int kEmptyHandbox = 0;
    
    Robot(Input* in, Output* out, Vacant* vac, bool hs = true)
        : handbox_(kEmptyHandbox), handbox_state_(hs), cmd_(this, in, out, vac) {}
    
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
  public:
    bool game_state_ = true;
    std::vector<std::string> available_cmd_;
    std::vector<int> provided_seq_, needed_seq_;
    int vac_size_;
    Robot game_robot_;
    Input game_input_;
    Output game_output_;
    Vacant game_vacant_;
    Game() : game_robot_(&game_input_, &game_output_, &game_vacant_) {}
    void initialize(std::vector<std::string>& a,
                    std::vector<int>& ps,
                    std::vector<int>& ns,
                    std::vector<std::pair<std::string, int>>& cmd,
                    int vs);
    bool getState() { return game_state_; }
};

}

#endif