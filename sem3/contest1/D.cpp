#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <utility>
#include <functional>

using namespace std;

template<typename T>
using vvec = vector<vector<T>>;

struct DualGraph;
struct Match;

ostream& operator<<(ostream& out, const Match& match);
ostream& operator<<(ostream& out, const DualGraph& g);

vector<int> vec_inversed(const vector<int>& vec, const int n) {
    vector<bool> take(n, true);
    for (const int& v: vec)
        take.at(v) = false;

    vector<int> retval;
    retval.reserve(n - vec.size());
    for (int i = 0; i < n; ++i)
        if (take.at(i)) 
            retval.push_back(i);

    return retval;
}

struct DualGraph {
    vvec<int> left;
    vvec<int> right;

    DualGraph(int n, int m) : left(n), right(m) {}

    DualGraph inversed() const;
    DualGraph orientate(const Match& match) const;

    pair<vector<int>, vector<int>> get_min_vc() const;
    pair<vector<int>, vector<int>> get_max_is() const;

    inline void add_l2r(const int left_idx, const int right_idx) {
        left.at(left_idx).push_back(right_idx);
    }

    inline void add_r2l(const int right_idx, const int left_idx) {
        right.at(right_idx).push_back(left_idx);
    }

    inline void add(const int left_idx, const int right_idx) {
        add_l2r(left_idx, right_idx);
        add_r2l(right_idx, left_idx);
    }
};

struct Match {
    vector<pair<int, int>> edges;

    static Match get_max_on(const DualGraph& graph);

private:
    static bool _try_get_max_on(const DualGraph& graph, vector<bool>& left_used, vector<int>& right_to, const int v);
};

DualGraph DualGraph::inversed() const {
    DualGraph retval(0, 0);

    for (const auto& v: left)
        retval.left.push_back(vec_inversed(v, right.size()));

    for (const auto& v: right)
        retval.right.push_back(vec_inversed(v, left.size()));

    return retval;
}

DualGraph DualGraph::orientate(const Match& match) const {
    vector<int> left2right(left.size(), -1);
    for (const auto& [l, r]: match.edges)
        left2right.at(l) = r;

    DualGraph retval(left.size(), right.size());

    for (int l = 0; l < left.size(); ++l)
        for (const int& r: left.at(l))
            if (left2right.at(l) == r) retval.add_r2l(r, l);
            else retval.add_l2r(l, r);

    return retval;
}

pair<vector<int>, vector<int>> DualGraph::get_min_vc() const {
    const Match match = Match::get_max_on(*this);

    auto gen = [] (const int n) -> vector<int> {
        vector<int> retval(n);
        for (int i = 0; i < n; ++i) retval.at(i) = i;
        return retval;
    };

    if (match.edges.size() == left.size())
        return {gen(left.size()), {}};

    if (match.edges.size() == right.size())
        return {{}, gen(right.size())};

    vector<int> starts;
    for (const auto& [l, r]: match.edges)
        starts.push_back(l);
    starts = vec_inversed(starts, left.size());

    const DualGraph graph = this->orientate(match);

    vector<char> Lplus(left.size(), false);
    vector<char> Rplus(right.size(), false);

    function<void(int)> dfs = [&] (const int v) -> void {
        if (Lplus.at(v)) return;
        Lplus.at(v) = true;

        for (const int& u: graph.left.at(v)) {
            Rplus.at(u) = true;
            for (const int& t: graph.right.at(u)) dfs(t);
        }
    };

    for (const auto& v: starts) dfs(v);

    vector<int> vLminus;
    for (int v = 0; v < left.size(); ++v)
        if (!Lplus.at(v)) vLminus.push_back(v);

    vector<int> vRplus;
    for (int v = 0; v < right.size(); ++v)
        if (Rplus.at(v)) vRplus.push_back(v);

    return {vLminus, vRplus};
}

pair<vector<int>, vector<int>> DualGraph::get_max_is() const {
    const auto vc = get_min_vc();

    return {    vec_inversed(vc.first, left.size()),
                vec_inversed(vc.second, right.size())   };
}

Match Match::get_max_on(const DualGraph& graph) {
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

bool Match::_try_get_max_on(const DualGraph& graph, vector<bool>& left_used, vector<int>& right_to, const int v) {
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

istream& operator>>(istream& in, DualGraph& g) {
    for (int i = 0; i < g.left.size(); ++i) {
        int j = 0;
        cin >> j;

        while (j) {
            g.add(i, j - 1);
            cin >> j;
        }
    }

    return in;
}

ostream& operator<<(ostream& out, const DualGraph& g) {
    cout << "DualGraph:" << endl;

    cout << "Left: " << endl;
    for (int v = 0; v < g.left.size(); ++v) {
        cout << v << ": ";
        for (const int u: g.left.at(v))
            cout << u << " ";
        cout << endl;
    }

    cout << "Right: " << endl;
    for (int v = 0; v < g.right.size(); ++v) {
        cout << v << ": ";
        for (const int u: g.right.at(v))
            cout << u << " ";
        cout << endl;
    }

    cout << "end" << endl;

    return out;
}

ostream& operator<<(ostream& out, const Match& match) {
    out << match.edges.size() << endl;

    for (const auto& [v, u]: match.edges)
        out << (v + 1) << " " << (u + 1) << endl;

    return out;
}

int main() {
    int k = 0;
    cin >> k;

    while (k--) {
        int n = 0, m = 0;
        cin >> n >> m;

        DualGraph graph(n, m);
        cin >> graph;

        const auto res = graph.inversed().get_max_is();
        cout << res.first.size() << " " << res.second.size() << endl;
        for (const int& v: res.first)
            cout << v + 1 << " ";
        cout << endl;
        for (const int& v: res.second)
            cout << v + 1 << " ";
        cout << endl << endl;
    }
}
