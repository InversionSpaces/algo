#include <vector>
#include <iostream>
#include <stack>
#include <limits>
#include <algorithm>
#include <utility>
#include <numeric>

using std::pair;
using std::vector;
using std::stack;

template<typename T>
using vvec = vector<vector<T>>;

using lolo = long long;

class Graph {
private:
    vvec<lolo> neighbours;
    vvec<lolo> capacity;
    vvec<lolo> flow;

    const lolo vertex;
    const lolo source;
    const lolo drain;

    pair<vector<lolo>, lolo> find_path(const vvec<lolo>& qualifier) const {
        vector<lolo> from(vertex, -1);
        from.at(source) = source;
        stack<lolo> st;
        st.push(source);

        while (!st.empty()) {
            const lolo current = st.top();
            if (current == drain) break;
            st.pop();

            for (const lolo& to: neighbours.at(current)) {
                if (from.at(to) != -1) continue;
                if (qualifier.at(current).at(to) <= 0) continue;

                from.at(to) = current;
                st.push(to);
            }
        }

        if (from.at(drain) == -1) return {{}, -1};

        vector<lolo> path;
        lolo current = drain;
        lolo min_quality = std::numeric_limits<lolo>::max();

        while (current != source) {
            path.push_back(current);

            const lolo& prev = from.at(current);
            const lolo& quality = qualifier.at(prev).at(current);

            min_quality = quality < min_quality ? quality : min_quality;
            current = prev;
        }

        path.push_back(source);
        std::reverse(path.begin(), path.end());

        return {path, min_quality};
    }

public:
    Graph(const lolo n, const lolo s, const lolo t) :
        capacity(n, vector<lolo> (n, 0)),
        flow(n, vector<lolo> (n, 0)),
        neighbours(n),
        vertex(n),
        source(s),
        drain(t) {}

    void set_capacity(const lolo u, const lolo v, const lolo cap) {
        capacity.at(u).at(v) = cap;
        auto& un = neighbours.at(u);
        if (find(un.begin(), un.end(), v) == un.end()) {
            un.push_back(v);
            neighbours.at(v).push_back(u);
        }
    }

    void maximize_flow() {
        /*
        std::cout << "capacity before it: " << std::endl;
        for (lolo u = 0; u < vertex; ++u)
            for (lolo v = 0; v < vertex; ++v)
                if (capacity.at(u).at(v))
                    std::cout << u << "->" << v << ": " << capacity.at(u).at(v) << std::endl;
        */
        while (true) {
            const auto& [path, min_cap] = find_path(capacity);
            if (min_cap == -1) break;

            for (lolo i = 0; i < path.size() - 1; ++i) {
                const lolo& u = path.at(i);
                const lolo& v = path.at(i + 1);

                flow.at(u).at(v) += min_cap;
                flow.at(v).at(u) -= min_cap;

                capacity.at(u).at(v) -= min_cap;
                capacity.at(v).at(u) += min_cap;
            }
        }

        /*
        std::cout << "flow after: " << std::endl;
        for (lolo u = 0; u < vertex; ++u)
            for (lolo v = 0; v < vertex; ++v)
                if (flow.at(u).at(v))
                    std::cout << u << "->" << v << ": " << flow.at(u).at(v) << std::endl;
        */
    }

    lolo count_flow() const {
        lolo cur_flow = 0;
        for (const auto& v: neighbours.at(source))
            cur_flow += flow.at(source).at(v);
        return cur_flow;
    }
};

class TeaPartySolver {
private:
    const lolo people;
    const lolo kinds;

    vector<lolo> count;
    vvec<lolo> favorite;

    bool check_possible(const lolo n) const {
        const lolo vertex = 2 + people + kinds;
        const lolo source = 0;
        const lolo drain = vertex - 1;

        Graph g = Graph(vertex, source, drain);

        const lolo people_start = source + 1;
        const lolo people_end = people_start + people;
        const lolo kinds_start = people_end;
        const lolo kinds_end = kinds_start + kinds;

        for (lolo p = people_start; p < people_end; ++p) {
            g.set_capacity(source, p, n);

            for (const auto& k: favorite.at(p - people_start))
                g.set_capacity(p, k + kinds_start, count.at(k));
        }

        for (lolo k = kinds_start; k < kinds_end; ++k)
            g.set_capacity(k, drain, count.at(k - kinds_start));

        g.maximize_flow();

        return g.count_flow() == people * n;
    }

public:
    TeaPartySolver(const int n, const int m) : 
        people(n),
        kinds(m),
        count(m),
        favorite(n) {}

    lolo solve() {
        lolo l = 0;
        lolo r = std::accumulate(count.begin(), count.end(), 0) / people + 1;

        while (l + 1 < r) {
            const lolo m = (l + r) / 2;
            if (check_possible(m)) l = m;
            else r = m;
        }

        return l;
    }

    friend std::istream& operator>>(std::istream& in, TeaPartySolver& tps);
};


std::istream& operator>>(std::istream& in, TeaPartySolver& tps) {
    for (auto& c: tps.count)
        in >> c;

    for (auto& f: tps.favorite) {
        lolo size = 0;
        in >> size;
        f.resize(size);
        for (auto& k: f) {
            in >> k;
            --k;
        }
    }

    return in;
}

int main() {
    lolo n = 0, m = 0;
    std::cin >> n >> m;

    TeaPartySolver tps(n, m);
    std::cin >> tps;

    std::cout << tps.solve();

    return 0;
}
