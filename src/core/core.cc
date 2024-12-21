//======================================================//
// Copyright (c) 2024 AshGrey. All rights reserved.     //
// Released under MIT license as described in the file  //
// LICENSE.                                             //
// Author: AshGrey (Grey He)                            //
//                                                      //
// This file is the implement of header file `core.h`   //
//======================================================//

#include "core.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>


/**
 * @program:     Core::initLogFile
 * @description: This function is to create `log` directory and the log file
 */
void Core::initLogFile() {
    std::filesystem::create_directory(log);

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d_%H_%M_%S");

    log_name = ss.str();

    std::ofstream log_file(log / (log_name + ".log"));
}

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


    std::ofstream log_file(log / (log_name + ".log"), std::ios::app);

    if (type == Core::LogType::kInfo) {
        log_file << "Info  ["
                 << ss.str() 
                 << "] #("
                 << loc_str
                 << ") "
                 << message
                 << std::endl;
    } else if (type == Core::LogType::kError) {
        log_file << "Error ["
                 << ss.str() 
                 << "] #("
                 << loc_str
                 << ") "
                 << message
                 << std::endl;
    }

    log_file.close();
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
    Core::logMessage("Pass command name and operated index of "
                     "vacant from class `Core::Command` to class "
                     "`Core::Command::SingleCommand`.",
                     Core::LogLocation::kCore,
                     Core::LogType::kInfo);
}

/**
 * @program:     Core::Robot::initCommandList
 * @description: This function passes the command name and vacant index to the Core::Command::appendToList
 * @name:        The name of command, see Core::Command::kAllCmd
 * @index:       The index of vacant
 */
void Core::Robot::initCommandList(const std::string& name, int index) {
    cmd_.appendToList(name, index);
    Core::logMessage("Pass command name and operated index of "
                     "vacant from class `Core::Robot` to class "
                     "`Core::Command`.", 
                     Core::LogLocation::kCore, 
                     Core::LogType::kInfo);
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

            Core::logMessage("Unknown command `" + str + "`, please check "
                             "the entire supported command list by typing key `?`.", 
                             Core::LogLocation::kCore, 
                             Core::LogType::kError);
            game_state_ = false;
            error_state_ = true;
            return;
        }
    }

    for (const auto& e : provided_seq_) {
        game_input_.seq_.push_back(e);
    }

    for (auto it = cmd.begin(); it < cmd.end(); ++it) {
        auto& [name, index] = *it;
        if (std::find(available_cmd_.begin(), available_cmd_.end(), name) == available_cmd_.end()) {

            // This condition means the command name is not in the available commands, 
            // so we need error here and return

            std::cout << "Error on instruction " + std::to_string(it - cmd.begin() + 1) << std::endl;
            Core::logMessage("Command ID " + std::to_string(it - cmd.begin() + 1) + 
                             " : Unknown command `" + name + "`, please "
                             "check the available command list by typing "
                             "key `?`.", 
                             Core::LogLocation::kCore, 
                             Core::LogType::kError);
            game_state_ = false;
            error_state_ = true;
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

/**
 * @program:     Core::Command::checkOpindexSurplus
 * @description: This function is to check if the operated index (the operated 
                 vacant index of the command that is executed now) is surplus
 */
bool Core::Command::checkOpindexSurplus() {
    if (list_[ref_ - 1].target_index_ != Core::Command::SingleCommand::kNullVacant) {

        // Notice reference begins from **1** !!!, only use this function in no-parameter
        // command. If the no-parameter command has target index, then we need to error here

        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Surplus operated vacant index, the "
                         "command `" + list_[ref_ - 1].cmd_name_ + "` doesn't need an operated "
                         "vacant index.", 
                         Core::LogLocation::kCore,
                         Core::LogType::kError);
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
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Invalid operated vacant index, index (" +
                         std::to_string(list_[ref_ - 1].target_index_) + 
                         ") is greater than or equal to vacant size (" +
                         std::to_string(vacant_->seq_.size()) + ".", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else if (list_[ref_ - 1].target_index_ < 0) {

        // vacant index is less than 0

        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Invalid operated vacant index, index (" +
                         std::to_string(list_[ref_ - 1].target_index_) + 
                         ") is less than 0.",
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else if ((list_[ref_ - 1].cmd_name_ == "add"
             || list_[ref_ - 1].cmd_name_ == "sub"
             || list_[ref_ - 1].cmd_name_ == "copyfrom")
             && vacant_->seq_empty_[list_[ref_ - 1].target_index_ - 1]) {
        
        // The target vacant is empty
        // for command "add", "sub" and "copyfrom", the target vacant cannot be empty!

        game_->setGameState(false);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : The operated vacant doesn't store any box.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
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
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Robot doesn't take any box, but the command "
                         "`" + list_[ref_ - 1].cmd_name_ + "` means put the handbox down.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else {
        return false;
    }
}

bool Core::Command::checkCmindexInvalid() {
    if (ref_ > list_.size()) {
        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : This command jumps out of the end of command list.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;
    } else if (ref_ <= 0) {
        game_->setGameState(false);
        game_->setErrorState(true);
        std::cout << "Error on instruction " + std::to_string(ref_) << std::endl;
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : This command jumps out of the begin of command list.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
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
        Core::logMessage("Input has been empty. Now check the game state.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
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
        Core::logMessage("Command ID " + std::to_string(ref_) + 
                         " : This command needs the target vacant not empty.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kError);
        return true;    // Empty
    } else {
        return false;   // Not empty
    }
}

/**
 * @program:     Core::Command::runRefCommand
 * @description: This function is to run the reference command
 */
void Core::Command::runRefCommand() {
    if (ref_ == list_.size() + 1) {

        // All the commands have been executed, so we set the game state false

        game_->setGameState(false);
        Core::logMessage("All command has been executed.",
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        return;
    }
    
    int index = std::find(kAllCmd.begin(), kAllCmd.end(), list_[ref_ - 1].cmd_name_) - kAllCmd.begin();

    // Get the index of this command

    switch (index) {
    case 0 : // "inbox"
        if (checkOpindexSurplus()) return;
        
        // Check if vacant index is given, "inbox" command doesn't need parameter

        if (checkInputEmpty()) return;
        
        // Check if input is empty, if true, then the game ends

        owner_->setValue(input_->seq_.front());
        owner_->setState(false);
        input_->seq_.pop_front();
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Robot takes the box (value : " + 
                         std::to_string(owner_->getValue()) + ") from the input.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 1 : // "outbox"
        if (checkOpindexSurplus()) return;
        
        // Check if vacant index is given. "outbox" command doesn't need parameter

        if (checkHandboxEmpty()) return;
        
        // Check if the handbox is empty. "outbox" command needs the robot holds a box

        output_->seq_.push_back(owner_->getValue());    // Put the box to output
        owner_->setValue(Core::Robot::kEmptyHandbox);   // Robot doesn't hold this box anymore
        owner_->setState(true);
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Robot puts the handbox (value : " +
                         std::to_string(output_->seq_.back()) +
                         ") down on the output.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 2 : // "add"
        if (checkHandboxEmpty()) return;
        
        // Check if the handbox is empty. "add" command needs the robot holding a box

        if (checkOpindexInvalid()) return;
        
        // Check if vacant index is invalid.

        owner_->setValue(owner_->getValue() + vacant_->seq_[list_[ref_ - 1].target_index_]);
        //               ^^^^^^^^^^^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //                 Handbox Value                      Target Vacant Value
        //                                    Notice command reference begins from **1**
        //                                    Vacant index begins from **0**

        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Robot adds the number (" + 
                         std::to_string(vacant_->seq_[list_[ref_ - 1].target_index_]) +
                         ") of operated vacant index (" + 
                         std::to_string(list_[ref_ - 1].target_index_) +
                         "), and the handbox becomes " +
                         std::to_string(owner_->getValue()), 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 3 : // "sub"
        if (checkHandboxEmpty()) return;
        
        // Check if the handbox is empty. "sub" command needs the robot holding a box

        if (checkOpindexInvalid()) return;
        
        // Check if vacant index is invalid

        owner_->setValue(owner_->getValue() - vacant_->seq_[list_[ref_ - 1].target_index_]);
        //               ^^^^^^^^^^^^^^^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        //                 Handbox Value                    Target Vacant Value
        //                                    Notice command reference begins from **1**
        //                                    Vacant index begins from **0**

        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Robot subs the number (" +
                         std::to_string(vacant_->seq_[list_[ref_ - 1].target_index_]) +
                         ") of operated vacant index (" + 
                         std::to_string(list_[ref_ - 1].target_index_) +
                         "), and the handbox becomes " +
                         std::to_string(owner_->getValue()), 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 4 : // "copyto"
        if (checkHandboxEmpty()) return;
        
        // Check if the handbox is empty. "copyto" command needs the robot holding a box

        if (checkOpindexInvalid()) return;
        
        // Check if vacant index is invalid

        vacant_->seq_[list_[ref_ - 1].target_index_] = owner_->getValue();  // Put the box to the vacant
        vacant_->seq_empty_[list_[ref_ - 1].target_index_] = false;         // Set the vacant state to not-empty
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Robot copy its handbox to the number of operated vacant index (" +
                         std::to_string(list_[ref_ - 1].target_index_) + ").", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 5 : // "copyfrom"
        if (checkOpindexInvalid()) return;
        
        // Check if vacant index is invalid

        if (checkVacantEmpty()) return;
        
        // Check if the vacant is empty

        owner_->setValue(vacant_->seq_[list_[ref_ - 1].target_index_ - 1]);
        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Robot copy from the number of operated vacant index (" +
                         std::to_string(list_[ref_ - 1].target_index_) +
                         ") to its handbox, now the handbox is "
                         + std::to_string(owner_->getValue()), 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_++;
        break;
    case 6 : // jump
        if (checkCmindexInvalid()) return;
        
        // Check if there is the target command ID

        Core::logMessage("Command ID " + std::to_string(ref_) +
                         " : Robot's current command jumps to the index " +
                         std::to_string(list_[ref_ - 1].target_index_) + " command", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
        ref_ = list_[ref_ - 1].target_index_; 
        break;
    case 7 : // jumpifzero
        if (owner_->getValue() == 0) {
            if (checkHandboxEmpty()) return;
            
            // Check if there is any box held by robot

            if (checkCmindexInvalid()) return;
            
            // Check if there is the target command ID

            ref_ = list_[ref_ - 1].target_index_;
            Core::logMessage("Command ID " + std::to_string(ref_) +
                             " : Robot's current command jumps to the index " +
                             std::to_string(ref_) + " command, because handbox is 0.", 
                             Core::LogLocation::kCore, 
                             Core::LogType::kInfo);
        } else {
            ref_++;
        }
    }
}

/**
 * @program:     Core::Game::check
 * @description: This function checks the game state when all commands have been executed 
 *               or the input is empty
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
        Core::logMessage("Success! The output is same as needed.", 
                         Core::LogLocation::kCore,
                         Core::LogType::kInfo);
    } else {
        std::cout << "Fail" << std::endl;
        Core::logMessage("Fail! The output is not same as needed.", 
                         Core::LogLocation::kCore, 
                         Core::LogType::kInfo);
    }
}

/**
 * @program:     Core::Game::runAll
 * @description: This function is to run all commands from begin to end
 */
void Core::Game::runAll() {
    game_state_ = !error_state_;

    // First set game state the negation of error state

    while (game_state_ && !error_state_) {
        game_robot_.runRefCommand();
        std::this_thread::sleep_for(std::chrono::seconds(game_gap_));

        // pause to wait for the animation of robot

    }
    check();
}

/**
 * @program:     Core::Game::runTo
 * @description: This function is to run all commands from begin to the target reference
 * @param:       target_ref : The target reference of commands
 */
void Core::Game::runTo(int target_ref) {
    game_state_ = !error_state_;
    while (game_state_ && error_state_) {
        game_robot_.runRefCommand();
        if (target_ref == game_robot_.getRef()) {
            game_state_ = false;
        }
        std::this_thread::sleep_for(std::chrono::seconds(game_gap_));

        // pause to wait for the animation of robot
    }
    check();
}

/**
 * @program:     Core::Game::restart
 * @description: This function is to run all commands from begin to end **again**
 */
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