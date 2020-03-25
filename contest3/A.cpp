#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>
#include <string>
#include <cstdlib>
#include <optional>

using namespace std;

#define NDEBUG 1

template<typename T>
struct RBNode {
using Node = RBNode<T>;
	char col;
	
	Node* parent;
	Node* left;
	Node* right;

	T val;

	inline Node* grandparent() const noexcept {
		if (parent)
			return parent->parent;
		return nullptr;
	}

	inline Node* uncle() const noexcept {
		auto gp = grandparent();
		if (!gp)
			return nullptr;

		if (parent == gp->left)
			return gp->right;

		return gp->left;
	}

	void dump(ostream& out) {
		out << "node_" << this;
		if (col == 'r') 
			out << " [shape=\"circle\", color=\"red\", label=\"" << this->val << "\"]\n";
		else
			out << " [shape=\"circle\", color=\"black\", label=\"" << this->val << "\"]\n";

		if (left) {
			assert(left->parent == this);
			left->dump(out);
			out << "node_" << this << "->" << "node_" << left << ";\n";
		}
		else {
			out << "null1_" << this << "[shape=\"circle\", style=\"filled\", fill=\"solid\", fillcolor=\"black\", label=\"n\"]\n";
			out << "node_" << this << "->" << "null1_" << this << ";\n";
		}

		if (right) {
			assert(right->parent == this);
			right->dump(out);
			out << "node_" << this << "->" << "node_" << right << ";\n";
		}
		else {
			out << "null2_" << this << "[shape=\"circle\", style=\"filled\", fill=\"solid\", fillcolor=\"black\", label=\"n\"]\n";
			out << "node_" << this << "->" << "null2_" << this << ";\n";
		}
	}
};

template<typename T>
class RBTree;

template<typename T>
void snapshot(const RBTree<T>& tree);

template<typename T>
class RBTree {
using Node = RBNode<T>;	
private:	
	Node* root;	
	
	enum ROT_MODE {
		ROT_LEFT,
		ROT_RIGHT
	};

	// ROT_LEFT:
	//   |          |
	//   n          b
	//  / \        / \
	// a   b  ->  n  br
	//    / \    / \
	//   bl br  a  bl
	// ROT_RIGHT:
	//     |         |
	//     n         a
	//    / \       / \
	//   a   b ->  al  n
	//  / \           / \
	// al ar         ar  b
	void rotate(Node* node, ROT_MODE mode) {
		assert(node);

		Node* pivot = (mode == ROT_LEFT ? node->right : node->left);
		
		assert(pivot);

		Node* parent = node->parent;
		pivot->parent = parent;
		node->parent = pivot;
		
		if (!parent) 
			root = pivot;
		else if (node == parent->left) 
			parent->left = pivot;
		else
			parent->right = pivot;

		switch (mode) {
		case ROT_LEFT:
			node->right = pivot->left;
			if (pivot->left) 
				node->right->parent = node;
			pivot->left = node;
		break;
		
		case ROT_RIGHT:
			node->left = pivot->right;
			if (pivot->right)
				node->left->parent = node;
			pivot->right = node;	
		break;
		}
	}

	void fixInsert(Node* node) {
		assert(node);
		assert(node->col == 'r'); // fixInsert called only on red nodes
		assert(node->parent->col == 'r'); // fixInsert called only if there is conflict

		Node* gp = node->grandparent();

		assert(gp); // grandparent must exist
		assert(gp->col == 'b'); // it must be black

		Node* uncle = node->uncle();
		
		// case 1:
		//     gp(b)  gp(r or b if root)
		//     /  \         /   \
		//   p(r) u(r) -> p(b)  u(b)    and fix  
		//   /            /             conflict
		// n(r)         n(r)            recursively
		if (uncle && uncle->col == 'r') {
			node->parent->col = 'b';
			uncle->col = 'b';
			
			// may push conflict up
			// if gp is root just keep it black
			if (gp != root) {
				// else - paint it red 
				gp->col = 'r';
				// if got conflict - resolve recursively
				if (gp->parent->col == 'r')
					fixInsert(gp);
			}

			return;
		}

		//     gp(b)        gp(b)
		//     /  \         /  \
		//   p(r) u(b) -> n(r) u(b)
		//      \         /
		//      n(r)    p(r)
		if (gp->left == node->parent && node->parent->right == node) {
			// parent is new node
			node = node->parent;
			
			rotate(node, ROT_LEFT);
		}
		//      gp           gp
		//     /  \         /  \
		//   u(b) p(r) -> u(b) n(r)
		//        /              \
		//      n(r)             p(r)
		else if (gp->right == node->parent && node->parent->left == node) {
			// parent is new node
			node = node->parent;

			rotate(node, ROT_RIGHT);	
		}
		
		// case 2:
		//     gp(b)        p(b)
		//     /  \         /  \
		//   p(r) u(b) -> n(r) gp(r)
		//   /                   \
		// n(r)                  u(b)
		//    or mirrored version
		node->parent->col = 'b';
		gp->col = 'r';

		if (node->parent->left == node && gp->left == node->parent)
			rotate(gp, ROT_RIGHT);
		else if (node->parent->right == node && gp->right == node->parent)
			rotate(gp, ROT_LEFT);
		else assert(!"Impossible case"); // shouldn't be here
	}

	void fixErase(Node* node, Node* parent) {
		assert(parent);

		Node* sib = (node == parent->left ? parent->right : parent->left);

		assert(sib); // sib must exist
		
		// case 1
		if (sib->col == 'r') {
			parent->col = 'r';
			sib->col = 'b';

			if (node == parent->left)
				rotate(parent, ROT_LEFT);
			else
				rotate(parent, ROT_RIGHT);
			
			// update sibling after rotate
			sib = (node == parent->left ? parent->right : parent->left);

			assert(sib); // sib still must exist
		}
		
		// now sib is certainly black 

		if (	(!sib->left || sib->left->col == 'b') && 
			(!sib->right || sib->right->col == 'b')) {
			
			// case 2
			if (parent->col == 'b') {
				sib->col = 'r';
				if (parent != root)
					fixErase(parent, parent->parent);
			}
			// case 3
			else  {
				sib->col = 'r';
				parent->col = 'b';
			}
		}
		else {	
			// case 4
			if (	node == parent->left &&
				(!sib->right || sib->right->col == 'b')) {
				// due to previos cases sib->left 
				// now must exist and be red
				sib->col = 'r';
				sib->left->col = 'b';
				
				rotate(sib, ROT_RIGHT);
				sib = (node == parent->left ? parent->right : parent->left);
			}
			else if (	node == parent->right &&
					(!sib->left || sib->left->col == 'b')){
				// due to previous cases sib->right
				// must now exist and be red
				sib->col = 'r';
				sib->right->col = 'b';
				
				rotate(sib, ROT_LEFT);
				sib = (node == parent->left ? parent->right : parent->left);
			}
			
			// case 5
			sib->col = parent->col;
			parent->col = 'b';

			if (node == parent->left) {
				sib->right->col = 'b';
				rotate(parent, ROT_LEFT);
			}
			else {
				sib->left->col = 'b';
				rotate(parent, ROT_RIGHT);
			}
		}
	}

public:
	RBTree() : root(nullptr) {
	}

	bool insert(const T& val) {
		if (!root) {
			root = new Node {'b', nullptr, nullptr, nullptr, val};

			return false;
		}

		Node* prev = nullptr;
		for(Node* cur = root; cur;) {
			prev = cur;
			if (cur->val < val)
				cur = cur->right;
			else if (cur->val > val)
				cur = cur->left;
			else return true; // there is already such key
		}
		
		assert(prev);

		// prev is father of ins 
		Node* ins = new Node {'r', prev, nullptr, nullptr, val};
		// choose which children of prev ins is
		(prev->val < val ? prev->right : prev->left) = ins; 

		if (prev->col == 'r') // we got conflict here
			fixInsert(ins);

		return false;
	}

	bool erase(const T& val) {
		Node* cur = root;
		
		// find val in BST
		while (cur) {
			if (cur->val < val)
				cur = cur->right;
			else if (val < cur->val)
				cur = cur->left;
			else break;
		}
		
		// there is no such val
		if (!cur) return false;

		// if cur has both children - replace its
		// value with minimum of right subtree
		if (cur->left && cur->right) {
			Node* minl = cur->right;
			while (minl->left)
				minl = minl->left;

			swap(cur->val, minl->val);
			cur = minl;
		}

		// now cur has at most one not null child
		Node* child = cur->right ? cur->right : cur->left;
		
		// replace cur with child
		if (child) 
			child->parent = cur->parent;

		if (cur->parent) {
			if (cur == cur->parent->left) 
				cur->parent->left = child;
			else
				cur->parent->right = child;
		}

		// if cur->col == 'r' replace was safe
		if (cur->col == 'b') {
			// if child was red just paint it black
			if (child && child->col == 'r')
				child->col = 'b';
			else if (cur == root)
				root = child;
			else
				fixErase(child, cur->parent);
		}

		delete cur;

		return true;
	}

	optional<const T> next(const T& val) {
		Node* cur = root;
		Node* ans = nullptr;

		while (cur) {
			if (cur->val < val) 
				cur = cur->right;
			else { // cur->val >= val
				if (!ans || cur->val < ans->val)
					ans = cur;
				cur = cur->left;
			}
		}

		if (!ans) return nullopt;

		return ans->val;
	}

	bool contains(const T& val) {
		for (Node* cur = root; cur;) {
			if (cur->val < val)
				cur = cur->right;
			else if (val < cur->val)
				cur = cur->left;
			else return true;
		}

		return false;
	}
	
	template<typename U>
	friend ostream& operator<<(ostream& out, const RBTree<U>& tree);
};

template<typename T>
ostream& operator<<(ostream& out, const RBTree<T>& tree) {
	out << "digraph RBTree {\n";
	tree.root->dump(out);
	out << "}\n";

	return out;
}

template<typename T>
void snapshot(const RBTree<T>& tree) {
	static int i = 0;
	ofstream tmp("/tmp/tmp.dot");
	tmp << tree;
	tmp.close();
	string cmd = string("dot /tmp/tmp.dot -Tpng -Gdpi=300 -o ./snapshot") + to_string(i++) + string(".png");
	system(cmd.c_str());
	cout << "snapshot: " << i << endl;
}

typedef long long ll;

const ll mod = 1000000000;

int main() {
	RBTree<ll> tree;

	ll n;
	cin >> n;

	bool is_next = false;
	ll last_next = 0;
	for (ll i = 0; i < n; ++i) {
		char op;
		cin >> op;
		
		ll tmp;
		cin >> tmp;

		if (op == '+') {
			if (is_next) {
				is_next = false;
				tmp = (last_next + tmp) % mod;
			}
			
			tree.insert(tmp);
		}
		else if (op == '?') {
			is_next = true;

			auto res = tree.next(tmp);
			if (res)
				last_next = *res;
			else
				last_next = -1;

			cout << last_next << "\n";
		}
	}
	// test
	/*
	srand(3);

	RBTree<int> tree;
	for (int i = 0; i < 15; ++i) {
		int ins = rand() % 100;
		cout << ins << endl;
		tree.insert(ins);
		assert(tree.contains(ins));
	}

	snapshot(tree);

	int find = rand() % 100;
	cout << "find: " << find << endl;
	auto next = tree.next(find);
	cout << (next ? *next : -1) << endl;
	*/
}
