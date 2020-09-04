#include <iostream>
#include <vector>
#include <queue>
#include <utility>

using namespace std;

typedef long long ll;

struct dir {
	ll v, e_id;
};
vector<vector<dir>> graph;

struct edge {
	ll v, u;
};
vector<edge> edges;

vector<ll> subsize;
vector<ll> update;

int count_subsizes(ll prev, ll cur) {
	ll result = 1;

	for (const dir& d: graph[cur]) {
		if (d.v == prev) continue;
		update.push_back(d.e_id);
		result += count_subsizes(cur, d.v);
	}

	return subsize[cur] = result;
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	ll n, m;
	cin >> n >> m;

	graph.resize(n);
	for (ll i = 0; i < m; ++i) {
		ll v, u;
		cin >> v >> u;
		--v; --u;

		ll e_id = static_cast<ll>(edges.size());
		graph[v].push_back({u, e_id});
		graph[u].push_back({v, e_id});
		edges.push_back({v, u});
	}

	subsize.assign(n, 0);
	vector<ll> ans(m);
	for (ll i = 0; i < n; ++i) {
		if (subsize[i]) continue;

		ll full = count_subsizes(i, i);
		for (const ll& id: update) {
			const ll v = edges[id].v;
			const ll u = edges[id].u;

			const ll half = min(subsize[v], subsize[u]);
			ans[id] = (full - half) * half;
		}

		update.clear();
	}

	ll q;
	cin >> q;
	while (q--) {
		ll i;
		cin >> i;
		--i;	

		cout << ans[i] << '\n';
	}
}
