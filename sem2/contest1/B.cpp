#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>

using namespace std;

bool compare(const string& left, const string& right) {
	size_t n = min(left.size(), right.size());
	
	for (size_t i = 0; i < n; ++i) {
		if (left[i] == right[i]) continue;
		return left[i] > right[i];
	}
	
	if (left.size() == right.size()) return false;
	
	return 	left.size() < right.size() ? 
			compare(left, right.substr(n)) :
			!compare(right, left.substr(n));
}

int main() {
	ifstream in("number.in");
	ofstream out("number.out");
	//istream& in = cin;
	//ostream& out = cout;
	
	string tmp;
	
	vector<string> parts;
	while (in >> tmp) 
		parts.push_back(tmp);
		
	sort(parts.begin(), parts.end(), compare);
		
	for (const auto& s: parts) 
		out << s;
}
