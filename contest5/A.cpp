#include <iostream>
#include <vector>

#pragma GCC optimize ("O3")
#pragma GCC target("avx2")

using namespace std;

int main() {
	using vi = vector<int>;
	using vvi = vector<vi>;
	
	int N, M;
	cin >> N >> M;

	vi m(N);
	vi c(N);
	for (auto& i: m)
		cin >> i;
	for (auto& i: c)
		cin >> i;

	vector<vvi> table(N, vvi(M + 1, vi(2, 0)));
	table[0][m[0]][0] = c[0];
	table[0][m[0]][1] = 1;

	for (int i = 1; i < N; ++i) {
		for (int curm = 1; curm <= M; ++curm) {
			int pick = m[i] > curm ? 0 : table[i - 1][curm - m[i]][0] + c[i];
			int lose = table[i - 1][curm][0];
			if (pick > lose) {
				table[i][curm][0] = pick;
				table[i][curm][1] = 1;
			}
			else {
				table[i][curm][0] = lose;
				table[i][curm][1] = 0;
			}
		}
	}

	int maxc = 0;
	int maxm = 0;
	for (int i = 0; i <= M; ++i) {
		int curc = table[N - 1][i][0];
		if (maxc < curc) {
			maxc = curc;
			maxm = i;
		}
	}

	vi ans(N, 0);
	int count = 0;
	for (int i = N; i; --i) {
		if (table[i - 1][maxm][1]) {
			ans[count++] = i;
			maxm -= m[i - 1];
		}
	}

	cout << count << endl;
	for (int i = count; i; --i)
		cout << ans[i - 1] << ' ';
}
