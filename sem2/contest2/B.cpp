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
#include <set>

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

vector<char> is_relevant;
vector<blist*> requests;
quickheap<bheap> heap;//(1UL << 24); 
size_t fsize;

bool operator<(const bheap& a, const bheap& b) {
	if (a.size == b.size)
		return a.id < b.id;
	
	return a.size > b.size;
}

inline void xmalloc(size_t rsize) 
{
	// extract all not relevant blocks
	while (!heap.empty() && !is_relevant.at(heap.find_min().id))
		heap.extract_min();
	
	// have no space for request
	if (heap.empty() || heap.find_min().size < rsize) {
		cout << "-1" << endl;
		
		// failed request
		requests.push_back(nullptr);
		
		return;
	}
	
	blist* block = heap.extract_min().pos;
	
	assert(block);
	assert(block->is_free);
	
	block->is_free = false;
	is_relevant.at(block->id) = false;
	
	// request succeded
	requests.push_back(block);
	
	cout << block->start + 1 << "\n";
	
	blist* const next = block->next;
	const ll size = (next ? next->start : fsize) - block->start;
	
	if (size == rsize)
		return;
	
	// insert new block in list {
	blist* nblock = new blist {	
							next, 					// next
							block, 					// prev
							block->start + rsize,	// start
							is_relevant.size(),			// id
							true 					// is_free
						};
	block->next = nblock;
	if (nblock->next)
		nblock->next->prev = nblock;
	// } now block <-> nblock <-> next
	
	// insert nblock in heap
	heap.insert({
					nblock, 		// pos
					is_relevant.size(), 	// id
					size - rsize	// size
				});
	
	// is_relevantelevant[nblock->id] = true
	is_relevant.push_back(true);	
}

inline void xfree(size_t id)
{	
	// dummy request to support indexing
	requests.push_back(nullptr);
	
	blist* cur = requests.at(id);
	
	// it was failed request
	if (!cur)
		return;
	
	assert(!is_relevant.at(cur->id)); 
	
	// collapse with previous block {
	blist* prev = cur->prev;
	if (prev && prev->is_free) {
		is_relevant.at(prev->id) = false;
		
		cur->prev = prev->prev;
		cur->start = prev->start;
		
		if (cur->prev)
			cur->prev->next = cur;
		
		delete prev;
	}
	// }
	
	// collapse with next block {
	blist* next = cur->next;
	if (next && next->is_free) {
		is_relevant.at(next->id) = false;
		
		cur->next = next->next;
		
		if (cur->next)
			cur->next->prev = cur;
		
		delete next;
	}
	// }
	
	cur->is_free = true;
	cur->id = is_relevant.size();
	
	// is_relevantelevamt[cur->id] = true
	is_relevant.push_back(true);
	
	// insert new element in heap
	heap.insert({
		cur, 												// pos
		cur->id, 											// id
		(cur->next ? cur->next->start : fsize) - cur->start	// size
	});					
}

#define CF_LET_ME_IIIIIIIIIIN 1

int main() {
	size_t m;
	cin >> fsize >> m;
	
	// first empty block
	blist* first = new blist{	
								nullptr,	//next
								nullptr,	//prev
								0,			//start
								0, 			//id
								true		//is_free
							};
	// dummy block
	blist* head = new blist{first}; 
	// head <-> first
	first->prev = head;
	
	heap.insert({first, 0, fsize});
	
	// is_relevantelevant[first->id] = true
	is_relevant.push_back(true);
	
	for (ll i; m--;) {
		cin >> i;
		
		if (i > 0)
			xmalloc(i);
		else
			xfree(-i - 1);
	}
	
	// delete
	while (head) {
		blist* tmp = head->next;
		delete head;
		head = tmp;
	}
}
