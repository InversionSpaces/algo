#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <fstream>

using namespace std;

typedef int ll;

class BITree {

#define FFUNC(i) (i & (i + 1))
#define GFUNC(i) (i | (i + 1))

private:
	vector<ll> inarr;
	vector<ll> forward;
	vector<ll> backward;
public:
	BITree(const vector<ll>&& arr) : inarr(arr)
	{
		forward.resize(inarr.size());
		backward.resize(inarr.size());

		for (ll i = 0; i < backward.size(); ++i) {
			ll tmax = i;
			for (ll j = FFUNC(i); j < i; ++j)
				tmax = inarr[tmax] > inarr[j] ? tmax : j;
			backward[i] = tmax;
		}

		for (ll i = 0; i < forward.size(); ++i) {
			ll tmax = i;
			for (ll j = i + 1; j < inarr.size() && j <= GFUNC(i); ++j)
				tmax = inarr[tmax] > inarr[j] ? tmax : j;
			forward[i] = tmax;
		}
	}

	ll segment(ll l, ll r) {
		ll tmax = l;
		ll vmax = inarr[l];
		
		while (r >= 0 && FFUNC(r) >= l) {
			const ll& indx = backward[r];
			const ll& val = inarr[indx];
			
			if (vmax < val) {
				tmax = indx;
				vmax = val;
			}
			
			r = FFUNC(r) - 1;
		}

		while (GFUNC(l) <= r) {
			const ll& indx = forward[l];
			const ll& val = inarr[indx];
			
			if (vmax < val) {
				tmax = indx;
				vmax = val;
			}
			
			l = GFUNC(l) + 1;
		}

		return tmax;
	}

#undef FFUNC
#undef GFUNC

};

#include <random>
#include <cassert>

int main() {
	/*
	while (true) {
		cout << "-----------------" << endl;

		ll n = rand() % 200 + 10;
		cout << n << endl;
		vector<ll> arr(n);
		for (ll i = 0 ; i < n; ++i) {
			arr[i] = rand() % 500;
			cout << "[" << i << "]" << arr[i] << " ";
		}
		cout << endl;
		
		auto tmp = arr;
		BITree tree(move(tmp));

		ll l = rand() % n;
		ll r = l + rand() % (n - l);

		ll idx = l;
		for (ll i = l + 1; i <= r; ++i)
			if (arr[i] > arr[idx])
				idx = i;

		ll ans = tree.segment(l, r);
		if (arr[idx] > arr[ans]) {
			cout << l << " - " << r << endl;
			cout << ans << " | " << idx << endl;
			return 1;
		}
	}
	*/

	freopen("index-max.in", "r", stdin);
	freopen("index-max.out", "w", stdout);

	ios_base::sync_with_stdio(false);
	cin.tie(0);

	ll n;
	cin >> n;
	
	vector<ll> arr(n);
	for (auto& i: arr) {
		cin >> i;
	}

	BITree tree(move(arr));
	
	ll k;
	cin >> k;

	while (k--) {
		ll l, r;
		cin >> l >> r;
		cout << tree.segment(l - 1, r - 1) + 1 << '\n';
	}
}
