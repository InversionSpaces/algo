#include <stdio.h>

#pragma GCC optimize ("O3")
#pragma GCC target("avx2")

int main() {
	int N, M;
	scanf("%d %d", &N, &M);

	int m[100], c[100];
	for (int i = 0; i < N; ++i)
		scanf("%d", m + i);
	for (int i = 0; i < N; ++i)
		scanf("%d", c + i);
	
	int table[100][10001][2];
	for (int i = 0; i <= M; ++i) {
		table[0][i][0] = 0;
		table[0][i][1] = 0;
	}

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

	int ans[100];
	int count = 0;
	for (int i = N; i; --i) {
		if (table[i - 1][maxm][1]) {
			ans[count++] = i;
			maxm -= m[i - 1];
		}
	}

	printf("%d\n", count);
	for (int i = count; i; --i)
		printf("%d ", ans[i - 1]);
}
