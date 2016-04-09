#include <iostream>
#include <sstream>
#include <cstdlib>

template <class T> inline std::string to_string (const T &t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

void flip(char *one, char *two) {
	char temp = *one;
	*one = *two;
	*two = temp;
}

void pancake(char *arr, int start, int end) {
	int dif = abs(start-end);
	for(int i = 0; i <= dif / 2; ++i) {
		char temp = arr[start+i];
		arr[start+i] = arr[end-i];
		arr[end-i] = temp;
	}
	for(int i = start; i <= end; ++i) {
		if(arr[i] == '-') arr[i] = '+';
		else arr[i] = '-';
	}
}

int main() {
	int n;
	std::cin >> n;
	if(n < 1 || n > 100) {
		std::cerr << "invalid input size\n";
		return 1;
	}
	int i = 0;
	while(i < n) {
		std::string pancakes;
		std:: cin >> pancakes;
		std::cout << pancakes;
		++i;
	}
	return 0;
}
