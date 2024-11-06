#include "core.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <thread>

void Core::logMessage(const std::string &message, Core::LogLocation loc, Core::LogType type) {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S") 
       << std::setw(3) << std::setfill('0') << ms.count();
    std::string loc_str = (loc == Core::LogLocation::kCore) ? "CORE"
                        : (loc == Core::LogLocation::kCli)  ? "CLI"
                        : (loc == Core::LogLocation::kGui)  ? "GUI"
                        : "UNKNOWN";
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

unsigned int Core::Command::kCmdCount = 0;
std::array<std::string, 8> Core::Command::kAllCmd = {
    "inbox", "outbox", "add", "sub",
    "copyto", "copyfrom", "jump", "jumpifzero"
};

void Core::Command::appendToList(const std::string& name, int index) {
    kCmdCount++;
    list_.emplace_back(name, index);
    Core::logMessage("Pass command name and operated index of "
                     "vacant from class `Core::Command` to class "
                     "`Core::Command::SingleCommand`.",
                     Core::LogLocation::kCore,
                     Core::LogType::kInfo);
}

void Core::Robot::initCommandList(const std::string& name, int index) {
    cmd_.appendToList(name, index);
    Core::logMessage("Pass command name and operated index of "
                     "vacant from class `Core::Robot` to class "
                     "`Core::Command`.", 
                     Core::LogLocation::kCore, 
                     Core::LogType::kInfo);
}

void Core::Game::initialize(std::vector<std::string>& a,
                            std::vector<int>& ps,
                            std::vector<int>& ns,
                            std::vector<std::pair<std::string, int>>& cmd,
                            int vs) {
    vac_size_ = vs;
    for (int i = 1; i <= vs; ++i) {
        game_vacant_.seq_.push_back(0);
        game_vacant_.seq_empty_.push_back(true);
    }
    game_vacant_.seq_.resize(vs);
    available_cmd_ = std::move(a);
    for (const auto& str : available_cmd_) {
        if (std::find(Core::Command::kAllCmd.begin(), Core::Command::kAllCmd.end(), str) == Core::Command::kAllCmd.end()) {
            Core::logMessage("Unknown command `" + str + "`, please check "
                             "the entire supported command list by typing key `?`.", 
                             Core::LogLocation::kCore, 
                             Core::LogType::kError);
            game_state_ = false;
            return;
        }
    }
    provided_seq_ = std::move(ps);
    needed_seq_ = std::move(ns);
    for (const auto& e : provided_seq_) {
        game_input_.seq_.push(e);
    }
    for (auto it = cmd.begin(); it < cmd.end(); ++it) {
        auto& [name, index] = *it;
        if (std::find(available_cmd_.begin(), available_cmd_.end(), name) == available_cmd_.end()) {
            std::cout << "Error on instruction " + std::to_string(it - cmd.begin()) << std::endl;
            Core::logMessage("Command ID " + std::to_string(it - cmd.begin() + 1) + 
                             " : Unkown command `" + name + "`, please "
                             "check the available command list by typing "
                             "key `?`.", 
                             Core::LogLocation::kCore, 
                             Core::LogType::kError);
            game_state_ = false;
            return;
        }
        game_robot_.initCommandList(name, index);
    }
    Core::logMessage("Initialize the following variable : `Core::"
                     "Game::[vac_size_ | game_vacant_ | available_"
                     "cmd_ | provided_seq_ | needed_seq_ | game_input_ | "
                     "game_robot_ ]`", 
                     Core::LogLocation::kCore, 
                     Core::LogType::kInfo);
}

bool Core::Command::checkOpindexSurplus() {
    if (list_[ref_].vacant_index_ != Core::Command::SingleCommand::kNullVacant) {
        game_->setState(false);
        std::cout << "Error on instruction " + std::to_string(ref_ + 1) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Surplus operated vacant index, the "
                         "command `" + list_[ref_].cmd_name_ + "` doesn't need an operated "
                         "vacant index.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else {
        return false;
    }
}

bool Core::Command::checkOpindexInvalid() {
    if (list_[ref_].vacant_index_ >= vacant_->seq_.size()) {
        game_->setState(false);
        std::cout << "Error on instruction " + std::to_string(ref_ + 1) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Invalid operated vacant index, index (" +
                         std::to_string(list_[ref_].vacant_index_) + 
                         ") is greater than vacant size (" +
                         std::to_string(vacant_->seq_.size()) + ".", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else if (list_[ref_].vacant_index_ < 0) {
        game_->setState(false);
        std::cout << "Error on instruction " + std::to_string(ref_ + 1) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Invalid operated vacant index, index (" +
                         std::to_string(list_[ref_].vacant_index_) + 
                         ") is less than vacant size (" +
                         std::to_string(vacant_->seq_.size()) + ".", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else if ((list_[ref_].cmd_name_ == "add"
             || list_[ref_].cmd_name_ == "sub"
             || list_[ref_].cmd_name_ == "copyfrom")
             && vacant_->seq_empty_[list_[ref_].vacant_index_]) {
        game_->setState(false);
        std::cout << "Error on instruction " + std::to_string(ref_ + 1) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : The operated vacant doesn't store any box.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else {
        return false;
    }
}

bool Core::Command::checkHandboxEmpty() {
    if (owner_->isEmpty()) {
        game_->setState(false);
        std::cout << "Error on instruction " + std::to_string(ref_ + 1) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Robot doesn't take any box, but the command "
                         "`" + list_[ref_].cmd_name_ + "` means put the handbox down.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else {
        return false;
    }
}

bool Core::Command::checkCmindexInvalid() {
    if (ref_ >= list_.size()) {
        game_->setState(false);
        std::cout << "Error on instrcution " + std::to_string(ref_ + 1) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : This command jumps out of the end of command list.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else if (ref_ < 0) {
        game_->setState(false);
        std::cout << "Error on instrcution " + std::to_string(ref_ + 1) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : This command jumps out of the begin of command list.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else {
        return false;
    }
}

void Core::Command::runRefCommand() {
    if (ref_ == list_.size()) {
        game_->setState(false);
        Core::logMessage("All command has been executed.",
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        return;
    }
    int index = std::find(kAllCmd.begin(), kAllCmd.end(), list_[ref_].cmd_name_) - kAllCmd.begin();
    switch (index) {
    case 0 : // "inbox"
        if (checkOpindexSurplus()) return;
        owner_->setValue(input_->seq_.front());
        owner_->setState(false);
        input_->seq_.pop();
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Robot takes the box (value : " + 
                         std::to_string(owner_->getValue()) + ") from the input.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 1 : // "outbox"
        if (checkOpindexSurplus()) return;
        if (checkHandboxEmpty()) return;
        output_->seq_.push(owner_->getValue());
        owner_->setValue(Core::Robot::kEmptyHandbox);
        owner_->setState(true);
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Robot puts the handbox (value : " +
                         std::to_string(output_->seq_.back()) +
                         ") down on the output.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 2 : // "add"
        if (checkHandboxEmpty()) return;
        if (checkOpindexInvalid()) return;
        owner_->setValue(owner_->getValue() + vacant_->seq_[list_[ref_].vacant_index_]);
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Robot adds the number (" + 
                         std::to_string(vacant_->seq_[list_[ref_].vacant_index_]) +
                         ") of operated vacant index (" + 
                         std::to_string(list_[ref_].vacant_index_) +
                         "), and the handbox becomes " +
                         std::to_string(owner_->getValue()), 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 3 : // "sub"
        if (checkHandboxEmpty()) return;
        if (checkOpindexInvalid()) return;
        owner_->setValue(owner_->getValue() + vacant_->seq_[list_[ref_].vacant_index_]);
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Robot subs the number (" +
                         std::to_string(vacant_->seq_[list_[ref_].vacant_index_]) +
                         ") of operated vacant index (" + 
                         std::to_string(list_[ref_].vacant_index_) +
                         "), and the handbox becomes " +
                         std::to_string(owner_->getValue()), 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 4 : // "copyto"
        if (checkHandboxEmpty()) return;
        if (checkOpindexInvalid()) return;
        vacant_->seq_[list_[ref_].vacant_index_] = owner_->getValue();
        vacant_->seq_empty_[list_[ref_].vacant_index_] = false;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Robot copy its handbox to the number of operated vacant index (" +
                         std::to_string(list_[ref_].vacant_index_) + ").", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 5 : // "copyfrom"
        if (checkOpindexInvalid()) return;
        owner_->setValue(vacant_->seq_[list_[ref_].vacant_index_]);
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Robot copy from the number of operated vacant index (" +
                         std::to_string(list_[ref_].vacant_index_) +
                         ") to its handbox, now the handbox is "
                         + std::to_string(owner_->getValue()), 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 6 : // jump
        if (checkCmindexInvalid()) return;
        ref_ = list_[ref_].vacant_index_;
        Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                         " : Robot's current command jumps to the index " +
                         std::to_string(ref_ + 1) + " command", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 7 : // jumpifzero
        if (owner_->getValue() == 0) {
            if (checkHandboxEmpty()) return;
            if (checkCmindexInvalid()) return;
            ref_ = list_[ref_].vacant_index_;
            Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                             " : Robot's current command jumps to the index " +
                             std::to_string(ref_ + 1) + " command, because handbox is 0.", 
                             Core::LogLocation::kCore, 
                             Core::LogType::kInfo);
        } else {
            ref_++;
        }
    }
}

void Core::Game::runAll() {
    while (game_state_) {
        game_robot_.runRefCommand();
        std::this_thread::sleep_for(std::chrono::seconds(game_gap_));
    }
}