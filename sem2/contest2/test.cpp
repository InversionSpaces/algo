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

template<typename T>
class quickheap 
{
private:
	vector<T> 		heap;
	vector<size_t> 	pivots;
	size_t 			start, size_;
	
	inline size_t get_end() {
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
	quickheap(size_t capacity=256) :
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
	
	bool empty() {
		return size_ == 0;
	}
	
	size_t size() {
		return size_;
	}
};

int main() {
	srand(time(NULL));
	quickheap<int> qh;
	
	string cmd;
	int x;
	while (cin >> cmd) {
		if (cmd == "insert") {
			cin >> x;
			qh.insert(x);
		}
		else if (cmd == "remove") {
			qh.extract_min();
		}
		else if (cmd == "find") {
			cout << qh.find_min() << endl;
		}
	}
}
