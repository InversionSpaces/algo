#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <random>
#include <ctime>
#include <utility>
#include <cassert>

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
		pos = (pos + 1) % heap.size();	
			
		for (const auto& g: greater) {			
			heap[pos] = g;					
			pos = (pos + 1) % heap.size();	
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
			if (val < heap[*it]) {
				swap(val, heap[*it]);
				*it = (*it + 1) % heap.size();
				swap(val, heap[*it]);
			}
		}
		
		if (pivots.empty() || pivots.front() != start + size_)
				heap[(start + size_) % heap.size()] = val;
		
		++size_;
		
		const auto& end = unique(pivots.begin(), pivots.end());
		pivots.erase(end, pivots.end());
		
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

struct blist {
	blist* next;
	blist* prev;
	
	size_t start, id;
	int is_free;
};

struct bheap {
	blist* pos;
	size_t id, size;
};

bool operator<(const bheap& a, const bheap& b) {
	return a.size > b.size;
}

inline void xmalloc(	vector<char>& is_r,
						vector<blist*>& reqs,
						quickheap<bheap>& heap,
						size_t rsize, size_t fsize) 
{
	while (!heap.empty() && !is_r.at(heap.find_min().id))
		heap.extract_min();
	
	if (heap.empty() || heap.find_min().size < rsize) {
		cout << "-1" << endl;
		
		reqs.push_back(nullptr);
		
		return;
	}
	
	blist* block = heap.extract_min().pos;
	
	assert(block);
	assert(block->is_free);
	
	block->is_free = false;
	is_r.at(block->id) = false;
	
	reqs.push_back(block);
	
	cout << block->start + 1 << "\n";
	
	blist* const next = block->next;
	const ll size = (next ? next->start : fsize) - block->start;
	
	if (size == rsize)
		return;
		
	blist* nblock = new blist {	
							next, 
							block, 
							block->start + rsize,
							is_r.size(),
							true 
						};
	
	block->next = nblock;
	if (nblock->next)
		nblock->next->prev = nblock;
	
	heap.insert({nblock, is_r.size(), size - rsize});
	is_r.push_back(true);	
}

inline void xfree(		vector<char>& is_r,
						vector<blist*>& reqs,
						quickheap<bheap>& heap,
						ll id, ll fsize) 
{	
	reqs.push_back(nullptr);
	
	blist* cur = reqs.at(id);
	
	if (!cur)
		return;
	
	assert(!is_r.at(cur->id)); 
	
	blist* prev = cur->prev;
	if (prev && prev->is_free) {
		is_r.at(prev->id) = false;
		cur->prev = prev->prev;
		cur->start = prev->start;
		
		if (cur->prev)
			cur->prev->next = cur;
		
		delete prev;
	}
	
	blist* next = cur->next;
	if (next && next->is_free) {
		is_r.at(next->id) = false;
		cur->next = next->next;
		
		if (cur->next)
			cur->next->prev = cur;
		
		delete next;
	}
	
	cur->is_free = true;
	
	cur->id = is_r.size();
	is_r.push_back(true);
	
	heap.insert({
		cur, 
		cur->id, 
		(cur->next ? cur->next->start : fsize) - cur->start
	});					
}

int main() {
	size_t n, m;
	cin >> n >> m;
	
	vector<char> is_relevant;
	vector<blist*> requests;
	quickheap<bheap> heap; 
	
	blist* first = new blist{nullptr, nullptr, 0, 0, true};
	blist* head = new blist{first, nullptr, 0, 0, false}; 
	first->prev = head;
	
	heap.insert({first, 0, n});
	is_relevant.push_back(true);
	
	for (ll i; m--;) {
		cin >> i;
		
		if (i > 0)
			xmalloc(is_relevant, requests, heap, i, n);
		else
			xfree(is_relevant, requests, heap, -1 - i, n);
	}
	
	while (head) {
		blist* tmp = head->next;
		delete head;
		head = tmp;
	}
}
