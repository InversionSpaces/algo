#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned long long ull;

vector<ull> arr;
ull delta, n, p, q;

bool check(const ull hits) {
	auto it = upper_bound(arr.begin(), arr.end(), q * hits);
	
	if (delta == 0) return it == arr.end();
	
	for (ull remained = hits; it != arr.end(); ++it) {
		const ull last = (*it) - hits * q;
		const ull needed = last / delta + ull((last % delta) > 0);
		if (needed > remained) return false;
		remained -= needed;
	}
	
	return true;
}

ull search(ull l, ull r) {
	ull p = 0;
	
	while (l + 1 < r) {
		p = (l + r) / 2;
		if (check(p)) r = p;
		else l = p;
	}
	
	return r;
}

int main() {	
	cin >> n >> p >> q;
	
	arr.resize(n);
	for (auto& i: arr)
		cin >> i;
		
	sort(arr.begin(), arr.end());
	
	delta = p - q;
	
	//cout << check(2) << endl;
	
	cout << search(0, arr.back() / q + 1);
}
