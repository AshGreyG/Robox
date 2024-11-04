#include "core.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>

void Core::logMessage(const std::string &message, Core::LogLocation loc, Core::LogType type) {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H-:%M:%S") 
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
    Core::logMessage("Pass command name and operated index of  "
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
    game_vacant_.seq_.resize(vs);
    available_cmd_ = std::move(a);
    provided_seq_ = std::move(ps);
    needed_seq_ = std::move(ns);
    for (const auto& e : provided_seq_) {
        game_input_.seq_.push(e);
    }
    for (const auto& p : cmd) {
        auto& [name, index] = p;
        if (std::find(available_cmd_.begin(), available_cmd_.end(), name) == available_cmd_.end()) {
            Core::logMessage("Unkown command `" + name + "`, please "
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
    } else {
        return false;
    }
}

void Core::Command::runRefCommand() {
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
        break;
    case 1 : // "outbox"
        if (checkOpindexSurplus()) return;
        if (owner_->isEmpty()) {
            std::cout << "Error on instruction " + std::to_string(ref_ + 1) << std::endl;
            Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                             " : Robot doesn't take any box, but the command "
                             "`outbox` means put the handbox down.", 
                             Core::LogLocation::kCore, 
                             Core::LogType::kError);
            game_->setState(false);
            return;
        } else {
            output_->seq_.push(owner_->getValue());
            owner_->setValue(Core::Robot::kEmptyHandbox);
            owner_->setState(true);
            Core::logMessage("Command ID " + std::to_string(ref_ + 1) +
                             " : Robot puts the handbox (value : " +
                             std::to_string(output_->seq_.back()) +
                             ") down on the output.", 
                             Core::LogLocation::kCore, 
                             Core::LogType::kInfo);
        }
        break;
    case 2 : // "add"
        if (checkOpindexInvalid()) return;
        break;
    }
}