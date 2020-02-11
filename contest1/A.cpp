#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

typedef unsigned long long ull;

ull merge(ull* arr, ull l, ull r) {
	if (r == l + 1) return 0;
	
	ull p = (l + r) / 2;
	
	ull retval = 0;
	retval += merge(arr, l, p);
	retval += merge(arr, p, r);
	
	ull tmparr[r - l];
	ull pos = 0;
	
	ull lpos = l;
	ull rpos = p;
	
	while (lpos < p && rpos < r) {
		if (arr[lpos] > arr[rpos]) 
			retval += p - lpos;
		
		if (arr[lpos] < arr[rpos])
			tmparr[pos++] = arr[lpos++];
		else
			tmparr[pos++] = arr[rpos++];
	}
	
	while (lpos < p)
		tmparr[pos++] = arr[lpos++];
	
	while (rpos < r)
		tmparr[pos++] = arr[rpos++];
		
	for (int i = 0; i < pos; i++)
		arr[l + i] = tmparr[i];
	
	return retval;
}

int main() {
	ifstream in("inverse.in");
	ofstream out("inverse.out");
	
	ull n;
	in >> n;
	
	ull arr[n];
	for (ull i = 0; i < n; ++i) 
		in >> arr[i];
	
	out << merge(arr, 0, n) << endl;
	
	in.close();
	out.close();
}
