#include "util.hpp"

#include <string>

int SystemCall(const char *attempt, const int return_value, const int errno_mask) {
  if (return_value >= 0 || errno == errno_mask) {
    return return_value;
  }

  throw unix_error(attempt);
}

int SystemCall(const std::string &attempt, const int return_value, const int errno_mask) {
  return SystemCall(attempt.c_str(), return_value, errno_mask);
}
