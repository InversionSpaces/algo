#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>

using namespace std;

int main() {
	int n;
	cin >> n;

	vector<int> coords(n);
	for (auto& i: coords)
		cin >> i;

	sort(coords.begin(), coords.end());
	
	int connect = coords[1] - coords[0];
	int disconnect = numeric_limits<int>::max();
	for (int i = 2; i < n; ++i) {
		int tmpdis = connect;
		connect = (disconnect > connect ? connect : disconnect) + coords[i] - coords[i - 1];
		disconnect = tmpdis;
	}

	cout << connect;
}
