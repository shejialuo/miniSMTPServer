#include "context.hpp"
#include "socket.hpp"

#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> getParameters(std::string &request) {
  request.pop_back();
  request.pop_back();
  int split = 0;
  for (; split < request.size(); split++) {
    if (request[split] == ' ') {
      break;
    }
  }

  std::string command = request.substr(0, split);
  if (split != request.size()) {
    std::string parameter = request.substr(split + 1, request.size() - split - 1);
    return {command, parameter};
  }

  return {command};
}

int main() {
  std::cout << "Hello, This is a simple SMTP server\n";

  TCPSocket socket{};
  socket.set_reuseaddr();
  socket.bind();
  socket.listen();

  std::string request{};
  Context context{};

  while (true) {
    auto s = socket.accept();

    bool isDone = false;
    while (!isDone) {
      s.read(request);
      if (request.empty()) {
        std::cout << "S: Connection lost\n";
        s.close();
        break;
      }
      std::cout << "C: " << request;
      std::string parameter{};
      std::vector<std::string> parameters = getParameters(request);

      std::string result = context.transitive(parameters);

      s.write(result + "\r\n");
      std::cout << "S: " << result << std::endl;
      if (result.substr(0, 3) == "221") {
        s.close();
        isDone = true;
      }
    }
  }

  return 0;
}
