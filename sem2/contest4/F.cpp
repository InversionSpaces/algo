#define NDEBUG
#pragma GCC optimization ("O3")
#pragma GCC optimization ("unroll-loops")

#include <iostream>
#include <cassert>

using namespace std;

typedef int ll;

struct minid {
	ll min, min_id;
};

struct STNode {
	ll l, r;
	minid m;

	bool need_push;
	ll push_val;
	
	STNode* left;
	STNode* right;
};

STNode* build(const ll l, const ll r) {
	assert(l <= r);

	STNode* node = new STNode {l, r, {0, -1}, false};

	if (l == r) {
		node->right = nullptr;
		node->left = nullptr;

		return node;
	}

	ll mid = (l + r) / 2;
	
	node->left = build(l, mid);
	node->right = build(mid + 1, r);

	return node;
}

inline void push(STNode* node) {
	assert(node);

	if (node->need_push) {
		node->need_push = false;
		
		if (node->left && node->right) {
			STNode* left = node->left;
			STNode* right = node->right;

			left->m.min += node->push_val;
			right->m.min += node->push_val;

			if (left->need_push)
				left->push_val += node->push_val;
			else {
				left->need_push = true;
				left->push_val = node->push_val;
			}
			
			if (right->need_push)
				right->push_val += node->push_val;
			else {
				right->need_push = true;
				right->push_val = node->push_val;
			}
		}
	}
}

inline minid choose(const minid& left, const minid& right) {
	if (left.min_id == -1 && right.min_id == -1)
		return {min(left.min, right.min), -1};
	
	if (left.min_id != -1 && right.min_id != -1) {
		if (left.min < right.min) 
			return left;
		return right;
	}
	
	if (left.min_id == -1) 
		return right;
	
	return left;
}

void update(STNode* node, ll l, ll r, ll dif) {
	assert(node);
	assert(l <= r);
	assert(max(l, node->l) <= min(r, node->r));

	push(node);
	
	if (node->l == l && node->r == r) {
		node->need_push = true;
		node->push_val = dif;
		node->m.min += dif;

		return;
	}

	ll mid = (node->l + node->r) / 2;
	if (r > mid)
		update(node->right, max(mid + 1, l), r, dif);
	if (l < mid + 1)
		update(node->left, l, min(mid, r), dif);

	node->m = choose(node->left->m, node->right->m);
}


minid get(STNode* node, ll l, ll r) {
	assert(node);
	assert(l <= r);
	assert(max(l, node->l) <= min(r, node->r));

	push(node);

	if (node->l == l && node->r == r)
		return node->m;

	ll mid = (node->l + node->r) / 2;
	if (l > mid)
		return get(node->right, l, r);
	if (r < mid + 1)
		return get(node->left, l, r);

	minid left = get(node->left, l, mid);
	minid right = get(node->right, mid + 1, r);

	return choose(left, right);
}

void add(STNode* node, ll id) {
	assert(node);
	
	push(node);

	if (node->l == node->r) {
		assert(id == node->l);
		node->m.min_id = id;
	}
	else {
		ll mid = (node->l + node->r) / 2;
		if (id > mid) add(node->right, id);
		else add(node->left, id);

		node->m = choose(node->left->m, node->right->m);
	}
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);

	ll n;
	cin >> n;

	STNode* root1 = build(0, n - 1);
	STNode* root2 = build(0, n - 1);

	for (ll i = 0; i < n; ++i) {
		ll r, s;
		cin >> r >> s;
		--r;

		if (s == 1) {
			if (r > 0) {
				update(root1, 0, r - 1, -1);
				update(root2, 0, r - 1, 1);
			}
			add(root1, r);
		}
		else if (s == 2) {
			if (r > 0) {
				update(root1, 0, r - 1, 1);
				update(root2, 0, r - 1, -1);
			}
			add(root2, r);
		}
		else {
			cout << "bruh";
			return 0;
		}
		/*
		cout << "1: ";
		for (int i = 0; i < n; ++ i) {
			minid m = get(root1, i, i);
			if (m.min_id == -1) cout << m.min << '\t';
			else cout << '[' << m.min << "]\t";
		}
		cout << "\n2: ";
		for (int i = 0; i < n; ++ i) {
			minid m = get(root2, i, i);
			if (m.min_id == -1) cout << m.min << '\t';
			else cout << '[' << m.min << "]\t";
		}
		cout << '\n';
		*/

		minid one = get(root1, 0, n - 1);
		minid two = get(root2, 0, n - 1);

		if (one.min_id == -1) {
			cout << "<\n";
			continue;
		}

		if (two.min_id == -1) {
			cout << ">\n";
			continue;
		}

		if (one.min > 0) {
			cout << "<\n";
			continue;
		}

		if (two.min > 0) {
			cout << ">\n";
			continue;
		}

		cout << "?\n";
	}
}
