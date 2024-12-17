#include <vector>
#include <string>
#include <array>
#include <deque>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

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

// forward declaration

// input                       output
// ┌─┐                          ┌─┐
// ├─┤           🤖             ├─┤
// ├─┤                          ├─┤
// ├─┤                          ├─┤
// ├─┤                          ├─┤
// ├─┤                          ├─┤
// ├─┤                          ├─┤
// └─┘                          └─┘
//       ┌─┬─┬─┬─┬─┬─┬─┬─┬─┐
//       └─┴─┴─┴─┴─┴─┴─┴─┴─┘ vacant

/**
 * @author: AshGrey
 * @date:   2024-12-05
 */
class Input {
  public:
    std::deque<int> seq_;
};

/**
 * @author: AshGrey
 * @date:   2024-12-05
 */
class Output {
  public:
    std::deque<int> seq_;
};

/**
 * @author: AshGrey
 * @date:   2024-12-05
 */
class Vacant {
  public:
    std::vector<bool> seq_empty_; // Should set defaultly true, means empty
    std::vector<int> seq_;
};

/**
 * @author: AshGrey
 * @date:   2024-12-05
 */
class Command {
  public:
    class SingleCommand {
      public:
        static constexpr int kNullVacant = -1;  // kNullVacant means the command doesn't have the vacant index parameter
        std::string cmd_name_;                  // The name of command, including
                                                // inbox, outbox, add, sub, copyto, copyfrom, jump, jumpifzero
        int target_index_;                      // Target index (or vacant index) is the target block of command
        SingleCommand(const std::string& cn, int vi = kNullVacant)
            : cmd_name_(cn), target_index_(vi) {}
        ~SingleCommand() = default;
    };
    static unsigned int kCmdCount;
    static std::array<std::string, 8> kAllCmd;  // inbox, outbox, add, sub, copyto, copyfrom, jump, jumpifzero

    Command(Game* g, Robot* o, Input* in, Output* out, Vacant* vac)
        : game_( g )
        , owner_( o )
        , input_( in )
        , output_( out )
        , vacant_( vac ) {}

    ~Command() = default;

    void runRefCommand();
    void appendToList(const std::string& name, int index);
    int getRef() { return ref_; }
    void setRef(int r) { ref_ = r; }

  private:
    
    std::vector<SingleCommand> list_; // A list of all command
    unsigned int ref_ = 1;            // Referrence to the command which is executed now

    Game* game_;
    Robot* owner_;
    Input* input_;
    Output* output_;
    Vacant* vacant_;

    bool checkOpindexSurplus(); // `Opindex` stands for `Operated Index`
    bool checkOpindexInvalid();
    bool checkHandboxEmpty();
    bool checkCmindexInvalid(); // `Cmindex` stands for `Command Index`
    bool checkInputEmpty();
    bool checkVacantEmpty();

};

/**
 * @author: AshGrey
 * @date:   2024-12-05
 */
class Robot {
  private:
    Command cmd_;           // The inner command of Robot
    int handbox_;           // The num of box which is taken by Robot now
    bool handbox_state_;    // FALSE when hand is empty, TRUE when not

  public:
    static constexpr int kEmptyHandbox = 0;
    
    Robot(Game* g, Input* in, Output* out, Vacant* vac, bool hs = true)
        : handbox_( kEmptyHandbox )
        , handbox_state_( hs )
        , cmd_(g, this, in, out, vac) {}
    
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

/**
 * @author: AshGrey
 * @date:   2024-12-05
 */
class Game {
  private:
    bool game_state_ = true;    // TRUE means game is running correctly, FALSE when not
    bool error_state_ = false;  // TRUE means there is a happened error, FALSE when not
    int game_gap_ = 0;          // Game gap sets the gap between one command and the next command
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

//======================================================//
// Copyright (c) 2024 AshGrey. All rights reserved.     //
// Released under MIT license as described in the file  //
// LICENSE.                                             //
// Author: AshGrey (He Yuhui)                           //
//                                                      //
// This file is the implement of header file `core.h`   //
//======================================================//


/**
 * @program:     Core::logMessage
 * @description: This is a logger function, the format is "level [time] #(location) message"
 * @message:     The detail of log message
 * @loc:         The location where message comes from, there are three locations : core, cli and gui
 * @type:        The type (or level) of message, there are two types : Info < Error
 */
void Core::logMessage(const std::string& message, Core::LogLocation loc, Core::LogType type) {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S:") 
       << std::setw(3) << std::setfill('0') << ms.count();

    // Get the timestamp

    std::string loc_str = (loc == Core::LogLocation::kCore) ? "CORE"
                        : (loc == Core::LogLocation::kCli)  ? "CLI"
                        : (loc == Core::LogLocation::kGui)  ? "GUI"
                        : "UNKNOWN";

    // Get the location string : Core::LogLocation::kCore => "CORE"
    //                           Core::LogLocation::kCli  => "CLI"
    //                           Core::LogLocation::kGui  => "GUI"

    if (type == Core::LogType::kInfo) {
        std::clog << "Info  ["
                  << ss.str() 
                  << "] #("
                  << loc_str
                  << ") "
                  << message
                  << std::endl;
    } else if (type == Core::LogType::kError) {
        std::clog << "Error ["
                  << ss.str() 
                  << "] #("
                  << loc_str
                  << ") "
                  << message
                  << std::endl;
    }
}

unsigned int Core::Command::kCmdCount = 0;              // Counts from 0

std::array<std::string, 8> Core::Command::kAllCmd = {
    "inbox", "outbox", "add", "sub",
    "copyto", "copyfrom", "jump", "jumpifzero"
};

/**
 * @program:     Core::Command::appendToList
 * @description: This function passes the command name and vacant index to the inner class Command
 * @name:        The name of command, see Core::Command::kAllCmd
 * @index:       The index of vacant
 */
void Core::Command::appendToList(const std::string& name, int index) {
    kCmdCount++;
    list_.emplace_back(name, index);
}

/**
 * @program:     Core::Robot::initCommandList
 * @description: This function passes the command name and vacant index to the Core::Command::appendToList
 * @name:        The name of command, see Core::Command::kAllCmd
 * @index:       The index of vacant
 */
void Core::Robot::initCommandList(const std::string& name, int index) {
    cmd_.appendToList(name, index);
}

/**
 * @program:     Core::Game::initialize
 * @description: This function initialize the available commands, provided sequence,  
                 needed sequence and commans and vacant size to the game private members.
 * @a:           Available commands, it will display on the top of Game UI
 * @ps:          Provided sequence
 * @ns:          Needed sequence
 * @cmd:         Commands, they are the pair of command name and vacant index (default kNullVacant)
 * @vs:          The size of vacant
 */
void Core::Game::initialize(std::vector<std::string>& a,
                            std::vector<int>& ps,
                            std::vector<int>& ns,
                            std::vector<std::pair<std::string, int>>& cmd,
                            int vs) {
    vac_size_ = vs;                                 // pass the size of vacant
    for (int i = 1; i <= vs; ++i) {
        game_vacant_.seq_.push_back(0);             // Default num is 0
        game_vacant_.seq_empty_.push_back(true);    // Default is empty
    }
    game_vacant_.seq_.resize(vs);                   // The vacant resizes to the given size
    available_cmd_ = std::move(a);                  // Pass the available command to the private member
    provided_seq_ = std::move(ps);                  // Pass the provided sequence to the private member
    needed_seq_ = std::move(ns);                    // Pass the needed sequence to the private member

    for (const auto& str : available_cmd_) {
        if (std::find(Core::Command::kAllCmd.begin(), Core::Command::kAllCmd.end(), str) == Core::Command::kAllCmd.end()) {

            // This condition means the str is illegal, so we need error here and return

            game_state_ = false;
            error_state_ = true;
            return;
        }
    }

    for (const auto& e : provided_seq_) {
        game_input_.seq_.push_back(e);
    }

    for (auto it = cmd.begin(); it < cmd.end(); ++it) {
        auto name = it->first;
        auto index = it->second;
        if (std::find(available_cmd_.begin(), available_cmd_.end(), name) == available_cmd_.end()) {

            // This condition means the command name is not in the available commands, 
            // so we need error here and return

            std::cout << "Error on instruction " + std::to_string(it - cmd.begin() + 1) << std::endl;
            game_state_ = false;
            error_state_ = true;
            return;
        }
        game_robot_.initCommandList(name, index);
    }

}

/**
 * @program:     Core::Command::checkOpindexSurplus
 * @description: This function is to check if the operated index (the operated 
                 vacant index of the command that is executed now) is surplus
 */
bool Core::Command::checkOpindexSurplus() {
    if (list_[ref_ - 1].target_index_ != Core::Command::SingleCommand::kNullVacant) {

        // Notice referrence begins from **1** !!!, only use this function in no-parameter
        // command. If the no-parameter command has target index, then we need to error here

        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        return true;
    } else {
        return false;
    }
}

/**
 * @program:     Core::Command::checkOpindexInvalid
 * @description: This function is to check if the operated index (the operated
                 vacant index of the command that is executed now) is invalid
 */
bool Core::Command::checkOpindexInvalid() {
    if (list_[ref_ - 1].target_index_ >= vacant_->seq_.size()) {

        // vacant index is greater than or equal to the size of vacant
        // notice that the sequence of vacant counts from 0 !!!

        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        return true;
    } else if (list_[ref_ - 1].target_index_ < 0) {

        // vacant index is less than 0

        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        return true;
    } else if ((list_[ref_ - 1].cmd_name_ == "add"
             || list_[ref_ - 1].cmd_name_ == "sub"
             || list_[ref_ - 1].cmd_name_ == "copyfrom")
             && vacant_->seq_empty_[list_[ref_ - 1].target_index_ - 1]) {
        
        // The target vacant is empty
        // for command "add", "sub" and "copyfrom", the target vacant cannot be empty!

        game_->setGameState(false);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        return true;
    } else {
        return false;
    }
}

/**
 * @program:     Core::Command::checkHandboxEmpty
 * @description: This function is to check if there is a box in robot's hand.
                 If there is, this function will return TRUE, otherwise FALSE
 */
bool Core::Command::checkHandboxEmpty() {
    if (owner_->isEmpty()) {
        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        return true;
    } else {
        return false;
    }
}

bool Core::Command::checkCmindexInvalid() {
    if (ref_ > list_.size()) {
        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instrcution " + std::to_string(ref_) << std::endl;
        return true;
    } else if (ref_ <= 0) {
        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instrcution " + std::to_string(ref_) << std::endl;
        return true;
    } else {
        return false;
    }
}

/**
 * @program:     Core::Command::checkInputEmpty
 * @description: This function is to check if the input is empty.
 */
bool Core::Command::checkInputEmpty() {
    if (input_->seq_.size() == 0) {
        game_->setGameState(false);
        return true;
    } else {
        return false;
    }
}

/**
 * @program:     Core::Command::checkVacantEmpty
 * @description: This function is to check if the target vacant is empty
 */
bool Core::Command::checkVacantEmpty() {
    if (!vacant_->seq_empty_[list_[ref_ - 1].target_index_]) {
        game_->setErrorState(true);
        game_->setGameState(false);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        return true;    // Empty
    } else {
        return false;   // Not empty
    }
}

/**
 * @program:     Core::Command::runRefCommand
 * @description: This function is to run the referrence command
 */
void Core::Command::runRefCommand() {
    if (ref_ == list_.size() + 1) {

        // All the commands have been executed, so we set the game state false

        game_->setGameState(false);
        return;
    }
    
    int index = std::find(kAllCmd.begin(), kAllCmd.end(), list_[ref_ - 1].cmd_name_) - kAllCmd.begin();

    // Get the index of this command

    switch (index) {
    case 0 : // "inbox"
        if (checkOpindexSurplus()) return;  // Check if vacant index is given, "inbox" command doesn't need parameter
        if (checkInputEmpty()) return;      // Check if input is empty, if true, then the game ends

        owner_->setValue(input_->seq_.front());
        owner_->setState(false);
        input_->seq_.pop_front();
        ref_++;
        break;
    case 1 : // "outbox"
        if (checkOpindexSurplus()) return;  // Check if vacant index is given. "outbox" command doesn't need parameter
        if (checkHandboxEmpty()) return;    // Check if the handbox is empty. "outbox" command needs the robot holds a box

        output_->seq_.push_back(owner_->getValue());    // Put the box to output
        owner_->setValue(Core::Robot::kEmptyHandbox);   // Robot doesn't hold this box anymore
        owner_->setState(true);
        ref_++;
        break;
    case 2 : // "add"
        if (checkHandboxEmpty()) return;    // Check if the handbox is empty. "add" command needs the robot holding a box
        if (checkOpindexInvalid()) return;  // Check if vacant index is invalid.
        owner_->setValue(owner_->getValue() + vacant_->seq_[list_[ref_ - 1].target_index_]);
        //               ^^^^^^^^^^^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //                 Handbox Value                      Target Vacant Value
        //                                    Notice command referrence begins from **1**
        //                                    Vacant index begins from **0**

        ref_++;
        break;
    case 3 : // "sub"
        if (checkHandboxEmpty()) return;    // Check if the handbox is empty. "sub" command needs the robot holding a box
        if (checkOpindexInvalid()) return;  // Check if vacant index is invalid
        owner_->setValue(owner_->getValue() - vacant_->seq_[list_[ref_ - 1].target_index_]);
        //               ^^^^^^^^^^^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //                 Handbox Value                    Target Vacant Value
        //                                    Notice command referrence begins from **1**
        //                                    Vacant index begins from **0**

        ref_++;
        break;
    case 4 : // "copyto"
        if (checkHandboxEmpty()) return;    // Check if the handbox is empty. "copyto" command needs the robot holding a box
        if (checkOpindexInvalid()) return;  // Check if vacant index is invalid
        vacant_->seq_[list_[ref_ - 1].target_index_] = owner_->getValue();  // Put the box to the vacant
        vacant_->seq_empty_[list_[ref_ - 1].target_index_] = false;         // Set the vacant state to not-empty
        ref_++;
        break;
    case 5 : // "copyfrom"
        if (checkOpindexInvalid()) return;  // Check if vacant index is invalid
        if (checkVacantEmpty()) return;     // Check if the vacant is empty
        owner_->setValue(vacant_->seq_[list_[ref_ - 1].target_index_ - 1]);
        ref_++;
        break;
    case 6 : // jump
        if (checkCmindexInvalid()) return;  // Check if there is the target command ID
        ref_ = list_[ref_ - 1].target_index_; 
        break;
    case 7 : // jumpifzero
        if (owner_->getValue() == 0) {
            if (checkHandboxEmpty()) return;    // Check if there is any box held by robot
            if (checkCmindexInvalid()) return;  // Check if there is the target command ID
            ref_ = list_[ref_ - 1].target_index_;
        } else {
            ref_++;
        }
    }
}

/**
 * @program:
 * @description: This function checks the game state
 */
void Core::Game::check() {
    if (error_state_) return;

    bool f = true;
    unsigned int count = 0;
    f = (game_output_.seq_.size() == needed_seq_.size());

    while (game_output_.seq_.size() != 0) {
        int e = game_output_.seq_.front();
        game_output_.seq_.pop_front();
        if (e != needed_seq_[count]) {
            f = false;
            break;
        }
        count++;
    }

    // Check the game output == needed sequence

    if (f) {
        std::cout << "Success" << std::endl;
    } else {
        std::cout << "Fail" << std::endl;
    }
}

void Core::Game::runAll() {
    game_state_ = !error_state_;
    while (game_state_ && !error_state_) {
        game_robot_.runRefCommand();
        std::this_thread::sleep_for(std::chrono::seconds(game_gap_));
    }
    check();
}

void Core::Game::runTo(int target_ref) {
    game_state_ = !error_state_;
    while (game_state_ && error_state_) {
        game_robot_.runRefCommand();
        if (target_ref == game_robot_.getRef()) {
            game_state_ = false;
        }
        // std::this_thread::sleep_for(std::chrono::seconds(game_gap_));
    }
    check();
}

void Core::Game::restart() {
    game_state_ = true;
    error_state_ = false;
    game_robot_.setRef(1);
    game_vacant_.seq_.clear();
    game_vacant_.seq_empty_.clear();
    game_input_.seq_.clear();
    game_output_.seq_.clear();
    for (const auto& e : provided_seq_) {
        game_input_.seq_.push_back(e);
    }
}

int main () {
    return 0;
}