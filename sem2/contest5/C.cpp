#include <iostream>
#include <vector>
#include <string>

using namespace std;

string compressed[1000][1000];
int replen[1000][1000];

string str;

#pragma comment(linker, "/stack:200000000")
#pragma GCC optimize("Ofast")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")

inline int int_len(int a) {
	int retval = 0;
	while (a) {
		++retval;
		a /= 10;
	}

	return retval;
}

inline int repeat_count(int l, int r) {
	const int len = r - l + 1;
	const int sublen = replen[l][r];

	if (len > 4 && len != sublen) {
		int res = int_len(len / sublen) + 2;
		res += compressed[l][l + sublen - 1].size();

		if (res < len)
			return res;
	}

	return len;
}

inline string repeat_compress(int l, int r) {
	const int len = r - l + 1;
	const int sublen = replen[l][r];

	if (len > 4 && len != sublen) {
		string str = to_string(len / sublen);
		const string& subres = compressed[l][l + sublen - 1];
		
		if (subres.size() + str.size() + 2 < len) {
			str += '(';
			str += subres;
			str += ')';

			return str;
		}
	}

	return str.substr(l, len);
}

inline void init_replen() {
	const size_t size = str.size();

	vector<int> prefix(size);
	for (int l = 0; l < size; ++l) {
		prefix[0] = 0;
		const size_t subsize = size - l;
		int* replen_l = replen[l];
		
		for (int i = 1; i < subsize; ++i) {
			int len = prefix[i - 1];
			while (len && str[l + i] != str[l + len])
				len = prefix[len - 1];
			
			if (str[l + i] == str[l + len]) ++len;
			prefix[i] = len;

			const int n = i + 1;
			const int k = n - prefix[i];
			if (n % k == 0)
				replen_l[l + i] = k;
			else
				replen_l[l + i] = n;
		}
		
		replen_l[l] = 1;
	}
}

inline void count_compressed() {
	const size_t size = str.size();

	for (int i = 0; i < size; ++i)
		compressed[i][i] = str[i];

	for (int len = 2; len <= size; ++len)
	for (int l = 0; l <= size - len; ++l) {
		const int r = l + len - 1;

		int minsize = repeat_count(l, r);
		int index = -1;
		for (int i = l; i < r; ++i) {
			const string& left = compressed[l][i];
	        	const string& right = compressed[i + 1][r];
			const int sumsize = left.size() + right.size();

			if (sumsize < minsize) {
				index = i;
				minsize = sumsize;
			}
		}

		if (index != -1)
			compressed[l][r] = compressed[l][index] + compressed[index + 1][r];
		else
			compressed[l][r] = repeat_compress(l, r);
	}
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	cin >> str;
	
	init_replen();
	count_compressed();

	cout << compressed[0][str.size() - 1];
}
