#include <vector>
#include <iostream>
#include <stack>
#include <limits>
#include <algorithm>
#include <utility>
#include <limits>
#include <functional>

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

struct Edge {
    lolo from;
    lolo to;
    lolo cap;
    lolo flow;
};

class Graph {
private:
    vvec<lolo> in_edges;
    vector<Edge> edges;

    const lolo source;
    const lolo drain;
    const lolo edges_count;
    const lolo vertex_count;

    vector<lolo> find_path(std::function<bool(const Edge&)> valid) const {
        vector<lolo> from(vertex_count, -1);

        stack<lolo> st;
        st.push(source);

        while (!st.empty()) {
            const lolo cur = st.top();
            //std::cout << cur + 1 << " -> ";
            st.pop();

            if (cur == drain)
                break;

            for (const auto& edge_id: in_edges.at(cur)) {
                const auto& edge = edges.at(edge_id);

                if (    !valid(edge) ||
                        from.at(edge.to) != -1 ||
                        edge.to == cur)
                    continue;

                from.at(edge.to) = edge_id;
                st.push(edge.to);
            }
        }

        //std::cout << std::endl;
        //std::cout.flush();

        return from;
    }

    bool push_flow() {
        const auto& from = find_path([] (const Edge& edge) {
                    return edge.cap > edge.flow;
                });

        //std::cout << "found path" << std::endl;
        //std::cout.flush();

        if (from.at(drain) == -1)
            return false;

        //std::cout << from;
        //std::cout.flush();

        lolo min_cap = std::numeric_limits<lolo>::max();
        for (lolo cur = drain; cur != source;) {
            const lolo& edge_id = from.at(cur);
            const lolo cap = edges.at(edge_id).cap;
            const lolo flow = edges.at(edge_id).flow;
            const lolo last = cap - flow;
            min_cap = min_cap > last ? last : min_cap;
            cur = edges.at(edge_id).from;
        }

        for (lolo cur = drain; cur != source;) {
            const lolo& edge_id = from.at(cur);
            edges.at(edge_id).flow += min_cap;
            edges.at(edge_id ^ 1).flow -= min_cap;
            cur = edges.at(edge_id).from;
        }

        return true;
    }

public:
    Graph(const lolo s, const lolo t, const lolo n, const lolo m) :
        source(s), drain(t), vertex_count(n), edges_count(m),
        in_edges(n)
    {}

    lolo maximize_flow() {
        while (push_flow()) {
            //std::cout << "pushing" << std::endl;
            //std::cout.flush();
        }

        return count_flow();
    }

    vector<lolo> gen_path() {
        const auto& from = find_path([] (const Edge& edge) {
                    return edge.flow > 0;
                });

        if (from.at(drain) == -1)
            return {};

        vector<lolo> path;

        for (lolo cur = drain; cur != source;) {
            path.push_back(cur);
            const lolo& edge_id = from.at(cur);
            edges.at(edge_id).flow -= 1;
            cur = edges.at(edge_id).from;
        }

        path.push_back(source);
        std::reverse(path.begin(), path.end());

        return path;
    }

    lolo count_flow() const {
        lolo flow = 0;
        for (const auto& edge_id: in_edges.at(source))
            flow += edges.at(edge_id).flow;
        return flow;
    }

    void insert_edge(const lolo u, const lolo v) {
        in_edges.at(u).push_back(edges.size());
        edges.push_back({u, v, 1, 0});
        in_edges.at(v).push_back(edges.size());
        edges.push_back({v, u, 0, 0});
    }

    friend std::istream& operator>>(std::istream& in, Graph& g);
};

std::istream& operator>>(std::istream& in, Graph& g) {
    for (lolo i = 0; i < g.edges_count; ++i) {
        lolo u = 0, v = 0;
        in >> u >> v;
        g.insert_edge(u - 1, v - 1);
    }

    return in;
}

int main() {
    std::cin.tie(0);
    std::ios_base::sync_with_stdio(false);

    lolo n = 0, m = 0, s = 0, t = 0;
    std::cin >> n >> m >> s >> t;

    Graph graph(s - 1, t - 1, n, m);

    std::cin >> graph;
    //std::cout << "done reading" << std::endl;
    //std::cout.flush();

    graph.maximize_flow();

    auto first = graph.gen_path();
    auto second = graph.gen_path();

    if (second.empty())
        std::cout << "NO";
    else
        std::cout << "YES\n" << first << second;

    return 0;
}
