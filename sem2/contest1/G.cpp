#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

typedef unsigned long long ull;

ull len;

ull search(ull* A, ull* B) {
	ull l = 0;
	ull r = len - 1;
	
	while (l + 1 < r) {
		const ull p = (l + r) / 2;
		if (A[p] > B[p]) r = p;
		else l = p;
	}
	
	return (max(A[l], B[l]) > max(A[r], B[r])) ? r : l;
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	
	ull n, m;
	cin >> n >> m >> len;
	
	ull arr_A[n][len];
	for (ull i = 0; i < n; ++i)
		for (ull j = 0; j < len; ++j)
			cin >> arr_A[i][j];
	
	ull arr_B[m][len];
	for (ull i = 0; i < m; ++i)
		for (ull j = 0; j < len; ++j)
			cin >> arr_B[i][j];
			
	ull q;
	cin >> q;
	
	while (q--) {
		ull i, j;
		cin >> i >> j;
		--i; --j;
		
		cout << (search(arr_A[i], arr_B[j]) + 1) << "\n";
	}
}
