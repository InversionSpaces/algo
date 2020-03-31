#include <iostream>
#include <list>
#include <vector>
#include <set>
#include <utility>

using namespace std;

typedef unsigned long long ull;
typedef pair<ull, ull> pos;

int main() {
	ull n, k, p;
	cin >> n >> k >> p;

	vector<ull> wants(p);
	for (auto& i: wants) {
		cin >> i;
		--i; // AUTHOR IS BAKA
	}

	list<ull> indexes[n];
	for (ull i = 0; i < p; ++i)
		indexes[wants[i]].push_back(i);

	set<pos, greater<pos>> positions;
	set<ull> floor;
	ull ans = 0;
	for (const auto& cur: wants) {
		auto& cindexes = indexes[cur];
		ull cindex = cindexes.front();
		cindexes.pop_front();

		if (floor.count(cur)) {
			positions.erase({cindex, cur});
		}
		else if (floor.size() < k) {
			floor.insert(cur);
			ans++;
		}
		else {
			ull putup = (*positions.begin()).second;
			positions.erase(positions.begin());
			
			floor.erase(putup);
			floor.insert(cur);

			ans++;
		}

		if (!cindexes.empty())
			positions.insert({cindexes.front(), cur});
		else
			positions.insert({p, cur});
	}

	cout << ans;
}
