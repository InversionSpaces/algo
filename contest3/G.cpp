#include <iostream>
#include <set>
#include <functional>
#include <cassert>

using namespace std;

typedef unsigned long long ull;

struct ver {
	ull x;
	ull st;
	ull ed;

	bool operator==(const ver& a) const {
		return x == a.x && st == a.st && ed == a.ed;
	}
};

bool by_x(const ver& a, const ver& b) {
	return a.x < b.x;
}

bool by_start(const ver& a, const ver& b) {
	return a.st < b.st;
}

bool by_end(const ver& a, const ver& b) {
	return a.ed < b.ed;
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	ull n, y, x;
	cin >> n >> y >> x;

	multiset<ver, bool(*)(const ver&, const ver&)>  verbyst(by_start);
	while (n--) {
		ver tmp;
		cin >> tmp.x >> tmp.st >> tmp.ed;
		verbyst.insert(tmp);
	}
	
	ver start = {0, 0, y};
	ver end = {x, 0, y};
	
	multiset<ver, bool(*)(const ver&, const ver&)> curverbyed(by_end);
	multiset<ull> dists = {x};
	
	multiset<ver, bool(*)(const ver&, const ver&)> curverbyx(by_x);
	curverbyx.insert(start); curverbyx.insert(end);

	auto addnext = verbyst.begin();
	for (ull i = 0; i < y + 1; ++i) {
		while (addnext != verbyst.end() && (*addnext).st == i) {
			curverbyed.insert(*addnext);
			
			auto nextit = curverbyx.lower_bound(*addnext);
			auto previt = prev(nextit);

			ull tmp = (*nextit).x - (*previt).x;
			dists.erase(dists.find(tmp));
			
			tmp = (*nextit).x - (*addnext).x;
			dists.insert(tmp);
			
			tmp = (*addnext).x - (*previt).x;
			dists.insert(tmp);
			
			curverbyx.insert(*addnext);

			++addnext;
		}

		auto del = curverbyed.lower_bound({0, 0, i - 1});
		while (del != curverbyed.end() && (*del).ed == (i - 1)) {
			auto cit = curverbyx.lower_bound(*del);
			while (!((*del) == (*cit))) ++cit;
			curverbyx.erase(cit);

			auto nextit = curverbyx.lower_bound(*del);
			auto previt = prev(nextit);

			ull tmp = (*nextit).x - (*del).x;
			dists.erase(dists.find(tmp));
			
			tmp = (*del).x - (*previt).x;
			dists.erase(dists.find(tmp));
			
			tmp = (*nextit).x - (*previt).x;
			dists.insert(tmp);

			del++;
		}
		
		auto ansit = dists.end();
		ansit = prev(ansit);
		cout << (*ansit) << '\n';
	}
}
