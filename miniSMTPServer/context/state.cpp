#include "state.hpp"

#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>

std::unique_ptr<State> States::idleState = std::make_unique<IdleState>();
std::unique_ptr<State> States::ehloState = std::make_unique<EhloState>();
std::unique_ptr<State> States::mailState = std::make_unique<MailState>();
std::unique_ptr<State> States::rcptState = std::make_unique<RcptState>();
std::unique_ptr<State> States::dataStartState = std::make_unique<DataStartState>();
std::unique_ptr<State> States::dataDoneState = std::make_unique<DataDoneState>();

static std::unordered_set<std::string> commands{"EHLO", "MAIL", "RCPT", "RSET", "NOOP", "QUIT", "DATA", "."};

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
  current = &States::idleState;
  return "221 " + codeToMessages["221"];
}

std::string State::transitiveFromNoop() { return "250 " + codeToMessages["250"]; }

std::optional<std::string> State::isCorrectParameters(std::vector<std::string> &parameters) {
  const std::regex pattern{"(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+"};

  std::string &command = parameters[0];
  if (command == "NOOP" || command == "QUIT" || command == "RSET") {
    if (parameters.size() != 1) {
      return "501 " + codeToMessages["501"];
    }
  } else if (parameters[0] == "EHLO") {
    if (parameters.size() != 2 || parameters[1] != "127.0.0.1") {
      return "501 " + codeToMessages["501"];
    }
  } else if (parameters[0] == "MAIL") {
    if (parameters.size() != 2 || !std::regex_match(parameters[1], pattern)) {
      return "501 " + codeToMessages["501"];
    }
  }

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

IdleState::IdleState() {}
std::string IdleState::transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) {
  if (auto result = transitiveHelper(parameters, current); result.has_value()) {
    return result.value();
  }

  if (parameters[0] == "EHLO") {
    current = &States::ehloState;
  }

  return "250 " + codeToMessages["250"];
}

EhloState::EhloState() { allowed.insert("MAIL"); }
std::string EhloState::transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) {
  if (auto result = transitiveHelper(parameters, current); result.has_value()) {
    return result.value();
  }

  if (parameters[0] == "MAIL") {
    current = &States::mailState;
  }

  return "250 " + codeToMessages["250"];
}

// TODO: add implementation for MailState
MailState::MailState() {}
std::string MailState::transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) { return {}; }

// TODO: add implementation for RcptState
RcptState::RcptState() {}
std::string RcptState::transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) { return {}; }

// TODO: add implementation for DataStartState
DataStartState::DataStartState() {}
std::string DataStartState::transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) {
  return {};
}

// TODO: add implementation for DataDoneState
DataDoneState::DataDoneState() {}
std::string DataDoneState::transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) {
  return {};
}
