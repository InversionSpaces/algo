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

	BITree(vector<ll>& tmp) : arr(tmp), inarr(tmp.size(), 0)
	{
		for (ll i = 1; i < tmp.size(); ++i)
			tmp[i] += tmp[i - 1];

		for (ll i = 0; i < inarr.size(); ++i) {
			ll idx = i & (i + 1);
			inarr[i] = tmp[i];
			inarr[i] -= idx ? tmp[idx - 1] : 0;
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
	
	vector<ll> tmp = arr;
	sort(tmp.begin(), tmp.end());
	
	for (auto& i: arr) {
		auto it = lower_bound(tmp.begin(), tmp.end(), i);
		i = it - tmp.begin();
	}

	fill(tmp.begin(), tmp.end(), 0);
	for (ll i = 0; i < n; ++i)
		++tmp[arr[i]];
	
	BITree back(tmp);
	BITree front(n);

	ll ans = 0;
	for (ll i = 0; i < n; ++i) {
		ans += front.prefix(n - arr[i] - 2) * back.prefix(arr[i] - 1);
		back.update(arr[i], -1);
		front.update(n - arr[i] - 1, 1);
	}

	cout << ans << '\n';
}