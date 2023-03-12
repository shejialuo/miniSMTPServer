// This file is modified from the CS144 Network Programming Code
// All the rights are reserved by the CS144 team

#pragma once

#include <cerrno>
#include <string>
#include <system_error>

/**
 * @brief std::system_error plus the name of what was being
 * attempted.
 *
 */
class tagged_error : public std::system_error {
private:
  std::string attempt_and_error;  //!< what was attempted, and what happened

public:
  /**
   * @brief Construct from a category, an attempt, and an error code.
   *
   * @param[in] category is the category of error
   * @param[in] attempt is what was supposed to happen
   * @param[in] error_code is the resulting error
   */
  tagged_error(const std::error_category &category, const std::string &attempt, const int error_code)
      : std::system_error{error_code, category}, attempt_and_error{attempt + ": " + std::system_error::what()} {}

  /**
   * @brief Returns a C string describing the error
   *
   */
  const char *what() const noexcept override { return attempt_and_error.c_str(); }
};

/**
 * @brief A tagged_error for syscalls
 *
 */
class unix_error : public tagged_error {
public:
  /**
   * @brief  Construct from a syscall name and the resulting errno
   *
   * @param[in] attempt is the name of the syscall attempted
   * @param[in] error is the [errno(3)](\ref man3::errno) that resulted
   */
  explicit unix_error(const std::string &attempt, const int error = errno)
      : tagged_error{std::system_category(), attempt, error} {}
};

/**
 * @brief Error-checking wrapper for most syscalls
 *
 */
int SystemCall(const char *attempt, const int return_value, const int errno_mask = 0);

/**
 * @brief Version of SystemCall that takes a C++ std::string
 *
 */
int SystemCall(const std::string &attempt, const int return_value, const int errno_mask = 0);
