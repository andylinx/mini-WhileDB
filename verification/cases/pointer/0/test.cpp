
#include <iostream>
#include <vector>

typedef long long ll;
int main() {
  std::vector<ll> a(10000);
  ll n;
  std::cin >> n;

  if (n > 10000) {
    std::cout << "n exceeds limit" << std::endl;
  } else {
    ll i = 0, s = 0;
    while (i < n) {
      std::cin >> a[i];
      i++;
    }

    i = n - 1;
    while (i >= 0) {
      s += a[i];
      std::cout << a[i] << std::endl;
      i--;
    }
    std::cout << s << std::endl;
  }

  return 0;
}