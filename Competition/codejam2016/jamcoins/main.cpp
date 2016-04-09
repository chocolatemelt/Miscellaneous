#include <iostream>
#include <vector>
#include <sstream>
#include <string>
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

std::string base10(int num, int base) {
	// base change 10 to <base> adapted from sanfoundry
	std::stringstream ss;
	if (num == 0) return ""; // never going to be 0 anyway
	int x = num % base;
	num /= base;
	if (x < 0) num += 1; 
	ss << base10(num, base);
	ss << x < 0 ? x + (base * -1) : x;
	return ss.str();
}

std::string basechange(std::string num, int base) {
	// base change 2 to <base>

}

void verify(std::string s) {
	// verifies a jamcoin
	for(int i = 2; i <= 10; ++i) {
	}
}

int main() {
	int n;
	std::cin >> n;
	int i = 0;
	while(i < n) {
		int length, jamcoins;
		std::cin >> length >> jamcoins;
		std::cout << "Base10: " << std::stoi(myNumberString, 0, 2) << '\n';
		++i;
	}
	return 0;
}