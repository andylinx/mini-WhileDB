#include <iostream>
typedef long long ll;
int main() {
  ll *a = (ll*)malloc(16);
  a[0] = a[1] = 0;
  std::cin >> *a;
  for (int i = 0; i < 9; i++) {
    ll *p = (ll*)(reinterpret_cast<ll>(a) + i);
    std::cout << *p << std::endl;
  }
  return 0;
}