#pragma once

#include <limits>
#include <memory>

class FileDescriptor {
  /**
   * @brief  A handle on a kernel file descriptor.
   *
   * @details FileDescriptor objects contain a std::shared_ptr to a FDWrapper.
   */
  class FDWrapper {
  public:
    int fd;               //!< The file descriptor number returned by the kernel
    bool eof = false;     //!< Flag indicating whether FDWrapper::_fd is at EOF
    bool closed = false;  //!< Flag indicating whether FDWrapper::_fd has been closed

    /**
     * @brief Construct from a file descriptor number returned by the kernel
     *
     */
    explicit FDWrapper(const int fd);
    /**
     * @brief Closes the file descriptor upon destruction
     *
     */
    ~FDWrapper();
    /**
     * @brief Calls [close(2)](\ref man2::close) on FDWrapper::fd
     *
     */
    void close();

    FDWrapper(const FDWrapper &other) = delete;
    FDWrapper &operator=(const FDWrapper &other) = delete;
    FDWrapper(FDWrapper &&other) = delete;
    FDWrapper &operator=(FDWrapper &&other) = delete;
  };

  //! A reference-counted handle to a shared FDWrapper
  std::shared_ptr<FDWrapper> internal_fd;

  // private constructor used to duplicate the FileDescriptor (increase the reference count)
  explicit FileDescriptor(std::shared_ptr<FDWrapper> other_shared_ptr);

public:
  //! Construct from a file descriptor number returned by the kernel
  explicit FileDescriptor(const int fd);

  //! Free the std::shared_ptr; the FDWrapper destructor calls close() when the refcount goes to zero.
  ~FileDescriptor() = default;

  //! Read up to `limit` bytes
  std::string read(const size_t limit = std::numeric_limits<size_t>::max());

  //! Read up to `limit` bytes into `str` (caller can allocate storage)
  void read(std::string &str, const size_t limit = std::numeric_limits<size_t>::max());

  //! Write a string, possibly blocking until all is written
  size_t write(const char *str);

  //! Write a string, possibly blocking until all is written
  size_t write(const std::string &str);

  //! Close the underlying file descriptor
  void close() { internal_fd->close(); }

  //! Copy a FileDescriptor explicitly, increasing the FDWrapper refcount
  FileDescriptor duplicate() const;

  int fd_num() const { return internal_fd->fd; }

  bool eof() const { return internal_fd->eof; }

  bool closed() const { return internal_fd->closed; }

  FileDescriptor(const FileDescriptor &other) = delete;
  FileDescriptor &operator=(const FileDescriptor &other) = delete;
  FileDescriptor(FileDescriptor &&other) = default;
  FileDescriptor &operator=(FileDescriptor &&other) = default;
};

class TCPSocket : public FileDescriptor {
public:
  //! Construct via [socket(2)](\ref man2::socket)
  TCPSocket();

  explicit TCPSocket(FileDescriptor &&fd);

  //! Wrapper around [setsockopt(2)](\ref man2::setsockopt)
  template <typename option_type>
  void setsockopt(const int level, const int option, const option_type &option_value);

  //! Bind a socket to a specified address with [bind(2)](\ref man2::bind), usually for listen/accept
  void bind(int port = 9400);

  //! Mark a socket as listening for incoming connections
  void listen(const int backlog = 16);

  //! Accept a new incoming connection
  TCPSocket accept();

  //! Allow local address to be reused sooner via [SO_REUSEADDR](\ref man7::socket)
  void set_reuseaddr();
};
