#include <iostream>
#include <vector>

using namespace std;

vector<vector<char>> table;
int n, m;

int black;
int white;

inline void dfs(int x, int y, char color) {
	if (x < 0 || x >= n || y < 0 || y >= m)
		return;

	if (table[x][y])
		return;

	table[x][y] = color;

	if (color == 'w') ++white;
	else ++black;
	
	const char ncolor = color == 'w' ? 'b' : 'w';
	for (int dx = -2; dx <= 2; ++dx) {
		if (!dx) continue;
		int dy = 3 - abs(dx);
		dfs(x + dx, y + dy, ncolor);
		dfs(x + dx, y - dy, ncolor);
	}
}

int main() {
	cin >> n >> m;

	table.resize(n);
	for (auto& v: table) v.assign(m, 0);
	
	int ans = 0;
	for (int x = 0; x < n; ++x)
	for (int y = 0; y < m; ++y)
	if (!table[x][y]) {
		black = white = 0;
		dfs(x, y, 'w');
		ans += max(white, black);
	}

	cout << ans;
}
