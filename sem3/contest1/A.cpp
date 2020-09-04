#include <iostream>
#include <vector>

using namespace std;

template<typename T>
using vvec = vector<vector<T>>;

struct DualGraph {
    vvec<int> left;
    vvec<int> right;
};

int main() {

}
