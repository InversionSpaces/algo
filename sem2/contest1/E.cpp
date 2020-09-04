#include <iostream>

using namespace std;

typedef unsigned int ui;
typedef unsigned long long ull;

const ui _size = 16777216 + 1;

ui a, b, cur = 0;

ui arr[_size];

ui next() {
	cur = cur * a + b;
	return cur >> 8;
}

int main() {
	ui m, q;
	cin >> m >> q >> a >> b;
	
	for (ui i = 0; i < _size; ++i)
		arr[i] = 0;
	
	for (ui i = 0; i < m; ++i) {
		ui add = next();
		ui l = next();
		ui r = next();
		if (l > r) swap(l, r);
		
		arr[l] 		+= add;
		arr[r + 1] 	-= add;
	}
	
	ui sum = 0;
	ui val = 0;
	
	for (ui i = 0; i < _size; ++i) {
		val += arr[i];
		arr[i] = sum;
		sum += val;
	}
	
	sum = 0;
	
	for (ui i = 0; i < q; ++i) {
		ui l = next();
		ui r = next();
		if (l > r) swap(l, r);
		
		sum += arr[r + 1] - arr[l];
	}
	
	cout << sum;
}
