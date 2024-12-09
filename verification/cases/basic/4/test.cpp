#include <iostream>
#include <cstdlib>

#define ll long long 
int main() {
    ll n, i, p, q, s;
    std::cin >> n;
    i = 0;
    p = 0;
    while (i < n) {
        q = reinterpret_cast<ll>(malloc(16));
        *(ll*)q = 0;
        std::cin >> *(ll*)q;
        *(ll*)(q + 8) = p;
        p = q;
        i = i + 1;
    }
    s = 0;
    while (p != 0) {
        s = s + *(ll*)p;
        p = *(ll*)(p + 8);
    }
    std::cout << s << std::endl;
    return 0;
}