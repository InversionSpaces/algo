#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <stack>
#include <vector>

using namespace std;

class SuffTree {
    struct Node {
        long long li, ri;
        Node* parent;
        Node* link;

        map<char, Node*> go;

        long long len;
        bool reachable;

        Node(long long li=0, long long ri=0, Node* parent=nullptr, Node* link=nullptr) :
            li(li), ri(ri), parent(parent), link(link), len(0), reachable(false) {}

        long long size() const {
            return ri - li;
        }

        bool is_leaf() const {
            return go.empty();
        }
    };

    struct Position {
        Node* node;
        long long offset;

        static Position invalid() {
            return {nullptr, 0};
        }

        bool is_valid() const {
            return node != nullptr;
        }
    };

    string str;
    Node* root;
    const long long k;
    vector<long long> kpos;

    Position go(Position pos, long long l, long long r) const {
        while (l < r) {
            const char& strc = str.at(l);
            if (pos.offset == pos.node->size()) {
                if (pos.node->go.count(strc))
                    pos = {pos.node->go.at(strc), 0};
                else
                    return Position::invalid();
            }
            else {
                const char& edgec = str.at(pos.node->li + pos.offset);
                if (edgec != strc)
                    return Position::invalid();

                const long long& last = r - l;
                const long long& sum  = last + pos.offset;
                const long long& size = pos.node->size();
                if (sum < size)
                    return {pos.node, sum};

                l += size - pos.offset;
                pos.offset = size;
            }
        }

        return pos;
    }

    Node* split(Position pos) {
        if (pos.offset == pos.node->size())
            return pos.node;
        if (pos.offset == 0)
            return pos.node->parent;

        Node*       node    = pos.node;
        Node*       parent  = node->parent;
        const char& last_c  = str.at(node->li);
        const long long&  mid     = node->li + pos.offset;
        const char& split_c = str.at(mid);

        Node* inserted          = new Node(node->li, mid, parent);
        parent->go.at(last_c)   = inserted;
        node->li                = mid;
        node->parent            = inserted;
        inserted->go.insert({split_c, node});

        return inserted;
    }

    Node* link(Node* node) {
        if (node->link) return node->link;
        if (node == root) return root;

        Node* to        = link(node->parent);
        Position pos    = {to, to->size()};

        pos = go(pos, node->li + (node->parent == root), node->ri);
        return node->link = split(pos);
    }

    Position extend(Position pos, const long long idx) {
        while (1) {
            Position npos = go(pos, idx, idx + 1);
            if (npos.is_valid())
                return npos;

            Node* inner     = split(pos);
            Node* leaf      = new Node(idx, str.size(), inner);
            inner->go.insert({str.at(idx), leaf});

            pos.node = link(inner);
            pos.offset = pos.node->size();

            if (inner == root)
                return pos;
        }
    }

    void count_lengths(Node* node, long long len) {
        node->len = len + node->size() - (node->is_leaf());
        for (const auto& [_, child]: node->go)
            count_lengths(child, node->len);
    }

    int check(long long l, long long r) {
        for (int i = 0; i < k; ++i) {
            const long long p = kpos.at(i);
            if (l <= p && p < r)
                return i;
        }

        return -1;
    }

    vector<char> count_reachable(Node* node) {
        vector<char> reachable(k, false);
        for (const auto& [_, child]: node->go) {
            const int end = check(child->li, child->ri);
            if (end != -1) {
                reachable.at(end) = true;
                continue;
            }

            const auto& creachable = count_reachable(child);
            for (int i = 0; i < k; ++i)
                reachable.at(i) = reachable.at(i) || creachable.at(i);
        }

        bool result = true;
        for (int i = 0; i < k; ++i)
            result = result && reachable.at(i);

        node->reachable = result;

        return reachable;
    }

    void print(Node* node, long long depth, ostream& out) const {
        if (depth) out << endl;
        for (const auto& [_, child]: node->go) {
            string fmt =    string(depth, ' ') + "|-(" +
                            to_string(child->reachable) + ")->" +
                            str.substr(child->li, child->size());
            out << fmt;
            print(child, fmt.length() - 1, out);
        }
    }

    public:
    SuffTree(const vector<string>& words) :
        root(new Node),
        k(words.size()),
        kpos(words.size())
    {
        for (int i = 0; i < k; ++i) {
            str += words.at(i);
            kpos.at(i) = str.size();
            str += char('$' + i);
        }

        Position pos = {root, 0};
        for (long long i = 0; i < str.size(); ++i)
            pos = extend(pos, i);

        count_lengths(root, 0);
        count_reachable(root);
    }

    string lcs() const {
        Node* result = root;

        stack<Node*> st;
        st.push(root);

        while (!st.empty()) {
            Node* cur = st.top();
            st.pop();

            for (const auto& [_, next]: cur->go)
                if (next->reachable)
                    st.push(next);

            if (cur->len > result->len)
                result = cur;
        }

        string retval;
        while (result != root) {
            retval = str.substr(result->li, result->size()) + retval;
            result = result->parent;
        }

        return retval;
    }

    ~SuffTree() {
        stack<Node*> st;
        st.push(root);

        while (!st.empty()) {
            Node* cur = st.top();
            st.pop();

            for (const auto& [_, next]: cur->go) {
                st.push(next);
            }

            delete cur;
        }
    }

    friend ostream& operator<<(ostream& out, const SuffTree& st);
};

ostream& operator<<(ostream& out, const SuffTree& st) {
    st.print(st.root, 0, out);

    return out;
}

int main() {
    int k = 0;
    cin >> k;

    vector<string> words(k);
    for (auto& word: words)
        cin >> word;

    SuffTree st(words);
    //cout << st;
    cout << st.lcs();
}
