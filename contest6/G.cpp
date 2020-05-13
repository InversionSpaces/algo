#include <vector>
#include <iostream>
#include <queue>
#include <utility>
#include <algorithm>

using namespace std;

typedef long long ll;
typedef vector<vector<ll>> graph_t;
typedef pair<ll, ll> edge_t;

struct BridgesFinder {
	const graph_t& graph;
	vector<edge_t> bridges;

	ll timer;
	vector<char> was;
	vector<ll> in;
	vector<ll> up;

	void dfs(ll v, ll p) {
		was[v] = true;
		in[v] = up[v] = timer++;
		ll cnt = 0;
	
		for (const ll& to: graph[v]) {
			if (to == p) {
				cnt++;
				continue;
			}
			if (was[to]) 
				up[v] = min(up[v], in[to]);
			else {
				dfs(to, v);
				up[v] = min(up[v], up[to]);
			}
		}
	
		if (p != -1 && up[v] > in[p] && cnt < 2)
			bridges.push_back({v, p});
	}

	void find() {
		was.assign(graph.size(), false);
		in.resize(graph.size()); 
		up.resize(graph.size());
		timer = 0;

		bridges.clear();

		dfs(0, -1);
	}
};

struct CycleGenerator {
	const graph_t& graph;
	ll answer;

	ll dfs(const ll v, const ll p) {
		ll first = 0;
		ll second = 0;

		for (const ll& to: graph[v]) {
			if (to == p) continue;
			ll child = dfs(to, v);

			if (child + 1 > first) {
				second = first;
				first = child + 1;
			}
			else if (child + 1 > second) {
				second = child + 1;
			}
		}

		if (first + second > answer)
			answer = first + second;

		return first;
	}

	ll generate() {
		answer = 0;
		
		dfs(0, -1);
		
		return answer;
	}
};

template<typename T>
inline void remove(vector<T>& vec, const T& val) {
	auto it = find(vec.begin(), vec.end(), val);
	if (it != vec.end()) vec.erase(it);
}

void compress(graph_t& graph) {
	BridgesFinder bf = {graph};
	bf.find();

	for (const auto& [v, w]: bf.bridges) {
		remove(graph[v], w);
		remove(graph[w], v);
	}

	queue<ll> verts;
	vector<ll> index(graph.size(), -1);
	ll count = 0;

	for (ll i = 0; i < graph.size(); ++i) {
		if (index[i] != -1) continue;

		verts.push(i);
		while (!verts.empty()) {
			ll cur = verts.front();
			verts.pop();

			if (index[cur] != -1) continue;
			index[cur] = count;
			
			for (const ll& to: graph[cur])
				if (index[to] == -1) 
					verts.push(to);
		}

		++count;
	}

	graph.clear();
	graph.resize(count);

	for (const auto& [v, w]: bf.bridges) {
		const ll w_i = index[w];
		const ll v_i = index[v];
		graph[v_i].push_back(w_i);
		graph[w_i].push_back(v_i);
	}
}

vector<graph_t> components(graph_t&& graph) {
	vector<char> was(graph.size(), false);
	vector<ll> index(graph.size());	
	queue<ll> verts;

	vector<graph_t> components;

	for (ll i = 0; i < graph.size(); ++i) {
		if (was[i]) continue;
		
		ll id = 0;
		components.emplace_back();

		verts.push(i);
		while (!verts.empty()) {
			ll cur = verts.front();
			verts.pop();

			if (was[cur]) continue;
			was[cur] = true;

			index[cur] = id++;
			for (const ll& to: graph[cur])
				if (!was[to]) verts.push(to);

			components.back().emplace_back(move(graph[cur]));
		}

		for (auto& list: components.back()) 
			for (auto& v: list)
				v = index[v];
	}

	return components;
}

void print(const graph_t& graph) {
	cout << "Graph on " << graph.size() << " verts:" << endl;
	for (ll i = 0; i < graph.size(); ++i) {
		cout << i << ": ";
		for (const ll& v: graph[i])
			cout << v << " ";
		cout << endl;
	}
}

int main() {
	ll n, m;
	cin >> n >> m;

	graph_t graph(n);
	for (ll i = 0; i < m; ++i) {
		ll u, v;
		cin >> u >> v;
		--u; --v;
		graph[u].push_back(v);
		graph[v].push_back(u);
	}

	ll full = 0;
	ll remove = 0;
	auto comps = components(move(graph));
	for (auto& comp: comps) {
		compress(comp);
		full += comp.size() - 1;
		CycleGenerator cg = {comp};
		remove = max(remove, cg.generate());
	}

	cout << full - remove;
}
