#include <iostream>

using namespace std;

int main() {
	int n;
	cin >> n;
	
	int arr[n];
	for (int i = 0; i < n; ++i)
		cin >> arr[i];
	
	int q;
	cin >> q;
	
	int simple[n + 1] 	= {0}; // Для поддержки простого добавления
	int step[n + 1] 	= {0}; // Для поддержки шага суммарной прогрессии
	int diff[n + 1]		= {0}; // Для поддержки суммарного добавка от суммарной прогрессии
	
	int l, r, b, d;
	while (q--) {
		cin >> l >> r >> b >> d;
		
		simple[l] 		+= b;
		simple[r + 1] 	-= b;
		
		step[l]			+= d;
		step[r + 1]		-= d;
		
		diff[r + 1]		-= (r - l + 1) * d; // Сколько суммарно накопится от этой прогрессии
	}
	
	int cur_sum		= 0; // Простая сумма
	int cur_diff 	= 0; // Накомпленная сумма прогрессий
	int cur_step 	= 0; // Текущей шаг суммарной прогресии
	for (int i = 0; i < n; ++i) {
		cur_sum 	+= simple[i];
		cur_diff	+= cur_step + diff[i];
		
		cout << arr[i] + cur_sum + cur_diff << " ";
		
		cur_step 	+= step[i];
	}
}
