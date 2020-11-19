#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

typedef int ull;

pair<char, size_t> range(const string& str) {
    if (str.empty())
        return {0, 0};

    char min = str.front(), max = str.front();
    for (const char& c: str) {
        min = min > c ? c : min;
        max = max < c ? c : max;
    }

    return {min, max - min + 1};
}

class SuffArray {
    string str;
    vector<ull> sorted;

    vector<ull> init_classes() const {
        const auto& [min, size] = range(str);

        vector<ull> cnt(size);
        for (const auto& c: str)
            cnt.at(c - min) = 1;

        for (size_t i = 1; i < size; ++i)
            cnt.at(i) += cnt.at(i - 1);

        vector<ull> retval(str.size());
        for (size_t i = 0; i < str.size(); ++i) {
            const char& c = str.at(i);
            retval.at(i) = cnt.at(c - min) - 1;
        }

        return retval;
    }

    void fill_sorted() {
        const size_t& n = str.size();
        vector<ull> classes = init_classes();

        vector<tuple<ull, ull, size_t>> suffs(n);
        for (size_t k = 0; (1 << k) < n; ++k) {
            const size_t pow = (1 << k);

            for (size_t i = 0; i < n; ++i)
                suffs.at(i) = tie(classes.at(i), classes.at((i + pow) % n), i);

            sort(suffs.begin(), suffs.end());

            ull new_class = 0;
            for (size_t i = 0; i < n;) {
                const auto& [fc, bc, _] = suffs.at(i);
                for (; i < n; ++i) {
                    const auto& [curfc, curbc, id] = suffs.at(i);
                    if (curfc != fc || curbc != bc) {
                        ++new_class;
                        break;
                    }
                    classes.at(id) = new_class;
                }
            }
        }

        for (size_t i = 0; i < n; ++i)
            sorted.at(classes.at(i)) = i;
    }

    public:
    SuffArray(const string& word) :
        str(word),
        sorted(word.size() + 1)
    {
        str += '$';
        fill_sorted();
    }

    void print() const {
        for (const auto& i: sorted)
            cout << str.substr(i) << endl;
    }

    friend ostream& operator<<(ostream& out, const SuffArray& sa);
};

ostream& operator<<(ostream& out, const SuffArray& sa) {
    for (size_t i = 1; i < sa.sorted.size(); ++i)
        out << sa.sorted.at(i) + 1 << ' ';
    out << endl;

    return out;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    string str;
    cin >> str;
    SuffArray sa(str);
    cout << sa;
}
