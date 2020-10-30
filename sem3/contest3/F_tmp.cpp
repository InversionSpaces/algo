#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <map>

#include <cassert>

using std::unordered_map;
using std::vector;
using std::string;
using std::queue;
using std::map;

inline vector<vector<long long>> mult(
        const vector<vector<long long>>& a,
        const vector<vector<long long>>& b,
        const size_t mod
        ) {
    const size_t size = a.size();
    vector<vector<long long>> m(size, vector<long long>(size, 0));

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j) {
            long long sum = 0;
            for (size_t k = 0; k < size; ++k) {
                sum = (sum + a.at(i).at(k) * b.at(k).at(j)) % mod;
            }
            m.at(i).at(j) = sum;
        }

    return m;
}

inline vector<vector<long long>> power(
        vector<vector<long long>> a,
        size_t n,
        const size_t mod
        ) {
    auto res = a;
    --n;
    while (n) {
        if (n & 1)
            res = mult(res, a, mod);
        a = mult(a, a, mod);
        n >>= 1;
    }

    return res;
}

inline void print(const vector<vector<long long>>& m) {
    for (const auto& r: m) {
        for (const auto& i: r)
            std::cout << i << '\t';
        std::cout << '\n';
    }
}

class Trie {
    struct Node {
        const long long id;
        const Node* parent;
        const Node* suff_link;

        const char last_char;
        bool is_terminate;

        unordered_map<char, Node*> go;

        Node(const long long id=0, const Node* parent=nullptr, char last_char=0) :
            id(id),
            parent(parent),
            last_char(last_char),
            suff_link(nullptr),
            is_terminate(false)
        {}
    };

    Node* const root = nullptr;
    vector<Node*> id_to_node;

    inline Node* get_or_create(Node* current, const char c) {
        if (!current->go.count(c)) {
            Node* inserted = new Node(id_to_node.size(), current, c);
            id_to_node.push_back(inserted);
            current->go.insert({c, inserted});
        }
        return current->go.at(c);
    }

    inline void add_string(const string& str, const long long id) {
        Node* current = root;
        for (const auto& c: str)
            current = get_or_create(current, c);
        current->is_terminate = true;
    }

    inline void calculate_suff_links() {
        queue<Node*> _queue;
        _queue.push(root);

        while (!_queue.empty()) {
            Node* const cur = _queue.front();
            _queue.pop();

            for (const auto& [_, child]: cur->go)
                _queue.push(child);

            if (cur == root || cur->parent == root) {
                cur->suff_link = root;
                continue;
            }

            const Node* suff = cur->parent->suff_link;
            while ( suff != root &&
                    suff->go.count(cur->last_char) == 0) {
                suff = suff->suff_link;
            }

            if (suff->go.count(cur->last_char))
                cur->suff_link = suff->go.at(cur->last_char);
            else
                cur->suff_link = root;
        }
    }

    const Node* jump(const Node* cur, const char& c) const {
        while (cur != root && cur->go.count(c) == 0)
            cur = cur->suff_link;

        if (cur->go.count(c))
            return cur->go.at(c);

        return root;
    }

public:
    Trie(const vector<string>& vocab) :
        root(new Node),
        id_to_node{root}
    {
        root->parent = root;

        for (long long i = 0; i < vocab.size(); ++i)
            add_string(vocab.at(i), i);

        calculate_suff_links();
    }

    inline vector<vector<long long>> get_matrix(const string& alphabet) const {
        const size_t vertex_count = id_to_node.size();
        vector<vector<long long>> matrix(
                vertex_count,
                vector<long long>(vertex_count, 0)
                );

        for (size_t id = 0; id < id_to_node.size(); ++id) {
            const Node* node = id_to_node.at(id);

            if (node->is_terminate)
                matrix.at(id).at(id) += alphabet.size();
            else for (const auto& c: alphabet) {
                const size_t to_id = jump(node, c)->id;
                matrix.at(id).at(to_id) += 1;
            }
        }

        return matrix;
    }

    long long count_bad_words(
            const long long n,
            const string& alphabet,
            const size_t mod
            ) const {
        auto matrix = get_matrix(alphabet);
        //prlong long(matrix);
        auto pow = power(
                matrix,
                n,
                mod);
        //prlong long(pow);

        size_t ans = 0;
        for (size_t id = 0; id < id_to_node.size(); ++id)
            if (id_to_node.at(id)->is_terminate)
                ans = (ans + pow.at(0).at(id)) % mod;

        return ans;
    }

    ~Trie() {
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
    long long n = 0, k = 0;
    std::cin >> n >> k;

    vector<string> vocab(k);
    for (auto& s: vocab)
        std::cin >> s;

    Trie trie(vocab);
    std::cout << trie.count_bad_words(n, "qwertyuiopasdfghjklzxcvbnm", 10000);
}
