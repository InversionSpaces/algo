#include <iostream>
#include <vector>
#include <utility>

using namespace std;

template<typename T>
using vvec = vector<vector<T>>;

struct DualGraph {
    vvec<int> left;
    vvec<int> right;

    DualGraph(int n, int m) : left(n), right(m) {}
};

istream& operator>>(istream& in, DualGraph& g) {
    for (int i = 0; i < g.left.size(); ++i) {
        int j = 0;
        cin >> j;

        while (j) {
            g.left.at(i).push_back(j - 1);
            g.right.at(j - 1).push_back(i);

            cin >> j;
        }
    }

    return in;
}

struct Match {
    vector<pair<int, int>> edges;

    static Match get_max_on(const DualGraph& graph) {
        vector<int> right_to(graph.right.size(), -1);

        for (int v = 0; v < graph.left.size(); ++v) {
            vector<bool> left_used(graph.left.size(), false);
            Match::_try_get_max_on(graph, left_used, right_to, v);
        }

        Match retval = {};
        for (int v = 0; v < right_to.size(); ++v) {
            if (right_to.at(v) == -1) continue;
            retval.edges.emplace_back(right_to.at(v), v);
        }

        return retval;
    }

private:
    static bool _try_get_max_on(const DualGraph& graph, vector<bool>& left_used, vector<int>& right_to, const int v) {
        left_used.at(v) = true;

        for (const int& u: graph.left.at(v)) {
            int& to = right_to.at(u);
            if (to == -1 || (!left_used.at(to) && _try_get_max_on(graph, left_used, right_to, to))) {
                to = v;
                return true;
            }
        }

        return false;
    }
};

ostream& operator<<(ostream& out, const Match& match) {
    out << match.edges.size() << endl;

    for (const auto& [v, u]: match.edges)
        out << (v + 1) << " " << (u + 1) << endl;

    return out;
}

int main() {
    int n = 0, m = 0;
    cin >> n >> m;

    DualGraph graph(n, m);

    cin >> graph;
    cout << Match::get_max_on(graph);
}
