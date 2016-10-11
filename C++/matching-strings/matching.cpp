#include <iostream>
#include <string>
#include <stack>
#include <fstream>

int findMatchingPairs(std::string input) {
	std::stack<char> stack;
	char curChar = input[0];
	int curIdx = 0;
	while(curIdx < input.length()) {
		if('a' <= curChar && curChar <= 'z') {
			// lowercase
			if(stack.empty()) return curIdx;
			char match = stack.top();
			stack.pop();
			match = match - 'A' + 'a';
			if(match != curChar) return curIdx;
		}
		else {
			// uppercase
			stack.push(curChar);
		}
		curChar = input[++curIdx];
	}
	return curIdx;
}

int main() {
	std::ifstream inf("matching.txt");
	std::string input;
	while(inf >> input) {
		std::cout << findMatchingPairs(input) << std::endl;
	}
	return 0;
}
