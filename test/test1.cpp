#include "../src/core/core.h"

int main() {
    Core::Game game;
    std::vector<std::string> available_command = { "inbox", "outbox" };
    std::vector<int> provided_sequence = {1, 2, 3};
    std::vector<int> needed_sequence = {3, 2, 1};
    std::vector<std::pair<std::string, int>> command;
    command.emplace_back("inbox", Core::Command::SingleCommand::kNullVacant);
    command.emplace_back("outbox", Core::Command::SingleCommand::kNullVacant);
    command.emplace_back("inbox", Core::Command::SingleCommand::kNullVacant);
    command.emplace_back("outbox", Core::Command::SingleCommand::kNullVacant);
    game.initialize(available_command,
                    provided_sequence,
                    needed_sequence,
                    command,
                    0);
    return 0;
}