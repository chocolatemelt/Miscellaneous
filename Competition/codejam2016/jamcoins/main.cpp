#include <iostream>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>

std::vector<int> SieveOfEratosthenes(int n) {
	// geekforgeeks sieve function modified for this problem
	std::vector<int> ret;
	bool prime[n+1];
	memset(prime, true, sizeof(prime));

	for (int p=2; p*p<=n; p++) {
		if (prime[p] == true) {
			for (int i=p*2; i<=n; i += p)
				prime[i] = false;
		}
	}

	for (int p=2; p<=n; p++)
		if (prime[p])
			ret.push_back(p);
}

int basechange(std::string num, int base) {
	// quick wrapper
	return std::stoi(num, 0, base);
}

bool verify(std::string s) {
	// verifies a jamcoin
	for(int i = 2; i <= 10; ++i) {
		int chk = basechange(s, i);
		std::vector<int> lst = SieveOfEratosthenes(chk);
		if(lst.empty()) return false;
	}
	return true;
}

void gencoins(std::string coin, int len, int n) {
	// generates n jamcoins of length len
	std::stringstream ss0, ss1;
	ss0 << coin;
	ss1 << coin;
	if(len > 1) {
		ss0 << "0";
		ss1 << "1";
		gencoins(ss0.str(), len-1, n);
		gencoins(ss1.str(), len-1, n);
	}
	else {
		ss0 << "01";
		ss1 << "11";
		std::cout << ss0.str() << std::endl;
		std::cout << ss1.str() << std::endl;
	}
}

int main() {
	int n;
	std::cin >> n;
	int i = 0;
	while(i < n) {
		int length, jamcoins, total = 0;
		std::cin >> length >> jamcoins;
		std::cout << "Case #" << i+1 << ":\n";
		gencoins("1", length-2, jamcoins);
		++i;
	}
	return 0;
}
