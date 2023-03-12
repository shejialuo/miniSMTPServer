#include "socket.hpp"

#include "util.hpp"

#include <arpa/inet.h>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

FileDescriptor::FDWrapper::FDWrapper(const int f) : fd{f} {
  if (fd < 0) {
    throw std::runtime_error("invalid fd number:" + std::to_string(fd));
  }
}

void FileDescriptor::FDWrapper::close() {
  SystemCall("close", ::close(fd));
  eof = closed = true;
}

FileDescriptor::FDWrapper::~FDWrapper() {
  try {
    if (closed) {
      return;
    }
    close();
  } catch (const std::exception &e) {
    std::cerr << "Exception destructing FileWrapper: " << e.what() << std::endl;
  }
}

FileDescriptor::FileDescriptor(const int fd) : internal_fd{std::make_shared<FDWrapper>(fd)} {}

FileDescriptor::FileDescriptor(std::shared_ptr<FDWrapper> other) : internal_fd{std::move(other)} {}

FileDescriptor FileDescriptor::duplicate() const { return FileDescriptor(internal_fd); }

void FileDescriptor::read(std::string &str, const size_t limit) {
  constexpr size_t BUFFER_SIZE = 1024 * 1024;
  const size_t size_to_read = std::min(BUFFER_SIZE, limit);
  str.resize(size_to_read);

  ssize_t bytes_read = SystemCall("read", ::read(fd_num(), str.data(), size_to_read));
  if (limit > 0 && bytes_read == 0) {
    internal_fd->eof = true;
  }

  if (bytes_read > static_cast<ssize_t>(size_to_read)) {
    throw std::runtime_error("read() read more than requested");
  }
  str.resize(bytes_read);
}

std::string FileDescriptor::read(const size_t limit) {
  std::string ret;
  read(ret, limit);
  return ret;
}

size_t FileDescriptor::write(const char *str) {
  size_t bytes_written = 0;
  bytes_written = SystemCall("write", ::write(fd_num(), str, strlen(str)));

  if (bytes_written == 0 && strlen(str) != 0) {
    throw std::runtime_error("write() returned 0 given non-empty input");
  }

  if (bytes_written > strlen(str)) {
    throw std::runtime_error("write() wrote more than requested");
  }

  return bytes_written;
}

size_t FileDescriptor::write(const std::string &str) { return write(str.c_str()); }

TCPSocket::TCPSocket() : FileDescriptor{SystemCall("socket", ::socket(AF_INET, SOCK_STREAM, 0))} {}

TCPSocket::TCPSocket(FileDescriptor &&fd) : FileDescriptor(std::move(fd)) {}

template <typename option_type>
void TCPSocket::setsockopt(const int level, const int option, const option_type &option_value) {
  SystemCall("setsockopt", ::setsockopt(fd_num(), level, option, &option_value, sizeof(option_value)));
}

void TCPSocket::set_reuseaddr() { setsockopt(SOL_SOCKET, SO_REUSEADDR, int(true)); }

void TCPSocket::bind(int port) {
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
  SystemCall("bind", ::bind(fd_num(), (struct sockaddr *)&address, sizeof(address)));
}

void TCPSocket::listen(const int backlog) { SystemCall("listen", ::listen(fd_num(), backlog)); }

TCPSocket TCPSocket::accept() {
  return TCPSocket(FileDescriptor(SystemCall("accept", ::accept(fd_num(), nullptr, nullptr))));
}
