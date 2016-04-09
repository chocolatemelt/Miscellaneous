#include <iostream>
#include <string>

int flipPancakes(std::string s, char target) {
	// flipping subproblems (divide and conquer)
	for(int i = s.size() - 1; i >= 0; --i) {
		if(s[i] != target) {
			if(target == '+') return(flipPancakes(s.substr(0, i+1), '-')) + 1;
			else return(flipPancakes(s.substr(0, i+1), '+')) + 1;
		}
	}
	return 0;
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
		std::cout << "Case #" << i+1 << ": " << flipPancakes(pancakes, '+') << std::endl;
		++i;
	}
	return 0;
}
