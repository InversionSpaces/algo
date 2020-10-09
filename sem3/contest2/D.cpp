#include <vector>
#include <string>
#include <iostream>
#include <stack>
#include <limits>
#include <algorithm>
#include <utility>
#include <numeric>

using std::pair;
using std::vector;
using std::stack;
using std::string;

template<typename T>
using vvec = vector<vector<T>>;

class Graph {
private:
    vvec<long> neighbours;
    vvec<long> capacity;
    vvec<long> flow;

    const long vertex;
    const long source;
    const long drain;

    pair<vector<long>, long> find_path(const vvec<long>& qualifier) const {
        vector<long> from(vertex, -1);
        from.at(source) = source;
        stack<long> st;
        st.push(source);

        while (!st.empty()) {
            const long current = st.top();
            if (current == drain) break;
            st.pop();

            for (const long& to: neighbours.at(current)) {
                if (from.at(to) != -1) continue;
                if (qualifier.at(current).at(to) <= 0) continue;

                from.at(to) = current;
                st.push(to);
            }
        }

        if (from.at(drain) == -1) return {{}, -1};

        vector<long> path;
        long current = drain;
        long min_quality = std::numeric_limits<long>::max();

        while (current != source) {
            path.push_back(current);

            const long& prev = from.at(current);
            const long& quality = qualifier.at(prev).at(current);

            min_quality = quality < min_quality ? quality : min_quality;
            current = prev;
        }

        path.push_back(source);
        std::reverse(path.begin(), path.end());

        return {path, min_quality};
    }

public:
    Graph(const long n, const long s, const long t) :
        capacity(n, vector<long> (n, 0)),
        flow(n, vector<long> (n, 0)),
        neighbours(n),
        vertex(n),
        source(s),
        drain(t) {}

    void set_capacity(const long u, const long v, const long cap) {
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
        for (long u = 0; u < vertex; ++u)
            for (long v = 0; v < vertex; ++v)
                if (capacity.at(u).at(v))
                    std::cout << u << "->" << v << ": " << capacity.at(u).at(v) << std::endl;
        */
        while (true) {
            const auto& [path, min_cap] = find_path(capacity);
            if (min_cap == -1) break;

            for (long i = 0; i < path.size() - 1; ++i) {
                const long& u = path.at(i);
                const long& v = path.at(i + 1);

                flow.at(u).at(v) += min_cap;
                flow.at(v).at(u) -= min_cap;

                capacity.at(u).at(v) -= min_cap;
                capacity.at(v).at(u) += min_cap;
            }
        }

        /*
        std::cout << "flow after: " << std::endl;
        for (long u = 0; u < vertex; ++u)
            for (long v = 0; v < vertex; ++v)
                if (flow.at(u).at(v))
                    std::cout << u << "->" << v << ": " << flow.at(u).at(v) << std::endl;
        */
    }

    long count_flow() const {
        long cur_flow = 0;
        for (const auto& v: neighbours.at(source))
            cur_flow += flow.at(source).at(v);
        return cur_flow;
    }
};

int main() {
    long n = 0, m = 0;
    std::cin >> n >> m;

    vector<string> paper(n);
    for (auto& line:paper)
        std::cin >> line;

    long count = 0;
    vvec<long> index(n, vector<long> (m, 0));
    for (long i = 0; i < n; ++i)
        for (long j = 0; j < m; ++j)
            if (paper.at(i).at(j) != '.')
                index.at(i).at(j) = count++ + 1;

    if (count == 0) {
        std::cout << "Invalid";
        return 0;
    }

    const long source = 0;
    const long drain = count + 1;

    Graph graph(count + 2, source, drain);
    const vector<char> elems = {'H', 'O', 'N', 'C'};

    long in_cons_count = 0;
    long out_cons_count = 0;
    for (long i = 0; i < n; ++i)
        for (long j = 0; j < m; ++j) {
            bool is_black = (i + j) % 2;

            const char& symbol = paper.at(i).at(j);
            if (symbol == '.') continue;
            const long& idx = index.at(i).at(j);

            const long cons = std::distance(
                    elems.begin(),
                    find(   elems.begin(),
                            elems.end(),
                            symbol
                            )
                    ) + 1;

            if (is_black) {
                in_cons_count += cons;
                //std::cout << source << " -> " << idx << " - " << cons << std::endl;
                graph.set_capacity(source, idx, cons);
            }
            else {
                out_cons_count += cons;
                //std::cout << idx << " -> " << drain << " - " << cons << std::endl;
                graph.set_capacity(idx, drain, cons);
            }

            if (!is_black) continue;

            for (long dh = -1; dh <= 1; ++dh)
                for (long dv = -1; dv <= 1; ++dv) {
                    if ((!dv && !dh) || (dv && dh)) continue;
                    const long v = (i + dv);
                    if (v == n || v < 0) continue;
                    const long h = (j + dh);
                    if (h == m || h < 0) continue;

                    const char& nsymbol = paper.at(v).at(h);
                    if (nsymbol == '.') continue;
                    const long& nidx = index.at(v).at(h);

                    //std::cout << idx << " -> " << nidx << " - " << cons << std::endl;
                    graph.set_capacity(idx, nidx, 1);
                }
        }

    graph.maximize_flow();
    //std::cout << out_cons_count << "-" << in_cons_count << "-" << graph.count_flow() << std::endl;
    if (out_cons_count == in_cons_count && in_cons_count == graph.count_flow())
        std::cout << "Valid";
    else
        std::cout << "Invalid";

    return 0;
}
