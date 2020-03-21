#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

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
};

template<typename T>
class RBTree {
using Node = RBNode<T>;	
private:	
	Node* root;	
	
	enum ROT_MODE {
		ROT_LEFT,
		ROT_RIGHT
	};

	void rotate(Node* node, ROT_MODE mode) {
		assert(node);

		Node* pivot = (mode == ROT_LEFT ? node->right : node->left);
		if (!pivot) return;

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
		//      gp           gp(r or b if root)
		//     /  \         /   \
		//   p(r) u(r) -> p(b)  u(b)   and fix  
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

		//      gp           gp
		//     /  \         /  \
		//   p(r) u(b) -> n(r) u(b)
		//      \         /
		//      n(r)    p(r)
		if (gp->left == node->parent && node->parent->right == node)
			rotate(node->parent, ROT_LEFT);
		//      gp           gp
		//     /  \         /  \
		//   u(b) p(r) -> u(b) n(r)
		//        /              \
		//      n(r)             p(r)
		else if (gp->right == node->parent && node->parent->left == node)
			rotate(node->parent, ROT_RIGHT);
		
		// parent is new node
		node = node->parent;
		
		node->parent->col = 'b';
		gp->col = 'r';

		if (node->parent->left == node && gp->left == node->parent)
			rotate(gp, ROT_RIGHT);
		else if (node->parent->right == node && gp->right == node->parent)
			rotate(gp, ROT_LEFT);
		else assert(0); // shouldn't be here
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

		if (ins->parent->col == 'r') // we got conflict here
			fixInsert(ins);

		return false;
	}

	bool erase(const T& val) {
		return true;
	}	
};

int main() {
	RBTree<int> tree;
	tree.insert(5);
}
