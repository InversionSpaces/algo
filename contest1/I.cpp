#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>

using namespace std;

typedef long long ll;

int main() {
	ll n, m;
	cin >> n >> m;
	
	vector<pair<ll, bool>> edges(2 * n);
	for (ll i = 0; i < n; ++i) {
		ll a, b;
		cin >> a >> b;
		
		if (a > b) swap(a, b);
		
		edges[2 * i] = {a, false};
		edges[2 * i + 1] = {b, true};
	}
	
	sort(edges.begin(), edges.end());
	
	vector<pair<ll, ll>> points(m);
	for (ll i = 0; i < m; ++i) {
		cin >> points[i].first;
		points[i].second = i;
	}
	
	sort(points.begin(), points.end());
	
	vector<ll> answers(m, 0);
	ll count = 0;
	ll pos = 0;
	
	for (const auto& e: edges) {
		if (!e.second) {
			while (pos < m && points[pos].first < e.first)
				answers[points[pos++].second] = count;
			++count;
		}
		else {
			while (pos < m && points[pos].first <= e.first)
				answers[points[pos++].second] = count;
			--count;
		}
	}
	
	for (const auto& a: answers) 
		cout << a << " ";
}
