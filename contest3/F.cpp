#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

typedef unsigned long long ull;

int main() {
	ull n, m, k;
	cin >> n >> m >> k;

	map<ull, set<ull>> in_sets;
	set<ull> sets[m + 1];

	string cmd;
	while (k--) {
		cin >> cmd;

		ull one;
		cin >> one;

		if (cmd == "ADD") {
			ull two;
			cin >> two;

			sets[two].insert(one);
			in_sets[one].insert(two);
		}
		else if (cmd == "DELETE") {
			ull two;
			cin >> two;

			sets[two].erase(one);
			in_sets[one].erase(two);
		}
		else if (cmd == "CLEAR") {
			for (const auto& i: sets[one]) 
				in_sets[i].erase(one);

			sets[one].clear();
		}
		else if (cmd == "LISTSET") {
			if (sets[one].empty()) {
				cout << "-1\n";
				continue;
			}

			for (const auto& i: sets[one])
				cout << i << " ";
			cout << "\n";
		}
		else if (cmd == "LISTSETSOF") {
			const auto& s = in_sets[one];

			if (s.empty()) {
				cout << "-1\n";
				continue;
			}

			for (const auto& i: s)
				cout << i << " ";
			cout << "\n";
		}
	}
}
