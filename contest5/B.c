#include <stdio.h>

char str[5001];

int main() {
	int n, k;
	scanf("%d %d", &n, &k);
	
	scanf(" %s", str);

	int ans = 0;
	for (int c = 0; c < n; ++c) {
		int tmpk = k;
		for (int i = 0; (c - i >= 0) && (c + i <= n - 1); ++i) {
			if (str[c - i] != str[c + i]) {
				if (!tmpk) break;
				--tmpk;
			}
			++ans;
		}

		tmpk = k;
		for (int i = 0; (c - 1 - i >= 0) && (c + i <= n - 1); ++i) {
			if (str[c - 1 - i] != str[c + i]) {
				if (!tmpk) break;
				--tmpk;
			}
			++ans;
		}
	}

	printf("%d\n", ans);
}
