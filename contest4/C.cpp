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

	BITree(const vector<ll>&& tmp) : arr(tmp), inarr(arr.size(), 0)
	{
		for (ll i = 0; i < inarr.size(); ++i)
			for (ll j = i & (i + 1); j <= i; ++j)
				inarr[i] += arr[j];
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
	
	BITree back(move(tmp));
	BITree front(n);


	ll ans = 0;
	for (ll i = 0; i < n; ++i) {
		ans += front.prefix(n - arr[i] - 2) * back.prefix(arr[i] - 1);
		back.update(arr[i], -1);
		front.update(n - arr[i] - 1, 1);
	}

	cout << ans << endl;
}
