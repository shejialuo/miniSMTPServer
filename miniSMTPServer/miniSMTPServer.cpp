#include "socket.hpp"

#include <iostream>
#include <string>

int main() {
  std::cout << "Hello, This is a simple SMTP server\n";

  TCPSocket socket{};
  socket.bind();
  socket.listen();

  std::string result{};

  while (true) {
    auto s = socket.accept();

    while (true) {
      s.read(result);
      if (result.size() == 0) {
        std::cout << "S: Connection lost\n";
        s.close();
        break;
      }
      s.write("Hello, this is the server\n");
    }
  }

  return 0;
}
