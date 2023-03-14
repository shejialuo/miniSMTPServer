#include "state.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

static std::unordered_map<std::string, std::string> codeToMessages{
    {"220", "Service ready"},
    {"221", "Service closing transmission channel"},
    {"250", "Requested mail action okay, completed"},
    {"354", "Start mail input end <CRLF>.<CRLF>"},
    {"500", "Syntax error, command unrecognized"},
    {"501", "Syntax error in parameters or arguments"},
    {"503", "Bad sequence of commands"},
};

TEST(State, isCorrectParametersNOOP) {
  std::vector<std::vector<std::string>> tests{
      {"NOOP", "param1"},
      {"NOOP", "param1", "param2"},
      {"NOOP", "12"},
      {"NOOP", "3"},
  };

  auto state = std::make_unique<IdleState>();

  for (auto &&test : tests) {
    auto result = state->isCorrectParameters(test);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), "501 " + codeToMessages["501"]);
  }

  std::vector<std::string> successful{"NOOP"};

  ASSERT_FALSE(state->isCorrectParameters(successful).has_value());
}

TEST(State, isCorrectParametersQUIT) {
  std::vector<std::vector<std::string>> tests{
      {"QUIT", "NOOP"},
      {"QUIT", "NOOP", "EHLO"},
      {"QUIT", "12", "13", "14", "15"},
      {"QUIT", "3", "4", "11111", "22"},
      {"QUIT", "MAIL", "RCPT", "11111", "22"},
  };

  auto state = std::make_unique<IdleState>();

  for (auto &&test : tests) {
    auto result = state->isCorrectParameters(test);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), "501 " + codeToMessages["501"]);
  }

  std::vector<std::string> successful{"QUIT"};

  ASSERT_FALSE(state->isCorrectParameters(successful).has_value());
}

TEST(State, isCorrectParametersRSET) {
  std::vector<std::vector<std::string>> tests{
      {"RSET", "NOOP"},
      {"RSET", "NOOP", "EHLO"},
      {"RSET", "12", "13", "14", "15"},
      {"RSET", "3", "4", "11111", "22"},
      {"RSET", "MAIL", "RCPT", "11111", "22"},
  };

  auto state = std::make_unique<IdleState>();

  for (auto &&test : tests) {
    auto result = state->isCorrectParameters(test);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), "501 " + codeToMessages["501"]);
  }

  std::vector<std::string> successful{"RSET"};

  ASSERT_FALSE(state->isCorrectParameters(successful).has_value());
}

TEST(State, isCorrectParametersEHLO) {
  std::vector<std::vector<std::string>> tests{
      {"EHLO"},
      {"EHLO", "127.0.0.2"},
      {"EHLO", "127.0.1.1"},
      {"EHLO", "NOOP", "EHLO"},
      {"EHLO", "12", "13", "14", "15"},
      {"EHLO", "3", "4", "11111", "22"},
      {"EHLO", "MAIL", "RCPT", "11111", "22"},
  };

  auto state = std::make_unique<IdleState>();

  for (auto &&test : tests) {
    auto result = state->isCorrectParameters(test);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), "501 " + codeToMessages["501"]);
  }

  std::vector<std::string> successful{"EHLO", "127.0.0.1"};

  ASSERT_FALSE(state->isCorrectParameters(successful).has_value());
}

TEST(State, isCorrectParametersMAIL) {
  std::vector<std::vector<std::string>> tests{
      {"MAIL"},
      {"MAIL", "MAIL"},
      {"MAIL", "NOOP", "MAIL"},
      {"MAIL", "shejialuo@gamil..com"},
      {"EHLO", "shejialuo"},
      {"EHLO", "shejialuo@.com.com"},
      {"EHLO", "shejialuo@123.1.cn"},
  };

  auto state = std::make_unique<IdleState>();

  for (auto &&test : tests) {
    auto result = state->isCorrectParameters(test);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), "501 " + codeToMessages["501"]);
  }

  std::vector<std::string> successful{"MAIL", "shejialuo@gmail.com"};

  ASSERT_FALSE(state->isCorrectParameters(successful).has_value());
}

TEST(State, IdleStateTransitive) {
  std::vector<std::vector<std::string>> tests{
      {"RSET"},
      {"NOOP"},
      {"QUIT"},
      {"EHLO", "127.0.0.1"},
      {"RSTE"},
      {"NOOQ"},
      {"RSET", "NOOP"},
      {"NOOP", "NOOP"},
      {"QUIT", "QUIT"},
      {"EHLO", "127.0.1.1"},
      {"DATA"},
  };

  std::vector<std::pair<std::string, std::unique_ptr<State> *>> expects{
      {"250 " + codeToMessages["250"], &States::idleState},
      {"250 " + codeToMessages["250"], &States::idleState},
      {"221 " + codeToMessages["221"], &States::idleState},
      {"250 " + codeToMessages["250"], &States::ehloState},
      {"500 " + codeToMessages["500"], &States::idleState},
      {"500 " + codeToMessages["500"], &States::idleState},
      {"501 " + codeToMessages["501"], &States::idleState},
      {"501 " + codeToMessages["501"], &States::idleState},
      {"501 " + codeToMessages["501"], &States::idleState},
      {"501 " + codeToMessages["501"], &States::idleState},
      {"503 " + codeToMessages["503"], &States::idleState},
  };

  for (int i = 0; i < tests.size(); ++i) {
    auto state = std::make_unique<IdleState>();
    std::unique_ptr<State> *current = &States::idleState;
    std::string result = state->transitive(tests[i], current);
    EXPECT_EQ(result, expects[i].first);
    EXPECT_EQ(current, expects[i].second);
  }
}

TEST(State, EhloStateTransitive) {
  std::vector<std::vector<std::string>> tests{
      {"RSET"},
      {"NOOP"},
      {"QUIT"},
      {"EHLO", "127.0.0.1"},
      {"MAIL", "shejialuo@gmail.com"},
      {"RSTE"},
      {"DATA"},
      {"."},
      {"RCPT"},
  };

  std::vector<std::pair<std::string, std::unique_ptr<State> *>> expects{
      {"250 " + codeToMessages["250"], &States::ehloState},
      {"250 " + codeToMessages["250"], &States::ehloState},
      {"221 " + codeToMessages["221"], &States::idleState},
      {"250 " + codeToMessages["250"], &States::ehloState},
      {"250 " + codeToMessages["250"], &States::mailState},
      {"500 " + codeToMessages["500"], &States::ehloState},
      {"503 " + codeToMessages["503"], &States::ehloState},
      {"503 " + codeToMessages["503"], &States::ehloState},
      {"503 " + codeToMessages["503"], &States::ehloState},
  };

  for (int i = 0; i < tests.size(); ++i) {
    auto state = std::make_unique<EhloState>();
    std::unique_ptr<State> *current = &States::ehloState;
    std::string result = state->transitive(tests[i], current);
    EXPECT_EQ(result, expects[i].first);
    EXPECT_EQ(current, expects[i].second);
  }
}
