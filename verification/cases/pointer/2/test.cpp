#include <iostream>
#include <cstdlib>

typedef long long ll;
int main() {
    ll* a = (ll*)malloc(16);
    ll* b;
    ll* c[2];
    ll** d = (ll**)malloc(8);
    std::cin >> *a;
    std::cin >> *(a + 1);
    b = a;
    std::cout << *b << std::endl;
    c[0] = a;
    c[1] = a + 1;
    std::cout << *c[0] << std::endl;
    std::cout << *c[1] << std::endl;
    *d = a;
    std::cout << **d << std::endl;
    free(a);
    free(d);
    return 0;
}