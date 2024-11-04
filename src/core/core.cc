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
std::vector<std::string> Core::Command::kAllCmd = {
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

void Core::Command::runRefCommand() {
    int index = std::find(kAllCmd.begin(), kAllCmd.end(), list_[ref_].cmd_name_) - kAllCmd.begin();
}