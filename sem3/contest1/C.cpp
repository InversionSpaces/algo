#include <iostream>
#include <vector>
#include <utility>
#include <cmath>

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

struct Point {
    const int time;
    const int x, y;
};

inline int time_between(const int x1, const int y1, const int x2, const int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

inline bool reachable(const Point& end, const Point& start) {
    return time_between(start.x, start.y, end.x, end.y) < start.time - end.time;
}

DualGraph gen_graph(const vector<Point>& starts, const vector<Point>& ends) {
    DualGraph retval(ends.size(), starts.size());

    for (int i = 0; i < ends.size(); ++i)
        for (int j = 0; j < starts.size(); ++j)
            if (reachable(ends.at(i), starts.at(j)))
                    retval.add(i, j);

    return retval;
}

int main() {
    int M = 0;
    cin >> M;

    vector<Point> starts, ends;
    starts.reserve(M);
    ends.reserve(M);

    for (int i = 0; i < M; ++i) {
        int h = 0, m = 0, xs = 0, ys = 0, xe = 0, ye = 0;
        scanf("%d:%d %d %d %d %d", &h, &m, &xs, &ys, &xe, &ye);

        const int time_s = h * 60 + m;
        const int time_e = time_s + time_between(xs, ys, xe, ye);

        starts.push_back({time_s, xs, ys});
        ends.push_back({time_e, xe, ye});
    }

    const DualGraph graph = gen_graph(starts, ends);
    const Match match = Match::get_max_on(graph);

    cout << (M - match.edges.size());
}
