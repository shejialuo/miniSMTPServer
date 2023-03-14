#include "context.hpp"

#include "state.hpp"

Context::Context() { current = &States::idleState; }
std::string Context::transitive(std::vector<std::string> &parameters) {
  return (*current)->transitive(parameters, current);
}
