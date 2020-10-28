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

class Trie {
    struct Node {
        const Node* parent;
        const Node* suff_link;
        const Node* term_link;

        const char last_char;
        vector<int> ids;

        unordered_map<char, Node*> go;

        Node(const Node* parent=nullptr, char last_char=0) :
            parent(parent),
            last_char(last_char),
            suff_link(nullptr),
            term_link(nullptr)
        {}

        inline bool is_terminate() const {
            return !ids.empty();
        }
    };

    Node* const root = nullptr;

    inline void add_string(const string& str, const int id) {
        Node* current = root;
        for (const auto& c: str) {
            if (current->go.count(c) == 0)
                current->go.insert({c, new Node(current, c)});
            current = current->go.at(c);
        }
        current->ids.push_back(id);
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

    inline void calculate_term_links() {
        queue<Node*> _queue;
        _queue.push(root);

        while (!_queue.empty()) {
            Node* const cur = _queue.front();
            _queue.pop();

            for (const auto& [_, child]: cur->go)
                _queue.push(child);

            if (cur == root)
                cur->term_link = root;
            else if (cur->is_terminate())
                cur->term_link = cur;
            else
                cur->term_link = cur->suff_link->term_link;
        }
    }

    const Node* jump(const Node* cur, const char& c) const {
        while (cur != root && cur->go.count(c) == 0)
            cur = cur->suff_link;

        if (cur->go.count(c))
            return cur->go.at(c);

        return root;
    }

    inline vector<int> gen_terms(const Node* cur) const {
        vector<int> ans;

        do {
            assert(cur != nullptr);
            if (cur->is_terminate()) {
                ans.insert(ans.end(), cur->ids.begin(), cur->ids.end());
                cur = cur->suff_link;
            }
            else cur = cur->term_link;
        } while (cur != root);

        return ans;
    }

public:
    Trie(const vector<string>& vocab) : root(new Node) {
        root->parent = root;

        for (int i = 0; i < vocab.size(); ++i)
            add_string(vocab.at(i), i);

        calculate_suff_links();
        calculate_term_links();
    }

    unordered_map<int, vector<size_t>> process(const string& text) {
        unordered_map<int, vector<size_t>> ans;
        const Node* current = root;

        auto update = [&ans] (const size_t pos, const vector<int>& ids) {
            for (const auto& id: ids)
                ans[id].push_back(pos);
        };

        for (size_t i = 0; i < text.size(); ++i) {
            const char& c = text.at(i);
            current = jump(current, c);
            update(i, gen_terms(current));
        }

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
    string text;
    std::cin >> text;

    int n = 0;
    std::cin >> n;

    vector<string> vocab(n);
    for (auto& s: vocab)
        std::cin >> s;

    Trie trie(vocab);

    const auto& ans = trie.process(text);

    for (size_t i = 0; i < vocab.size(); ++i) {
        if (!ans.count(i)) {
            std::cout << "0\n";
            continue;
        }
        const string& str = vocab.at(i);
        const auto& occurs = ans.at(i);
        std::cout << occurs.size() << " ";
        for (const auto& pos: occurs)
            std::cout << (pos + 2 - str.size()) << " ";
        std::cout << std::endl;
    }
}
