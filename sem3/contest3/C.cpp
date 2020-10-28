#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <cassert>

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

class DNAResolver {
    struct Node {
        unordered_map<char, Node*> go;
        unordered_set<size_t> pending;

        bool is_terminate() const {
            return go.empty();
        }
    };

    struct DNA {
        const string str;
        size_t pos;
        Node* node;

        DNA(Node* node, string&& str) : node(node), str(str), pos(0) {}
    };

    vector<DNA> dnas;
    Node* root;

    bool update_dna(const size_t id) {
        DNA& dna = dnas[id];
        Node* current = dna.node;
        current->pending.erase(id);

        while (dna.pos < dna.str.length()) {
            const char& c = dna.str[dna.pos];

            if (current->go.count(c)) {
                current = current->go[c];
                ++dna.pos;
            }
            else if (current->is_terminate() && current != root)
                current = root;
            else break;
        }

        if (dna.pos == dna.str.length() && current->is_terminate())
            return true;

        if (dna.pos < dna.str.length()) {
            current->pending.insert(id);
            dna.node = current;
        }
        else dna.node = nullptr;

        return false;
    }

public:
    DNAResolver() : root(new Node) {}

    vector<size_t> add_dna(string&& str) {
        const size_t id = dnas.size();
        root->pending.insert(id);
        dnas.emplace_back(root, move(str));
        if (update_dna(id)) return {id};
        return {};
    }

    vector<size_t> add_gene(const string& str) {
        vector<size_t> to_update;

        Node* current = root;
        for (const auto& c: str) {
            if (!current->go.count(c)) {
                current->go.insert({c, new Node});

                for (const int& id: current->pending) {
                    const DNA& dna = dnas[id];
                    if (dna.str[dna.pos] == c) // HERE
                        to_update.push_back(id);
                }
            }

            current = current->go.at(c);
        }

        vector<size_t> ans;
        for (const size_t& id: to_update)
            if (update_dna(id))
                ans.push_back(id);

        return ans;
    }

    ~DNAResolver() {
        queue<Node*> _queue;
        _queue.push(root);

        while (!_queue.empty()) {
            Node* cur = _queue.front();
            _queue.pop();

            for (const auto& [_, child]: cur->go)
                _queue.push(child);

            delete cur;
        }
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    size_t n = 0;
    cin >> n;

    DNAResolver dnares;

    size_t k = 0;
    while (n--) {
        char op = 0;
        cin >> op;

        string str;
        cin >> str;

        k %= str.length();
        if (k) str = str.substr(k, str.length() - k) + str.substr(0, k);

        const vector<size_t>& ans = op == '+' ?
            dnares.add_gene(str) : dnares.add_dna(move(str));

        k = ans.size();
        cout << k << " ";
        for (const auto& id: ans)
            cout << (id + 1) << " ";
        cout << "\n";
    }
}
