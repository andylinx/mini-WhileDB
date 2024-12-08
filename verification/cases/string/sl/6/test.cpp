#include <iostream>
#include <cstring>

int main() {
  const char c[] = "\\n\n";
  for (int i = 0; i < strlen(c); i++)
    std::cout << (int)c[i];
}