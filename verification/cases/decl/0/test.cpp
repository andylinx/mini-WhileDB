#include <iostream>

int main() {
    int a[4] = {0, 1, 2};
    int b[4] = {0};
    int c[4] = {0, 1, 2, 3};

    int i = 0;
    while (i < 4) {
        std::cout << a[i] << std::endl;
        std::cout << b[i] << std::endl;
        std::cout << c[i] << std::endl;
        i = i + 1;
    }

    return 0;
}
