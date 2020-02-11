#include <iostream>
#include <vector>
#include <stack>
#include <utility>
#include <algorithm>

using namespace std;

template<typename iter_t>
/*inline*/ vector<ssize_t> find_closest(iter_t from, iter_t to) {
		using val_t = typename iterator_traits<iter_t>::value_type;
		
		size_t n = distance(from, to);
		vector<ssize_t> retval(n);
		
		stack<pair<val_t, size_t>> closest;
		
		for (auto it = from; it < to; ++it) {
			while (!closest.empty() && 
					closest.top().first >= *it) closest.pop();
			
			size_t i = distance(from, it);
			retval[i] = closest.empty() ? 0 : closest.top().second + 1;
			
			closest.emplace(*it, i);
		}
				
		return retval;						
}

int main() {
	size_t n;
	cin >> n;
	
	vector<long long> arr(n);
	for (auto& i: arr)
		cin >> i;
	
	auto cl_left = find_closest(arr.begin(), arr.end());
	
	auto cl_right = find_closest(arr.rbegin(), arr.rend());
	reverse(cl_right.begin(), cl_right.end());
	for (auto& i: cl_right) i = arr.size() - i - 1;
	
	long long ans = 0;
	for (size_t i = 0; i < arr.size(); ++i)
		ans = max(ans, arr[i] * (cl_right[i] - cl_left[i] + 1));
		
	cout << ans;
}
