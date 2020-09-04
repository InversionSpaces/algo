#include <iostream>
#include <string>

using namespace std;

int main() {
	int n, k;
	cin >> n >> k;;
	
	string str;
	cin >> str;

	int ans = 0;
	for (int c = 0; c < n; ++c) {
		int lasted = k;
		for (int i = 0; (c - i >= 0) && (c + i <= n - 1); ++i) {
			if (str[c - i] != str[c + i]) {
				if (!lasted) break;
				--lasted;
			}
			++ans;
		}

		lasted = k;
		for (int i = 0; (c - 1 - i >= 0) && (c + i <= n - 1); ++i) {
			if (str[c - 1 - i] != str[c + i]) {
				if (!lasted) break;
				--lasted;
			}
			++ans;
		}
	}

	cout << ans;
}
