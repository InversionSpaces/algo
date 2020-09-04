#include <iostream>
#include <limits>

using namespace std;

#pragma GCC optimize("O3", "unroll-loops")

typedef long long ll;

const int maxn = 19;
const int maxmask = (1U << maxn);

struct { ll len; int prev; } dp[maxmask][maxn];
ll graph[maxn][maxn];

ll solve(int mask, int v, int n) {
	if (dp[mask][v].len != -1)
		return dp[mask][v].len;

	dp[mask][v].len = numeric_limits<ll>::max();
	int pmask = mask & ~(int(1) << v);
	for (int i = 0; i < n; ++i) {
		if (!(pmask & (int(1) << i))) continue;
		
		ll len = solve(pmask, i, n) + graph[i][v];
		if (len < dp[mask][v].len) {
			dp[mask][v].len = len;
			dp[mask][v].prev = i;
		}
	}

	return dp[mask][v].len;
}

void print_ans(int mask, int v) {
	int prev = dp[mask][v].prev;
	int pmask = mask & ~(int(1) << v);
	if (prev != v) print_ans(pmask, prev);
	cout << v + 1 << ' ';
}

int main() {
	int n;
	cin >> n;
	
	const int nmask = (1U << n);

	for (int i = 0; i < n; ++i)
	for (int j = 0; j < n; ++j)
		cin >> graph[i][j];

	for (int mask = 0; mask < nmask; ++mask)
	for (int i = 0; i < n; ++i)
		dp[mask][i].len = -1;

	for (int i = 0; i < n; ++i) {
		dp[int(1) << i][i].len = 0;
		dp[int(1) << i][i].prev = i;
	}

	int last = 0;
	ll len = numeric_limits<ll>::max();

	for (int i = 0; i < n; ++i) {
		ll ilen = solve(nmask - 1, i, n);
		if (len > ilen) {
			len = ilen;
			last = i;
		}
	}

	cout << len << endl;
	print_ans(nmask - 1, last);
}


