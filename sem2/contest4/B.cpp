#include <iostream>
#include <vector>

using namespace std;

template<typename T>
class Cube {
private:
	vector<T> inner;
	int n;
public:
	Cube(int n) : n(n), inner(n*n*n, T(0)) {}
	const T& operator()(int x, int y, int z) const noexcept {
		return inner[n*n*z + n*y + x];
	}

	T& operator()(int x, int y, int z) noexcept {
		return inner[n*n*z + n*y + x];
	}
};

class BITree {
private:
	Cube<int> arr;
	Cube<int> inner;
	int n;
public:
	BITree(int n) : arr(n), inner(n), n(n) {}

	void update(int x, int y, int z, int dif) {
		arr(x, y, z) += dif;

		for (int i = x; i < n; i = i | (i + 1))
		for (int j = y; j < n; j = j | (j + 1))
		for (int k = z; k < n; k = k | (k + 1))
			inner(i, j, k) += dif;
	}

	int prefix(int x, int y, int z) const {
		int ans = 0;
		
		for (int i = x; i >= 0; i = (i & (i + 1)) - 1)
		for (int j = y; j >= 0; j = (j & (j + 1)) - 1)
		for (int k = z; k >= 0; k = (k & (k + 1)) - 1)
			ans += inner(i, j, k);

		return ans;
	}

	int segment(int x1, int y1, int z1, int x2, int y2, int z2) const {
		int major = prefix(x2, y2, z2);
		int minor = prefix(x1 - 1, y1 - 1, z1 - 1);
		
		int xplate = prefix(x1 - 1, y2, z2);
		int yplate = prefix(x2, y1 - 1, z2);
		int zplate = prefix(x2, y2, z1 - 1);

		int xpillar = prefix(x2, y1 - 1, z1 - 1);
		int ypillar = prefix(x1 - 1, y2, z1 - 1);
		int zpillar = prefix(x1 - 1, y1 - 1, z2);

		return major - xplate - yplate - zplate + xpillar + ypillar + zpillar - minor;
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	int n;
	cin >> n;
	BITree tree(n);

	while (true) {
		int m;
		cin >> m;

		if (m == 3) return 0;

		if (m == 1) {
			int x, y, z, k;
			cin >> x >> y >> z >> k;
			tree.update(x, y, z, k);
		}
		else if (m == 2) {
			int x1, y1, z1, x2, y2, z2;
			cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
			cout << tree.segment(x1, y1, z1, x2, y2, z2) << '\n';
		}
	}
}
