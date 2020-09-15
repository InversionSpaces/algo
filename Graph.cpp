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

class Graph {
private:
    vvec<int> neighbours;
    vvec<int> capacity;
    vvec<int> flow;

    const int vertex;
    const int edges;
    const int source;
    const int drain;

    pair<vector<int>, int> find_path(const vvec<int>& qualifier) {
        vector<int> from(vertex, -1);
        from.at(source) = source;
        stack<int> st;
        st.push(source);

        while (!st.empty()) {
            const int current = st.top();
            if (current == drain) break;
            st.pop();

            for (const int& to: neighbours.at(current)) {
                if (from.at(to) != -1) continue;
                if (qualifier.at(current).at(to) <= 0) continue;

                from.at(to) = current;
                st.push(to);
            }
        }

        if (from.at(drain) == -1) return {{}, -1};

        vector<int> path;
        int current = drain;
        int min_quality = std::numeric_limits<int>::max();

        while (current != source) {
            path.push_back(current);

            const int& prev = from.at(current);
            const int& quality = qualifier.at(prev).at(current);

            min_quality = quality < min_quality ? quality : min_quality;
            current = prev;
        }

        path.push_back(source);
        std::reverse(path.begin(), path.end());

        return {path, min_quality};
    }

public:
    Graph(const int n, const int m, const int s, const int t) :
        capacity(n, vector<int> (n, 0)),
        flow(n, vector<int> (n, 0)),
        neighbours(n),
        vertex(n), edges(m), source(s), drain(t) {}

    void maximize_flow() {
        while (true) {
            const auto& [path, min_cap] = find_path(capacity);
            if (min_cap == -1) return;

            for (int i = 0; i < path.size() - 1; ++i) {
                const int& u = path.at(i);
                const int& v = path.at(i + 1);

                flow.at(u).at(v) += min_cap;
                flow.at(v).at(u) -= min_cap;

                capacity.at(u).at(v) -= flow.at(u).at(v);
                capacity.at(v).at(u) -= flow.at(v).at(u);
            }
        }
    }

    vector<int> gen_path() {
        const auto path = find_path(flow).first;
        if (path.empty()) return path;

        for (int i = 0; i < path.size() - 1; ++i) {
            const int& u = path.at(i);
            const int& v = path.at(i + 1);

            flow.at(u).at(v) -= 1;
        }

        return path;
    }

    friend std::istream& operator>>(std::istream& in, Graph& g);
    friend std::ostream& operator<<(std::ostream& out, const Graph& g);
};

std::istream& operator>>(std::istream& in, Graph& g) {
    for (int i = 0; i < g.edges; ++i) {
        int u = 0, v = 0;
        in >> u >> v;
        --u; --v;

        g.capacity.at(u).at(v) += 1;
        g.neighbours.at(u).push_back(v);
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

std::ostream& operator<<(std::ostream& out, const vector<int>& v) {
    for (const int& i: v)
        out << (i + 1) << " ";
    out << std::endl;

    return out;
}

int main(int argc, char** argv) {
    int n = 0, m = 0, s = 0, t = 0;
    std::cin >> n >> m >> s >> t;

    Graph graph(n, m, s - 1, t - 1);
    std::cin >> graph;

    graph.maximize_flow();

    const auto& first = graph.gen_path();
    const auto& second = graph.gen_path();
    if (first.empty() || second.empty()) std::cout << "NO\n";
    else std::cout << "YES\n" << first << second;

    return 0;
}
