#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>
#include <ctime>

using namespace std;

template<typename keyT, typename valT, uint8_t maxl=8>
class skiplist 
{
private:

	struct node {
		vector<node*> next;
		
		keyT key;
		valT val;
	};
	
	node* head;
	node* tail;
public:	

	skiplist()
	{
		tail = new node;
		head = new node {{maxl, tail}};
	}
	
	bool update(keyT key, valT val) {
		node* path[maxl];
		node* current = head;
		
		for (uint8_t i = 0; i < maxl; ++i) {
			const uint8_t level = maxl - i - 1;
			
			while (	current->next[level] != tail &&
					current->next[level]->key < key) 
				current = current->next[level];
			
			path[level] = current;
		}
		
		current = current->next[0];
		if (current != tail &&
			key == current->key) {
			current->val = val;
			
			return false;
		}
		
		uint8_t height = rand() % (maxl - 1) + 1;
		node* inserted = new node {{height, nullptr}, key, val};

		for (uint8_t i = 0; i < height; ++i) {
			inserted->next[i] = path[i]->next[i];
			path[i]->next[i] = inserted;
		}
		
		return true;
	}
	
	valT get(keyT key) {
		node* current = head;
		
		for (uint8_t i = 0; i < maxl; ++i) {
			const uint8_t level = maxl - i - 1;
			
			while (	current->next[level] != tail &&
					current->next[level]->key < key) 
				current = current->next[level];
		}
		
		current = current->next[0];
		if (current == tail ||
			current->key != key) 
			throw invalid_argument("No such key");
		
		return current->val;
	}
	
	valT remove(keyT key) {
		node* path[maxl];
		node* current = head;
		
		for (uint8_t i = 0; i < maxl; ++i) {
			const uint8_t level = maxl - i - 1;
			
			while (	current->next[level] != tail &&
					current->next[level]->key < key) 
				current = current->next[level];
			
			path[level] = current;
		}
		
		current = current->next[0];
		if (current == tail ||
			key != current->key) 
			throw invalid_argument("No such key");
		
		uint8_t height = current->next.size();
		
		for (uint8_t i = 0; i < height; ++i)
			path[i]->next[i] = current->next[i];
		
		valT retval = current->val;
		delete current;
		
		return retval;
	}
	
	bool contains(keyT key) {
		node* current = head;
		
		for (uint8_t i = 0; i < maxl; ++i) {
			const uint8_t level = maxl - i - 1;
			
			while (	current->next[level] != tail &&
					current->next[level]->key < key) 
				current = current->next[level];
		}
		
		current = current->next[0];
		return (current == tail || current->key != key) ? false : true;
	}
	
	void print() {
		cout << "head\t";
		for (const auto& i: head->next) {
			if (i == tail)
				cout << "[tail]\t";
			else
				cout << '[' << i->key << "]\t";
		}
		cout << endl;
		
		for (	node* current = head->next[0]; 
				current != tail; 
				current = current->next[0]) {
			cout << current->key << ":" << current->key << "\t";
			for (const auto& i: current->next) {
				if (i == tail)
					cout << "[tail]\t";
				else
					cout << '[' << i->key << "]\t";
			}
			cout << endl;
		}
		
		cout << "tail\t";
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
	}
};

int main() {
	srand(time(NULL));
	
	skiplist<int, int> sl;
	
	sl.update(0, 100);
	sl.update(10, 110);
	sl.update(5, 210);
	sl.update(4, 120);
	sl.update(6, 600);
	
	//sl.print();
	
	cout << sl.contains(7) << endl;
	
	//sl.print();
}
