#include <iostream>
#include <queue>
#include <vector>
#include <string>

#pragma GCC target ("avx2")
#pragma GCC optimization ("O3")
#pragma GCC optimization ("unroll-loops")

using namespace std;

typedef long long ll;

int main() {
	ll n, m, r, c, x, y;
	cin >> n >> m >> r >> c >> x >> y;
	--r; --c;

	vector<string> lab(n);
	for (auto& s: lab) cin >> s;

	struct lr {ll left, right;};
	vector<vector<lr>> lrs(n);
	for (auto& v: lrs)
		v.assign(m, {0, 0});

	ll ans = 1;
	lab[r][c] = 'w';
	lrs[r][c] = {x, y};

	struct point {const ll x, y, left, right;};
	queue<point> points;
	points.push({c, r, x, y});

	while (!points.empty()) {
		const point cur = points.front();
		points.pop();

#define UPDATE \
char& sq = lab[y][x];\
if (sq == '.') {\
	sq = 'w';\
	++ans;\
	points.push({x, y, left, right});\
	lrs[y][x] = {left, right};\
}\
else if (sq == 'w' && (lrs[y][x].left < left || lrs[y][x].right < right)) {\
	lrs[y][x].left = max(lrs[y][x].left, left);\
	lrs[y][x].right = max(lrs[y][x].right, right);\
	points.push({x, y, left, right});\
}

		if (0 < cur.x && 0 < cur.left) {
			const ll x = cur.x - 1;
			const ll y = cur.y;
			const ll left = cur.left - 1;
			const ll right = cur.right;
		
			UPDATE;
		}

		if (cur.x + 1 < m && 0 < cur.right) { 
			const ll x = cur.x + 1;
			const ll y = cur.y;
			const ll left = cur.left;
			const ll right = cur.right - 1;
		
			UPDATE
		}

		if (0 < cur.y) {
			const ll x = cur.x;
			const ll y = cur.y - 1;
			const ll left = cur.left;
			const ll right = cur.right;
		
			UPDATE
		}
		
		if (cur.y + 1 < n) {
			const ll x = cur.x;
			const ll y = cur.y + 1;
			const ll left = cur.left;
			const ll right = cur.right;
			
			UPDATE
		}
	}

	cout << ans;
}
