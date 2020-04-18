#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

typedef long long ll;

const double eps = 1e-8;

int main() {
	ll n, q;
	cin >> n >> q;

	using vc = vector<char>;
	using vvc = vector<vc>;
	using vvvc = vector<vvc>;

	vvvc table(n, vvc(q + 1, vc(n, 0)));
	vector<ll> d(n);
	for (auto& i: d) {
		cin >> i;
		i -= 10;
	}

	double ans = -1;

	for (ll i = 0; i < n; ++i)
	for (ll k = 0; k < n; ++k)
	for (ll len = 0; len <= q; ++len) {
		ll pick = 0;
		if (len == d[i] && k == 0)
			pick = 1;
		else if (len > d[i] && i && k && table[i - 1][len - d[i]][k - 1])
			pick = 1;

		ll lose = 0;
		if (i && table[i - 1][len][k])
			lose = 1;

		table[i][len][k] = pick | lose;

		if (!table[i][len][k])
			continue;
		
		double midans = 10.0 - (q - len) / double(k + 2);
		if ((midans > 0.0 - eps) && (midans < 5.0 + eps) && ans < midans)
			ans = midans;
	}

	if (ans > 0.0 - eps)
		cout << fixed << setprecision(7) << ans;
	else
		cout << "impossible";
}
