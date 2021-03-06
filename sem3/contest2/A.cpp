#include <vector>
#include <iostream>
#include <stack>
#include <limits>
#include <algorithm>
#include <utility>

using std::pair;
using std::vector;
using std::stack;

template<typename T>
using vvec = vector<vector<T>>;

using lolo = long long;

std::ostream& operator<<(std::ostream& out, const vector<lolo>& v) {
    for (const lolo& i: v)
        out << (i + 1) << " ";
    out << std::endl;

    return out;
}

class Graph {
private:
    vvec<lolo> neighbours;
    vvec<lolo> capacity;
    vvec<lolo> flow;

    const lolo vertex;
    const lolo edges;
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
    Graph(const lolo n, const lolo m, const lolo s, const lolo t) :
        capacity(n, vector<lolo> (n, 0)),
        flow(n, vector<lolo> (n, 0)),
        neighbours(n),
        vertex(n), edges(m), source(s), drain(t) {}

    lolo maximize_flow() {
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

        return count_flow();
    }

    vector<lolo> gen_path() {
        const auto path = find_path(flow).first;
        if (path.empty()) return path;

        for (lolo i = 0; i < path.size() - 1; ++i) {
            const lolo& u = path.at(i);
            const lolo& v = path.at(i + 1);

            flow.at(u).at(v) -= 1;
        }

        return path;
    }

    lolo count_flow() const {
        lolo cur_flow = 0;
        for (const auto& v: neighbours.at(source))
            cur_flow += flow.at(source).at(v);
        return cur_flow;
    }

    friend std::istream& operator>>(std::istream& in, Graph& g);
    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};

std::istream& operator>>(std::istream& in, Graph& g) {
    for (lolo i = 0; i < g.edges; ++i) {
        lolo u = 0, v = 0, c = 0;
        in >> u >> v >> c;
        --u; --v;

        g.capacity.at(u).at(v) += c;
        auto& un = g.neighbours.at(u);
        if (find(un.begin(), un.end(), v) == un.end()) {
            un.push_back(v);
            g.neighbours.at(v).push_back(u);
        }
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const Graph& g) {
    for (const auto& row: g.flow) {
        for (const auto& f: row)
            std::cout << f << '\t';
        std::cout << std::endl;
    }

    return out;
}


int main() {
    lolo n = 0, m = 0;
    std::cin >> n >> m;

    Graph graph(n, m, 0, n - 1);
    std::cin >> graph;

    std::cout << graph.maximize_flow();

    return 0;
}
