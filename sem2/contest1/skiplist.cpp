#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>
#include <ctime>
#include <string>
#include <utility>

using namespace std;

template<typename keyT, typename valT, uint8_t maxl=64>
class skiplist 
{
private:
	discrete_distribution<uint8_t>* dist;
	mt19937*						gen;

	size_t _size;

	struct node {
		vector<node*> next;
		
		keyT key;
		valT val;
	};
	
	node* head;
	node* tail;
	
	inline node* find(keyT key, node* path[maxl] = nullptr) {
		node* current = head;
		
		for (uint8_t i = 0; i < maxl; ++i) {
			const uint8_t level = maxl - i - 1;
			
			while (	current->next[level] != tail &&
					current->next[level]->key < key) 
				current = current->next[level];
			
			if (path != nullptr) 
				path[level] = current;
		}
		
		return current->next[0];
	}
	
public:	

	skiplist() : _size(0)
	{
		tail = new node;
		head = new node {{maxl, tail}};
		
		vector<double> weights(maxl);
		double weight = 0.5;
		for (auto& i: weights) {
			i = weight;
			weight /= 2.0;
		}
		
		dist = new discrete_distribution<uint8_t>(weights.begin(), weights.end());
		gen = new mt19937;
		gen->seed(time(NULL));
	}
	
	size_t size() {
		return _size;
	}
	
	bool update(keyT key, valT val) {
		node* path[maxl];
		node* current = find(key, path);
		
		if (current != tail && key == current->key) {
			current->val = val;
			
			return false;
		}
		
		uint8_t height = (*dist)(*gen) + 1;
		node* inserted = new node {{height, nullptr}, key, val};

		for (uint8_t i = 0; i < height; ++i) {
			inserted->next[i] = path[i]->next[i];
			path[i]->next[i] = inserted;
		}
		
		++_size;
		
		return true;
	}
	
	valT get(keyT key) {
		node* current = find(key);
		
		if (current == tail || current->key != key) 
			throw invalid_argument("No such key");
		
		return current->val;
	}
	
	valT remove(keyT key) {
		node* path[maxl];
		node* current = find(key, path);
		
		if (current == tail || key != current->key) 
			throw invalid_argument("No such key");
		
		for (uint8_t i = 0; i < current->next.size(); ++i)
			path[i]->next[i] = current->next[i];
		
		valT retval = current->val;
		delete current;
		
		--_size;
		
		return retval;
	}
	
	bool contains(keyT key) {
		node* current = find(key);
		
		return (current == tail || current->key != key) ? false : true;
	}
	
	pair<keyT, valT> remove_first() {
		if (!_size)
			throw invalid_argument("Remove first on empty skiplist");
			
		keyT key = head->next[0]->key;
		
		return {key, remove(key)};
	}
	
	void print() {
		cout << "head\t\t";
		
		for (const auto& i: head->next) 
			if (i == tail) cout << "[tail]\t";
			else cout << '[' << i->key << "]\t";
		
		cout << endl;
		
		for (	node* current = head->next[0]; 
				current != tail; 
				current = current->next[0]) {
			cout << current->key << ":\t" << current->val << "\t";
			
			for (const auto& i: current->next)
				if (i == tail) cout << "[tail]\t";
				else cout << '[' << i->key << "]\t";
			
			cout << endl;
		}
		
		cout << "tail\t\t";
		
		for (uint8_t i = 0; i < maxl; ++i)
			cout << "[-]\t";
		
		cout << endl;
	}
	
	~skiplist() {
		node* current = head;
		node* tmp;
		
		while (current != tail) {
			tmp = current->next[0];
			delete current;
			current = tmp;
		}
		
		delete current;
		
		delete dist;
		delete gen;
	}
};

typedef unsigned long long ull;

int main() {
	ull n;
	cin >> n;
	
	vector<ull> prices(n);
	vector<ull> a(n);
	vector<ull> b(n);
	
	for (auto& i: prices) 
		cin >> i;
		
	for (auto& i: a) {
		cin >> i; i--;
	}
		
	for (auto& i: b) {
		cin >> i; i--;
	}
		
	skiplist<ull, int> shirts[3];
	
	for (ull i = 0; i < n; ++i) {
		shirts[a[i]].update(prices[i], b[i]);
		if (a[i] != b[i]) shirts[b[i]].update(prices[i], a[i]);
	}
	
	ull m;
	cin >> m;
	
	while (m--) {
		int p_color;
		cin >> p_color;
		--p_color;
		
		if (shirts[p_color].size() == 0) {
			cout << "-1 " << endl;
			
			continue;
		}
		
		auto [price, a_color] = shirts[p_color].remove_first();
		if (a_color != p_color) shirts[a_color].remove(price);
		
		cout << price << " ";
	}
	
	cout << endl;
}
