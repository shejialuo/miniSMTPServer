#pragma once

#include "state.hpp"

#include <memory>

class Context {
private:
  std::unique_ptr<State> *current;

public:
  Context();
  std::string transitive(std::vector<std::string> &parameters);
  ~Context() = default;
};
