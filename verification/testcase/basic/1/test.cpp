#include <iostream>
using namespace std;

int main() {
    int n, i, flag;
    cin >> n;
    i = 2;
    flag = 1;
    while (flag && i * i <= n) {
        if (n % i == 0) {
            flag = 0;
        } else {
            flag = 1;
        }
        i = i + 1;
    }
    if (flag) {
        cout << "PRIME" << endl;
    } else {
        cout << "NONPRIME" << endl;
    }
    return 0;
}
