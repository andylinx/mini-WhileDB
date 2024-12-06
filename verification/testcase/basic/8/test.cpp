#include <iostream>
#include <vector>

int main() {
    int a[10000];
    int n;
    std::cin >> n;
    int i = 0;
    while (i < n) {
        std::cin >> *(a + i);
        i++;
    }
    i = 0;
    int s = 0;
    while (i < n) {
        s += a[i];
        i++;
    }
    std::cout << s << std::endl;
    return 0;
}
