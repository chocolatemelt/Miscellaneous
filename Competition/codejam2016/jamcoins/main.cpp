#include <iostream>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>

std::vector<long long> SieveOfEratosthenes(long long n) {
	// geekforgeeks sieve function modified for this problem
	std::vector<long long> ret;
	bool *prime = (bool*)malloc((n+1)*sizeof(bool));
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
	return ret;
}

long long basechange(std::string num, int base) {
	// quick wrapper
	return std::stoi(num, 0, base);
}

bool verify(std::string s) {
	// verifies a jamcoin
	for(int i = 2; i <= 10; ++i) {
		long long chk = basechange(s, i);
		std::vector<long long> lst = SieveOfEratosthenes(chk);
		if(lst.empty()) {
			return false;
		}
	}
	return true;
}

void print(std::string s) {
	// print jamcoin
	std::cout << s;
	for(int i = 2; i <= 10; ++i) {
		long long chk = basechange(s, i);
		std::vector<long long> lst = SieveOfEratosthenes(chk);
		std::cout << " " << lst[0];
	}
	std::cout << std::endl;
}

void gencoins(std::string coin, int len, int n) {
	// generates n jamcoins of length len
	std::stringstream ss0, ss1;
	ss0 << coin;
	ss1 << coin;
	if(len > 1) {
		// generate jamcoin
		ss0 << "0";
		ss1 << "1";
		gencoins(ss0.str(), len-1, n);
		gencoins(ss1.str(), len-1, n);
	}
	else {
		// check generated jamcoin
		ss0 << "01";
		ss1 << "11";
		if(verify(ss0.str())) print(ss0.str());
		else std::cout << ss0.str() << std::endl;
		if(verify(ss1.str())) print(ss1.str());
		else std::cout << ss1.str() << std::endl;
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
