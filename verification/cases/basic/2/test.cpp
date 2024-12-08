
#include <iostream>
using namespace std;

int main() {
    int n, i, s;
    cin >> n;
    i = 0;
    s = 0;
    while (i < n) {
        int temp;
        cin >> temp;
        s = s + temp;
        i = i + 1;
    }
    cout << s << endl;
    return 0;
}