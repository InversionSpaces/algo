#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef long long ll;

class BITree {
private:
	vector<ll> arr;
	vector<ll> inarr;
public:
	BITree(ll n) : arr(n, 0), inarr(n, 0) {}

	BITree(vector<ll>& fromarr) : arr(fromarr), inarr(fromarr.size(), 0)
	{
		auto& prefsum = fromarr; // to rename
		for (ll i = 1; i < prefsum.size(); ++i)
			prefsum[i] += prefsum[i - 1];

		for (ll i = 0; i < inarr.size(); ++i) {
			ll idx = i & (i + 1);
			inarr[i] = prefsum[i];
			inarr[i] -= idx ? prefsum[idx - 1] : 0;
		}
	}

	ll prefix(ll pos) {
		ll ans = 0;
		for (ll i = pos; i >= 0; i = (i & (i + 1)) - 1) 
			ans += inarr[i];
		
		return ans;
	}

	ll segment(ll l, ll r) {
		return prefix(r) - prefix(l - 1);
	}

	void update(ll pos, ll dif) {
		arr[pos] += dif;
		
		for (ll i = pos; i < inarr.size(); i = i | (i + 1))
			inarr[i] += dif;
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	ll n;
	cin >> n;

	vector<ll> arr(n);
	for (auto& i: arr) {
		cin >> i;
		--i;
	}
	
	vector<ll> sorted = arr;
	sort(sorted.begin(), sorted.end());
	
	for (auto& i: arr) {
		auto it = lower_bound(sorted.begin(), sorted.end(), i);
		i = it - sorted.begin();
	}

	auto& count = sorted; // to rename
	fill(count.begin(), count.end(), 0);
	for (ll i = 0; i < n; ++i)
		++count[arr[i]];
	
	BITree back(count);
	BITree front(n);

	ll ans = 0;
	for (ll i = 0; i < n; ++i) {
		ans += front.prefix(n - arr[i] - 2) * back.prefix(arr[i] - 1);
		back.update(arr[i], -1);
		front.update(n - arr[i] - 1, 1);
	}

	cout << ans << '\n';
}
