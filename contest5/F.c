#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>

int cmp(const void * a, const void * b) {
	return *(int*)a - *(int*)b;
}

int main() {
	int n;
	scanf("%d", &n);

	int coords[n];
	for (int i = 0; i < n; ++i)
		scanf("%d", coords + i);

	qsort(coords, n, sizeof(int), cmp);
	
	int connect = coords[1] - coords[0];
	int disconnect = INT_MAX;
	for (int i = 2; i < n; ++i) {
		int tmpdis = connect;
		connect = (disconnect > connect ? connect : disconnect) + coords[i] - coords[i - 1];
		disconnect = tmpdis;
	}

	printf("%d\n", connect);
}
