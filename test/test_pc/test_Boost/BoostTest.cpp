#include <boost/static_string/static_string.hpp>
#include <iostream>

int main() {
  boost::static_string<128> str = "Hello, world!";
  std::cout << "str = " << str << std::endl;
  std::cout << "str.size() = " << str.size() << std::endl;
  return 0;
}
