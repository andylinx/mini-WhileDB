#include <iostream>
#include <cstring>

int main() {
  const char c[4] = "\\n\n";
  for (int i = 0; i < 4; i++)
    std::cout << (int)c[i] << std::endl;
}