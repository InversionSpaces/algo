#include <iostream>
#include <vector>

using namespace std;

size_t log2i(size_t k) {
	size_t res = 0;
	
	while (k > 1) {
		k >>= 1;
		++res;
	}

	return res;
}

class SparseTable {
private:
	vector<vector<int>> table;
	vector<int> arr;
	vector<int> len2k;
public:
	SparseTable(vector<int>&& tmp) : arr(tmp) {
		const size_t& maxk = log2i(arr.size()) + 1;
		
		table.resize(arr.size());
		for (size_t i = 0; i < table.size(); ++i) {
			auto& v = table[i];
			v.resize(maxk + 1);
			v[0] = i;
		}

		for (size_t k = 1, prevpow = 1; k < maxk; ++k, prevpow *= 2)
			for (size_t i = 0; i < table.size(); ++i) {
				const size_t& j = i + prevpow;
				const size_t& maxl = table[i][k - 1];
				const size_t& maxr = j < table.size() ? table[j][k - 1] : maxl;
				table[i][k] = arr[maxl] > arr[maxr] ? maxl : maxr;
			}

		len2k.resize(arr.size());
		for (size_t l = 0, powk = 1, k = 0; l < len2k.size(); ++l) {
			if ((powk << 1) < l + 1)
				powk <<= 1, ++k;
			len2k[l] = k;
		}
	}

	size_t segment(size_t l, size_t r) {
		if (l == r) return l;

		//cout << l << " to " << r << ":" << endl;
		const size_t& len = r - l;
		const size_t& k = len2k.at(len);
		//cout << "k: " << k << endl;
		const size_t& powk = 1 << k;
		//cout << "powk: " << powk << endl;

		const size_t& maxl = table.at(l).at(k);
		const size_t& maxr = table.at(r - powk + 1).at(k);

		//cout << "maxl: " << maxl << "; maxr: " << maxr << endl;

		return arr.at(maxl) > arr.at(maxr) ? maxl : maxr;
	}
};

int main() {
	freopen("index-max.in", "r", stdin);
	freopen("index-max.out", "w", stdout);

	ios_base::sync_with_stdio(false);
	cin.tie(0);
	
	size_t n;
	cin >> n;
	
	vector<int> arr(n);
	for (auto& i: arr)
		cin >> i;

	SparseTable table(move(arr));

	size_t k;
	cin >> k;
	while (k--) {
		size_t l, r;
		cin >> l >> r;
		--l; --r;
		cout << table.segment(l, r) + 1 << '\n';
	}
}
