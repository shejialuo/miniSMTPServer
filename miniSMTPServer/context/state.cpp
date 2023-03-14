#include "state.hpp"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>

static std::unordered_set<std::string> commands{"EHLO", "MAIL", "RCPT", "RSET", "NOOP", "QUIT"};

static std::unordered_map<std::string, std::string> codeToMessages{
    {"220", "Service ready"},
    {"221", "Service closing transmission channel"},
    {"250", "Requested mail action okay, completed"},
    {"354", "Start mail input end <CRLF>.<CRLF>"},
    {"500", "Syntax error, command unrecognized"},
    {"501", "Syntax error in parameters or arguments"},
    {"503", "Bad sequence of commands"},
};

State::State() {
  allowed.insert("RSET");
  allowed.insert("EHLO");
  allowed.insert("QUIT");
  allowed.insert("NOOP");
}

bool State::canTransitive(std::vector<std::string> &parameters) { return allowed.count(parameters[0]); }

std::optional<std::string> State::checkCommand(std::vector<std::string> &parameters) {
  if (!commands.count(parameters[0])) {
    return "500 " + codeToMessages["500"];
  }
  if (!canTransitive(parameters)) {
    return "503 " + codeToMessages["503"];
  }
  return std::nullopt;
}

std::string State::transitiveFromQuit(std::unique_ptr<State> *&current) {
  // TODO: add transition later
  return "221 " + codeToMessages["221"];
}

std::string State::transitiveFromNoop() { return "250 " + codeToMessages["250"]; }

std::optional<std::string> State::isCorrectParameters(std::vector<std::string> &parameters) {
  // TODO: finish later
  return std::nullopt;
}

std::optional<std::string> State::transitiveHelper(std::vector<std::string> &parameters,
                                                   std::unique_ptr<State> *&current) {
  if (auto result = checkCommand(parameters); result.has_value()) {
    return result.value();
  }

  if (auto result = isCorrectParameters(parameters); result.has_value()) {
    return result.value();
  }

  if (parameters[0] == "QUIT") {
    return transitiveFromQuit(current);
  }

  if (parameters[0] == "NOOP") {
    return transitiveFromNoop();
  }

  return std::nullopt;
}
