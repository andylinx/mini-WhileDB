
#include <iostream>
#include <vector>

int main() {
    char a[10000];
    std::cin >> a;
    int i = 0;
    while (i < 10000 && a[i] != '\0') {
        std::cout << a[i];
        i++;
    }
    std::cout << std::endl;
    return 0;
}