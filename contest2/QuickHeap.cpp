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
class quickheap 
{
private:
	vector<T> heap;
	vector<size_t> pivots;
	size_t start, size_;
	
	void partition() {
		const size_t end 	= (pivots.empty() ? start + size_ : pivots.back()) 	% heap.size();
		const size_t range 	= (heap.size() + end - start) 						% heap.size();
		const size_t pivot 	= (start + rand() % range) 							% heap.size();
		
		vector<T> less, equal, greater;
		
		for (size_t i = start; i != end; i = (i + 1) % heap.size()) {
			if (heap[i] < heap[pivot])
				less.push_back(heap[i]);
			else if (heap[i] > heap[pivot])
				greater.push_back(heap[i]);
			else
				equal.push_back(heap[i]);
		}
		
#define UPDATE(ARR)							\
		for (const auto& a: ARR) {			\
			heap[pos] = a;					\
			pos = (pos + 1) % heap.size();	\
		}
		
		size_t pos = start;
		
		UPDATE(less)
		
		pivots.push_back(pos);
		
		UPDATE(equal)
		UPDATE(greater)
		
#undef UPDATE
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
		
		for (size_t i = psize; i < start + psize; ++i)
			heap[i] = heap[i % psize];
		
		for (auto& p: pivots)
			if (p < start)
				p += psize;
	}
	
public:
	quickheap(size_t capacity=256) :
		start(0),
		size_(0),
		heap(capacity),
		pivots()
	{ }
	
	void insert(T val) {
		for (auto it = pivots.rbegin(); it != pivots.rend(); ++it) {
			if (heap[*it] > val) {
				swap(val, heap[*it]);
				*it = (*it + 1) % heap.size();
				swap(val, heap[*it]);
			}
		}
		
		if (pivots.empty() || pivots.front() != start + size_)
				heap[(start + size_) % heap.size()] = val;
		
		++size_;
		
		reverse(pivots.begin(), pivots.end());
		const auto& end = unique(pivots.begin(), pivots.end());
		pivots.erase(end, pivots.end());
		reverse(pivots.begin(), pivots.end());
		
		if (size_ == heap.size()) 
			resize();
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
	
	bool empty() {
		return size_ == 0;
	}
	
	size_t size() {
		return size_;
	}
	
	void print() {
		cout << "pivots: " << pivots.size() << "; size: " << size_ << endl;
		for (	size_t i = start; 
				i != (start + size_) % heap.size();
				i = (i + 1) % heap.size()) {
			if (find(pivots.begin(), pivots.end(), i) != pivots.end())
				cout << "[" << heap[i] << "] ";
			else
				cout << heap[i] << " ";
		}
		cout << endl;
	}
};

typedef long long ll;

int main() {
	srand(time(NULL));
	
	quickheap<ll> qh(1UL << 24);
	
	cin.tie(NULL);
	ios_base::sync_with_stdio(false);
	
	/*
	qh.insert(520596304);
	qh.insert(-265233646);
	
	qh.print();
	
	while (!qh.empty() && qh.find_min() < -222234500) {
		qh.extract_min();
		cout << "extract: " << endl;
		qh.print();
	}
	
	if (qh.empty() || qh.find_min() > -222234500) {
		qh.insert(-222234500);
		cout << "insert: " << endl;
		qh.print();
	}
	
	return 0;
	*/
	
	string cmd; ll x;
	vector<pair<string, ll>> ans;
	
	ll m;
	cin >> m;
	while (m--) {
		//qh.print();
		cin >> cmd;
		if (cmd == "insert") {
			cin >> x;
			
			//cout << cmd << " " << x << ": " << endl;
			
			qh.insert(x);
			ans.push_back({cmd, x});
			
			//qh.print();
		}
		else if (cmd == "removeMin") {
			if (qh.empty())
				ans.push_back({"insert", 0});
			else
				qh.extract_min();
			
			//cout << cmd << endl;
			//qh.print();
				
			ans.push_back({cmd, 0});
		}
		else if (cmd == "getMin") {
			cin >> x;
			
			//cout << cmd << " " << x << ": " << endl;
			
			if (qh.empty() || qh.find_min() > x) {
				ans.push_back({"insert", x});
				qh.insert(x);
				
				//qh.print();
			}
			else {
				while (!qh.empty() && qh.find_min() < x) {
					ans.push_back({"removeMin", 0});
					qh.extract_min();
					
					//qh.print();
				}
				
				if (qh.empty() || qh.find_min() > x) {
					ans.push_back({"insert", x});
					qh.insert(x);
					
					//qh.print();
				}
			}
			
			ans.push_back({cmd, x});
		}
		
		//cout << "-----------------------" << endl;
	}
	
	cout << ans.size() << "\n";
	for (const auto& p: ans) {
		cout << p.first;
		if (p.first != "removeMin")
			cout << " " << p.second;
		cout << "\n";
	}
}
