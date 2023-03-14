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
