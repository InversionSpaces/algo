#include <iostream>
#include <vector>

using namespace std;

typedef int ll;

class BITree {
private:
	vector<ll> arr;
	vector<ll> inarr;
public:
	BITree(vector<ll>& arr) : arr(arr), inarr(arr.size(), 0)
	{
		arr[0] = arr[0] == 0;
		for (ll i = 1; i < arr.size(); ++i) {
			arr[i] = arr[i - 1] + ll(arr[i] == 0);
		}

		for (ll i = 0; i < inarr.size(); ++i) {
			ll idx = i & (i + 1);
			inarr[i] = arr[i];
			inarr[i] -= idx ? arr[idx - 1] : 0;
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

	void update(ll pos, ll val) {
		ll old = arr[pos];
		arr[pos] = val;
		
		for (ll i = pos; i < inarr.size(); i = i | (i + 1))
			inarr[i] += ll(val == 0) - ll(old == 0);
	}

	ll index(ll l, ll r, ll k) {
		if (segment(l, r) < k)
			return -1;

		while (l < r) {
			ll mid = (l + r) / 2;
			ll count = segment(l, mid);

			if (k > count) {
				l = mid + 1;
				k -= count;
			}
			else r = mid;
		}

		return l;
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	ll n, m;
	cin >> n;

	vector<ll> arr(n);
	for (auto& i: arr)
		cin >> i;

	BITree tree(arr);

	cin >> m;
	while (m--) {
		char c;
		cin >> c;

		if (c == 'u') {
			ll pos, val;
			cin >> pos >> val;
			tree.update(--pos, val);
		}

		if (c == 's') {
			ll l, r, k;
			cin >> l >> r >> k;
			ll ans = tree.index(--l, --r, k);
			cout << (ans == -1 ? ans : ans + 1) << ' ';
		}
	}
}
