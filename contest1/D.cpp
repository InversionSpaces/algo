#include <iostream>
#include <deque>

using namespace std;

typedef unsigned long long ull;

int main() {
	ull n;
	cin >> n;
	
	deque<ull> left;
	deque<ull> right;
	
	ull tmp;
	while (n--) {
		char op;
		cin >> op;
		
		switch (op) {
			case '+':
				cin >> tmp;
				right.push_back(tmp);
			break;
			case '-':
				cout << left.front() << endl;
				left.pop_front();
			break;
			case '*':
				cin >> tmp;
				right.push_front(tmp);
			break;
		}
		
		while (right.size() > left.size()) {
			left.push_back(right.front());
			right.pop_front();
		}
	}
}
