#include <iostream>
#include <sstream>
#include <set>

template <class T> inline std::string to_string (const T &t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

int main() {
	int n;
	std::cin >> n;
	if(n < 1 || n > 100) {
		std::cerr << "invalid input size\n";
		return 1;
	}
	int i = 0;
	std::set<int> setto;
	while(i < n) {
		long long num, cur = 0;
		int count = 1;
		int N = 0;
		std:: cin >> num;
		if(num == 0) {
			std::cout << "Case #" << i+1 << ": INSOMNIA" << std::endl;
		} else {
			while(N != -1) {
				cur += num;
				++N;
				std::string str = to_string(cur);
				for(int j = 0; j < str.size(); ++j) {
					int x = str[j] - '0';
					std::pair<std::set<int>::iterator, bool> ret = setto.insert(x);
					if(setto.size() == 10) {
						std::cout << "Case #" << i+1 << ": " << cur << std::endl;
						setto.clear();
						cur = 0;
						N = -1;
						break;
					}
				}
				++count;
			}
		}
		++i;
	}
	return 0;
}
