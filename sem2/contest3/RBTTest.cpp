#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>
#include <string>
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <utility>
#include <cstdio>

#define NDEBUG

using namespace std;

template<typename T>
struct RBNode {
using Node = RBNode<T>;
	char col;
	
	Node* parent;
	Node* left;
	Node* right;

	T val;
	unsigned int ssize;
	T ssum;

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

	inline void recalc_size() noexcept {
		ssize = 1;
		ssize += left ? left->ssize : 0;
		ssize += right ? right->ssize : 0;
	}

	inline void recal_sum() noexcept {
		ssum = val;
		ssum += left ? left->ssum : 0;
		ssum += right ? right->ssum : 0;
	}

	inline unsigned int lsize() const noexcept {
		return left ? left->ssize : 0;
	}

	inline unsigned int rsize() const noexcept {
		return right ? right->ssize : 0;
	}

	void dump(ostream& out) const noexcept {
		out 	<< "node_" 
			<< this 
			<< " [shape=\"circle\", style=\"filled\", color=";
		
		if (col == 'r')
			out << "\"red\",";	
		else
			out << "\"black\", fontcolor=\"white\", ";
		
		out 	<< "label=\""
			<< this->val 
			//<< " {"
			//<< this->ssize
			//<< ", "
			//<< this->ssum
			//<< "}\"]\n";
			<< "\"]\n";

		if (left) {
			assert(left->parent == this);
			left->dump(out);
			
			out 	<< "node_" 
				<< this 
				<< "-> node_" 
				<< left 
				<< ";\n";
		}
		else {
			out 	<< "null1_" 
				<< this 
				<< "[shape=\"circle\", \
					style=\"filled\", \
					fill=\"solid\", \
					fillcolor=\"black\", \
					label=\"n\"]\n";
			out 	<< "node_" 
				<< this 
				<< "-> null1_" 
				<< this 
				<< ";\n";
		}

		if (right) {
			assert(right->parent == this);
			right->dump(out);
			
			out 	<< "node_" 
				<< this 
				<< "-> node_" 
				<< right 
				<< ";\n";
		}
		else {
			out 	<< "null2_" 
				<< this 
				<< "[shape=\"circle\", \
					style=\"filled\", \
					fill=\"solid\", \
					fillcolor=\"black\", \
					label=\"n\"]\n";
			out 	<< "node_" 
				<< this 
				<< "-> null2_" 
				<< this 
				<< ";\n";
		}

		assert(ssize == 1 + (left ? left->ssize : 0) + (right ? right->ssize : 0));
		assert(ssum == val + (left ? left->ssum : 0) + (right ? right->ssum : 0));
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

		node->recalc_size();
		pivot->recalc_size();

		node->recal_sum();
		pivot->recal_sum();
	}

	void fix_insert(Node* node) {
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
					fix_insert(gp);
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

	void fix_erase(Node* node, Node* parent) {
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
					fix_erase(parent, parent->parent);
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
				
				// update sibling after rotate
				sib = (node == parent->left ? parent->right : parent->left);
			}
			else if (	node == parent->right &&
					(!sib->left || sib->left->col == 'b')){
				// due to previous cases sib->right
				// must now exist and be red
				sib->col = 'r';
				sib->right->col = 'b';
				
				rotate(sib, ROT_LEFT);
				
				// update sibling after rotate
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

	RBTree(Node* pointer) : root(pointer) {
	}

	bool insert(const T& val) {
		if (!root) {
			root = new Node {'b', nullptr, nullptr, nullptr, val, 1, val};

			return false;
		}

		vector<Node*> path;

		Node* prev = nullptr;
		for(Node* cur = root; cur;) {
			path.push_back(cur);

			prev = cur;
			if (cur->val < val)
				cur = cur->right;
			else if (cur->val > val)
				cur = cur->left;
			else return true; // there is already such key
		}
		
		assert(prev);

		// prev is father of ins 
		Node* ins = new Node {'r', prev, nullptr, nullptr, val, 1, val};
		// choose which children of prev ins is
		(prev->val < val ? prev->right : prev->left) = ins;

		for (const auto& node: path) {
			node->ssize += 1;
			node->ssum += val;
		}

		if (prev->col == 'r') // we got conflict here
			fix_insert(ins);

		return false;
	}

	bool erase(const T& val) {
		Node* cur = root;
		
		vector<Node*> path;

		// find val in BST
		while (cur) {
			path.push_back(cur);
			
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
			while (minl->left) {
				path.push_back(minl);
				minl = minl->left;
			}

			swap(cur->val, minl->val);
			cur = minl;
		}
		
		for (const auto& node: path) {
			node->ssize -= 1;
			node->ssum -= cur->val;
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
			if (child && child->col == 'r') {
				if (cur == root)
					root = child;
				child->col = 'b';
			}
			else if (cur == root)
				root = child;
			else
				fix_erase(child, cur->parent);
		}

		delete cur;

		return true;
	}

	optional<const T> next(const T& val) const noexcept {
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

	bool contains(const T& val) const noexcept {
		for (Node* cur = root; cur;) {
			if (cur->val < val)
				cur = cur->right;
			else if (val < cur->val)
				cur = cur->left;
			else return true;
		}

		return false;
	}

	size_t size() const noexcept {
		return root ? root->ssize : 0;
	}

	const T& k_stat(unsigned int k) const {
		if (k >= size())
			throw invalid_argument("k_stat: k is larger than size");
		
		Node* cur = root;
		while (cur) {
			if (k < cur->lsize())
				cur = cur->left;
			else if (k > cur->lsize()) {
				k -= cur->lsize() + 1;
				cur = cur->right;
			}
			else return cur->val;
		}

		assert(!"Impossible case");
	}

	~RBTree() {
		if (!root) return;

		vector<Node*> st = {root};

		while (!st.empty()) {
			Node* cur = st.back();
			st.pop_back();
			
			if (cur->left) st.push_back(cur->left);
			if (cur->right) st.push_back(cur->right);

			delete cur;
		}
	}

	T sum_greater(const T& val) const noexcept {
		T sum = 0;

		Node* cur = root;
		while (cur) {
			if (val < cur->val) { 
				sum += cur->val;
				sum += cur->right ? cur->right->ssum : 0;

				cur = cur->left;
			}
			else 
				cur = cur->right;

		}

		return sum;
	}

	T sum_lower(const T& val) const noexcept {
		T sum = 0;

		Node* cur = root;
		while (cur) {
			if (cur->val < val) {
				sum += cur->val;
				sum += cur->left ? cur->left->ssum : 0;

				cur = cur->right;
			}
			else 
				cur = cur->left;
		}

		return sum;
	}

	T sum() const noexcept {
		return root ? root->ssum : 0;
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
	string cmd = string("dot /tmp/tmp.dot -Tpng -Gdpi=300 -o ./snapshot") + to_string(i) + string(".png");
	system(cmd.c_str());
	cout << "snapshot: " << i++ << endl;
}

typedef unsigned long long ull;

int main() {
	srand(time(NULL));

	RBTree<ull> tree;
	for (int i = 0; i < 15; ++i)
		tree.insert(rand() % 100);

	snapshot(tree);
}
