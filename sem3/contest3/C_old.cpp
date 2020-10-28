#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>

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

class Hasher {
    static const ull p = 239017LL;
    static const ull q = 1073676287LL;
    static vector<ull> _powers;

    vector<ull> _hash;

    static void update_powers(const size_t size) {
        const size_t cur_size = _powers.size();
        ull pow = (_powers.back() * p) % q;

        for (ull i = cur_size; i < size; ++i) {
            _powers.push_back(pow);
            pow = (pow * p) % q;
        }
    }

public:
    Hasher(const string& str) : _hash(str.size())
    {
        const ull size = str.size();
        update_powers(size);

        ull cur = str.front() % q;
        for (ull i = 0; i + 1 < size; ++i) {
            _hash[i] = cur;
            cur = (cur * p + str[i + 1]) % q;
        }
        _hash[size - 1] = cur;
    }

    ull get_hash(const ull l, const ull r) const {
        if (l == 0) return _hash[r];
        return ((q + _hash[r]) - ((_hash[l - 1] * _powers[r - l + 1]) % q)) % q;
    }
};

vector<ull> Hasher::_powers = {1};

struct Gene {
    const string gene;
    const ull hash;

    Gene(string&& str) : gene(str), hash(Hasher(gene).get_hash(0, gene.length() - 1)) {}
};

struct DNA {
    const string dna;
    const Hasher hash;
    size_t pos;

    DNA(string&& dna) : dna(dna), hash(dna), pos(0) {}

    inline bool is_done() const {
        return pos == dna.size();
    }

    void update(const vector<Gene>& genes) {
        bool flag = true;
        while (flag && !is_done()) {
            flag = false;
            for (const auto& g: genes) {
                const string& gene = g.gene;
                const ull& gene_hash = g.hash;

                if (
                        (gene.length() <= dna.length() - pos) &&
                        (gene_hash == hash.get_hash(pos, pos + gene.length() - 1)) &&
                        (dna.compare(pos, gene.length(), gene) == 0)
                   ) {
                    pos += gene.length();
                    flag = true;
                }
            }
        }
    }
};

#include <unistd.h>

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    ull n = 0;
    cin >> n;

    ull k = 0;
    unordered_set<ull> ids;
    vector<DNA> dnas;
    vector<Gene> genes;
    while (n--) {
        char op = 0;
        cin >> op;

        string str;
        cin >> str;

        k %= str.length();
        if (k) str = str.substr(k, str.length() - k) + str.substr(0, k);

        if (op == '?') {
            dnas.emplace_back(move(str));
            dnas.back().update(genes);

            if (dnas.back().is_done()) {
                k = 1;
                cout << "1 " << dnas.size() << "\n";
            }
            else {
                k = 0;
                ids.insert(dnas.size() - 1);
                cout << "0\n";
            }
        }
        else if (op == '+') {
            genes.emplace_back(move(str));

            vector<ull> deleted;
            for (const auto& id: ids) {
                DNA& dna = dnas.at(id);
                dna.update(genes);
                if (dna.is_done())
                    deleted.push_back(id);
            }

            k = deleted.size();
            for (const auto& id: deleted)
                ids.erase(id);

            cout << k << " ";
            for (const auto& id: deleted)
                cout << (id + 1) << " ";
            cout << "\n";
        }
        else {
            cout << "bruh";
            return 1;
        }
    }

    cout.flush();
    _exit(0);
}
