#include <iostream>
#include <cstring>

int main() {
  const char c[100] = "Hello\nWorld\tTabbed\rCarriage\fFormFeed\aBell\bBackspace\\LiteralBackslash\x41\077";
  for (int i = 0; i < strlen(c); i++)
    std::cout << (int)c[i] << std::endl;
}