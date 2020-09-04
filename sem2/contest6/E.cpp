#include <iostream>
#include <vector>
#include <utility>
#include <limits>
#include <queue>

#pragma GCC target ("avx2")
#pragma GCC optimization ("O3")
#pragma GCC optimization ("unroll-loops")

using namespace std;

typedef long long ll;

vector<vector<pair<ll, ll>>> graph;
vector<ll> costs;

int main() {
	ll n, m, k, s, f;
	cin >> n >> m >> k >> s >> f;
	--s; --f;

	graph.resize(n);
	while (m--) {
		ll si, fi, pi;
		cin >> si >> fi >> pi;
		--si; --fi;
		graph[si].push_back({fi, pi});
	}

	costs.assign(n, numeric_limits<ll>::max());
	costs[s] = 0;
	
	struct point {const ll v, cost, remain;};
	queue<point> points;
	points.push({s, 0, k});

	while (!points.empty()) {
		const point cur = points.front();
		points.pop();
		
		for (const auto& [next, cost]: graph[cur.v]) {
			const ll ncost = cur.cost + cost;
			if (ncost >= costs[next]) continue;
			costs[next] = ncost;
			if (cur.remain > 1) points.push({next, ncost, cur.remain - 1});
		}
	}

	cout << (costs[f] == numeric_limits<ll>::max() ? -1 : costs[f]);
}
