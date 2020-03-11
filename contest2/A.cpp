#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <random>
#include <ctime>
#include <utility>

using namespace std;

template<typename riter>
typename iterator_traits<riter>::value_type 
	k_stat(riter begin, riter end, size_t k) {
	using val_t = typename iterator_traits<riter>::value_type;
	
	size_t n = distance(begin, end);
	
	if (k >= n)
		throw invalid_argument("k_stat: k is out of range");
	
	if (n < 5) {
		vector<val_t> tmpvec(begin, end);
		sort(tmpvec.begin(), tmpvec.end());
		
		return tmpvec[k];
	}
	
	vector<val_t> medians(n / 5);
	array<val_t, 5> tmparr;
	riter current = begin;
	
	for (val_t& m: medians) {
		copy(current, current + 5, tmparr.begin());
		sort(tmparr.begin(), tmparr.end());
		m = tmparr[3];
		current += 5;
	}
	
	val_t pivot = k_stat(	medians.begin(), 
							medians.end(), 
							medians.size() / 2
						);
	
	vector<val_t> less;
	vector<val_t> equal;
	vector<val_t> greater;
	
	for (riter it = begin; it != end; ++it) {
		if (*it < pivot) 
			less.push_back(*it);
		else if (*it > pivot) 
			greater.push_back(*it);
		else 
			equal.push_back(*it);
	}
	
	if (k < less.size())
		return k_stat(	less.begin(), 
						less.end(), 
						k
					);
	
	if (k >= less.size() + equal.size())
		return k_stat(	greater.begin(), 
						greater.end(), 
						k - less.size() - equal.size()
					);
		
	return pivot;
}

template<typename T>
class QuickHeap 
{
private:
	vector<T>       heap;
	vector<size_t>  pivots;
	size_t          start, size_;
	
	inline size_t get_end() const noexcept {
		if (pivots.empty()) 
			return (start + size_) % heap.size();
		return pivots.back();
	}
	
	void partition() {
		const size_t end = get_end();
		const size_t size = (heap.size() + end - start) % heap.size();
		const size_t pivot = (start + rand() % size) % heap.size();
		
		vector<T> less, greater;
		
		for (size_t i = start; i != end; i = (i + 1) % heap.size()) {
			if (i == pivot)
				continue;
			
			if (heap[i] < heap[pivot])
				less.push_back(heap[i]);
			else
				greater.push_back(heap[i]);
		}
		
		const T pval = heap[pivot];
		size_t pos = start;

		for (const auto& l: less) {			
			heap[pos] = l;					
			pos = (pos + 1) % heap.size();	
		}
		
		pivots.push_back(pos);
		heap[pos] = pval;	
			
		for (const auto& g: greater) {
			pos = (pos + 1) % heap.size();			
			heap[pos] = g;							
		}
	}
	
	void iqs() {
		if (size_ == 0)
			throw invalid_argument("iqs: heap is empty");
		
		while (pivots.empty() || pivots.back() != start)
			partition();
	}
	
	void resize() {
		const size_t psize = heap.size();
		heap.resize(psize * 2);
		
		for (size_t i = psize; i < start + size_; ++i)
			heap[i] = heap[i % psize];
		
		for (auto& p: pivots)
			if (p < start) p += psize;
	}
	
public:
	QuickHeap(size_t capacity=256) :
		start(0),
		size_(0),
		heap(capacity),
		pivots()
	{ }
	
	void insert(T val) {
		if (size_ * 2 >= heap.size()) 
			resize();
		
		size_t pos = (start + size_++) % heap.size();
		
		for (auto it = pivots.begin(); it != pivots.end(); ++it) {
			if (heap[*it] < val)
				break;
			
			size_t next = (*it + 1) % heap.size();
			
			if (pos == next) {
				heap[pos] = heap[*it];
				swap(pos, *it);
				
				continue;
			}
			
			heap[pos] = heap[next];
			heap[next] = heap[*it];
			pos = *it;
			*it = next;
		}
		
		heap[pos] = val;
	}
	
	T find_min() {
		if (size_ == 0)
			throw invalid_argument("find_min: heap is empty");
		
		iqs();
		
		return heap[start];
	}
	
	T extract_min() {
		if (size_ == 0)
			throw invalid_argument("extract_min: heap is empty");
		
		iqs();
		
		size_t pos = pivots.back();
		pivots.pop_back();
		
		start = (start + 1) % heap.size();
		--size_;
		
		return heap[pos];
	}
	
	bool empty() const noexcept {
		return size_ == 0;
	}
	
	size_t size() const noexcept {
		return size_;
	}
};

typedef long long ll;

enum CMD_TYPE {
	CMD_INSERT,
	CMD_REMOVE,
	CMD_FIND
};

int main() {
	srand(time(NULL));
	
	QuickHeap<ll> qh(1UL << 24);
	
	cin.tie(NULL);
	ios_base::sync_with_stdio(false);
	
	string cmd; ll x;
	vector<pair<CMD_TYPE, ll>> ans;
	
	ll m;
	cin >> m;
	while (m--) {
		cin >> cmd;
		if (cmd == "insert") {
			cin >> x;
			
			qh.insert(x);
			ans.push_back({CMD_INSERT, x});
		}
		else if (cmd == "removeMin") {
			if (qh.empty())
				ans.push_back({CMD_INSERT, 0});
			else
				qh.extract_min();
				
			ans.push_back({CMD_REMOVE, 0});
		}
		else if (cmd == "getMin") {
			cin >> x;
			
			if (qh.empty() || qh.find_min() > x) {
				ans.push_back({CMD_INSERT, x});
				qh.insert(x);
			}
			else {
				while (!qh.empty() && qh.find_min() < x) {
					ans.push_back({CMD_REMOVE, 0});
					qh.extract_min();
				}
				
				if (qh.empty() || qh.find_min() > x) {
					ans.push_back({CMD_INSERT, x});
					qh.insert(x);
				}
			}
			
			ans.push_back({CMD_FIND, x});
		}
	}
	
	cout << ans.size() << "\n";
	for (const auto& p: ans) {
		switch (p.first) {
			case CMD_FIND:
				cout << "getMin " << p.second << "\n";
			break;
			case CMD_INSERT:
				cout << "insert " << p.second << "\n";
			break;
			case CMD_REMOVE:
				cout << "removeMin\n";
		}
	}
}
