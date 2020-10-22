#include <iostream>
#include <string>

using namespace std;

int main() {
    string text, pattern;
    cin >> text >> pattern;

    const size_t end = text.size() - pattern.size();
    const size_t pat_size = pattern.size();
    for (int i = 0; i <= end; ++i)
        if (text.compare(i, pat_size, pattern) == 0)
            cout << i << " ";
}
