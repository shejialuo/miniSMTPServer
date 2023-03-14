#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class State {
protected:
  /**
   * @brief allowed operations for each state, it should be used in
   * derive class, so make it protected.
   *
   */
  std::unordered_set<std::string> allowed;

public:
  /**
   * @brief Construct a new State object. It will initialize the `allowed`
   * for adding four commands.
   *
   */
  State();

  /**
   * @brief can transitive to another state
   * @details check whether parameters[0] is in the allowed
   *
   * @param parameters the command and its parameters
   * @return true the command is OK for transition
   * @return false the command is not OK for transition
   */
  bool canTransitive(std::vector<std::string> &parameters);

  /**
   * @brief check the command whether it is in the command
   * and call `canTransitive`.
   *
   * @param parameters
   * @return std::optional<std::string>
   */
  std::optional<std::string> checkCommand(std::vector<std::string> &parameters);

  /**
   * @brief is the parameters are correct
   *
   * @param[in] parameters the command and its parameters
   * @return std::optional<std::string>
   */
  std::optional<std::string> isCorrectParameters(std::vector<std::string> &parameters);

  /**
   * @brief QUIT command handle
   *
   * @param[in] current the current state
   * @return std::string the response code
   */
  std::string transitiveFromQuit(std::unique_ptr<State> *&current);

  /**
   * @brief NOOP command handle
   *
   * @return std::string the response code
   */
  std::string transitiveFromNoop();

  /**
   * @brief The operations all the states need to do
   *
   */
  std::optional<std::string> transitiveHelper(std::vector<std::string> &parameters, std::unique_ptr<State> *&current);

  /**
   * @brief transitive to another state and return the response string.
   *
   * @param[in] parameters the command and its parameters
   * @param[out] current the current state
   * @return std::string the results should be sent back to the client
   */
  virtual std::string transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) = 0;

  virtual ~State() = default;
};

class IdleState : public State {
public:
  IdleState();
  std::string transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) override;
  ~IdleState() override = default;
};

class EhloState : public State {
public:
  EhloState();
  std::string transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) override;
  ~EhloState() override = default;
};

class MailState : public State {
public:
  MailState();
  std::string transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) override;
  ~MailState() override = default;
};

class RcptState : public State {
public:
  RcptState();
  std::string transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) override;
  ~RcptState() override = default;
};

class DataStartState : public State {
public:
  DataStartState();
  std::string transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) override;
  ~DataStartState() override = default;
};

class DataDoneState : public State {
public:
  DataDoneState();
  std::string transitive(std::vector<std::string> &parameters, std::unique_ptr<State> *&current) override;
  ~DataDoneState() override = default;
};

struct States {
  static std::unique_ptr<State> idleState;
  static std::unique_ptr<State> ehloState;
  static std::unique_ptr<State> mailState;
  static std::unique_ptr<State> rcptState;
  static std::unique_ptr<State> dataStartState;
  static std::unique_ptr<State> dataDoneState;
};
