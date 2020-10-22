#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#pragma GCC optimize(2)
#pragma GCC optimize(3)
#pragma GCC optimize("Ofast")
#pragma GCC optimize("inline")
#pragma GCC optimize("-fgcse")
#pragma GCC optimize("-fgcse-lm")
#pragma GCC optimize("-fipa-sra")
#pragma GCC optimize("no-stack-protector")
#pragma GCC optimize("unroll-loops")

typedef unsigned int ui;
typedef unsigned long long ull;

struct hasher {
    const ull p;
    const ull q;

    vector<ull> _hash;
    vector<ull> _powers;

    hasher( const vector<ull>& str,
            const ull p=239017LL,
            const ull q=1073676287LL
            ) :
        _hash(str.size()), _powers(str.size()), p(p), q(q)
    {
        const ull size = str.size();

        ull pow = 1;
        for (ull i = 0; i < size; ++i) {
            _powers[i] = pow;
            pow = (pow * p) % q;
        }

        ull cur = (str.empty() ? 0 : str.front()) % q;
        for (ull i = 0; i + 1 < size; ++i) {
            _hash[i] = cur;
            cur = (cur * p + str[i + 1]) % q;
        }

        if (size > 0)
            _hash[size - 1] = cur;
    }

    ull get_hash(const ull l, const ull r) {
        if (l == 0)
            return _hash[r];

        return ((q + _hash[r]) -
                (_hash[l - 1] * _powers[r - l + 1] % q)) % q;
    }
};

inline bool compare(const vector<ull>& str, ull bst, const ull len) {
    ull fst = bst - 1;
    for (ull i = 0; i < len; ++i)
        if (str[bst++] != str[fst--])
            return false;

    return true;
}

int main() {
    cin.tie(0);
    ios_base::sync_with_stdio(false);

    int n = 0, m = 0;
    cin >> n >> m;

    vector<ull> str(n);
    for (auto& c: str)
        cin >> c;

    hasher forward(str);
    reverse(str.begin(), str.end());
    hasher backward(str);

    for (ull len = str.size() / 2; len > 0; --len) {
        const ull fst = len;
        const ull fed = 2 * len - 1;
        const ull bst = str.size() - len;
        const ull bed = str.size() - 1;
        if (forward.get_hash(fst, fed) == backward.get_hash(bst, bed)) {
            if (compare(str, bst, len))
                cout << (str.size() - len) << " ";
        }
    }

    cout << str.size() << endl;
}
