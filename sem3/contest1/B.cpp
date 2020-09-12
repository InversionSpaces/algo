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

    void add(const int left_idx, const int right_idx) {
        left.at(left_idx).push_back(right_idx);
        right.at(right_idx).push_back(left_idx);
    }
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
    freopen("dominoes.in", "r", stdin);
    freopen("dominoes.out", "w", stdout);

    int n = 0, m = 0, a = 0, b = 0;
    cin >> n >> m >> a >> b;

    vvec<int> board(n, vector<int>(m));
    for (auto& row: board)
        for (auto& pos: row) {
            char c = 0;
            cin >> c;
            pos = c == '.' ? -1 : 0;
        }

    const int all = n * m;
    const int white = (all + 1) / 2;
    const int black = all / 2;

    int white_idx = 0, black_idx = 0;
    int empty_count = 0;
    DualGraph graph(white, black);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j) {
            if (board.at(i).at(j)) continue;
            ++empty_count;

            const bool is_black = (i + j) % 2;

            if (is_black) {
                board.at(i).at(j) = black_idx;
                ++black_idx;
            }
            else {
                board.at(i).at(j) = white_idx;
                ++white_idx;
            }
        }

    if (2 * b < a) {
        cout << b * empty_count;

        return 0;
    }

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j) {
            const int& cur_idx = board.at(i).at(j);
            if (cur_idx == -1) continue;

            const bool is_black = (i + j) % 2;
            const int& right_idx = i < (n - 1) ? board.at(i + 1).at(j) : -1;
            const int& bottom_idx = j < (m - 1) ? board.at(i).at(j + 1) : -1;

            if (is_black) {
                if (right_idx != -1) graph.add(right_idx, cur_idx);
                if (bottom_idx != -1) graph.add(bottom_idx, cur_idx);
            }
            else {
                if (right_idx != -1) graph.add(cur_idx, right_idx);
                if (bottom_idx != -1) graph.add(cur_idx, bottom_idx);
            }
        }

    const int max_a = Match::get_max_on(graph).edges.size();
    cout << max_a * a + (empty_count - 2 * max_a) * b;

    return 0;
}
