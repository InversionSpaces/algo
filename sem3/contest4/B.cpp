#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <stack>

using namespace std;

class SuffTree {
    struct Node {
        long long li, ri;
        Node* parent;
        Node* link;

        map<char, Node*> go;

        long long substrs;

        Node(long long li=0, long long ri=0, Node* parent=nullptr, Node* link=nullptr) :
            li(li), ri(ri), parent(parent), link(link), substrs(0) {}

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

    const string str;
    Node* root;

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

    long long count_substrs(Node* node) {
        for (const auto& [_, child]: node->go)
            node->substrs += child->size() + (
                    child->is_leaf() ? -1 : count_substrs(child)
                );

        return node->substrs;
    }

    string find_kth_substr(Node* node, long long k) const {
        string retval;
        while (node) for (const auto& [_, child]: node->go) {
            const long long edge_size = child->size() - (child->is_leaf());
            if (k <= edge_size)
                return retval + str.substr(child->li, k);
            k -= edge_size;

            if (k <= child->substrs) {
                retval += str.substr(child->li, edge_size);
                node = child;
                break;
            }
            k -= child->substrs;
        }

        return retval;
    }

    void print(Node* node, long long depth, ostream& out) const {
        if (!depth) out << node->substrs;
        out << endl;
        for (const auto& [_, child]: node->go) {
            string fmt =    string(depth, ' ') + "|-(" +
                            to_string(child->substrs) + ")-" +
                            str.substr(child->li, child->size());
            out << fmt;
            print(child, fmt.length() - 1, out);
        }
    }

    public:
    SuffTree(const string& word) :
        str(word + '$'),
        root(new Node)
    {
        Position pos = {root, 0};
        for (long long i = 0; i < str.size(); ++i)
            pos = extend(pos, i);

        count_substrs(root);
    }

    string kth_substr(long long k) const {
        k = k > root->substrs ? root->substrs : k;

        return find_kth_substr(root, k);
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
    string str;
    cin >> str;
    SuffTree st(str);
    long long k = 0;
    cin >> k;
    cout << st.kth_substr(k);

    /*
    for (long long k = 0; k < str.size() * str.size() + 2; ++k) {
        cout << k << ": " << st.kth_substr(k) << endl;
    }
    */
}
