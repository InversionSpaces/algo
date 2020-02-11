#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main() {
	long long n;
	cin >> n;
	
	vector<long long> arr(n);
	for (auto& i: arr) cin >> i;
	
	sort(arr.begin(), arr.end());
	
	cin >> n;
	while (n--) {
		long long tmp;
		cin >> tmp;
		
		auto up = upper_bound(arr.begin(), arr.end(), tmp);
		auto down = lower_bound(arr.begin(), arr.end(), tmp);
		
		cout << distance(down, up) << " ";
	}
}
