#include "app.hpp"

//std
#include <iostream>

int main() {
  std::cout << "Hello world!" << std::endl;
  Malodee::App app{800,600, "Malodee"};
  app.run();
  return 0;
}