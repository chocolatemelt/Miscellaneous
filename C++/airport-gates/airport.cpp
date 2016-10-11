#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

void split(const std::string &s, char delim, std::vector<int> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(stoi(item));
	}
}

int gateCounter(int flights, std::string arrivals, std::string departures) {
	std::vector<int> arrive;
	std::vector<int> departure;
	split(arrivals, ',', arrive);
	split(departures, ',', departure);
	return 0;
}

int main() {
	std::ifstream inf("airport.txt");
	int flights;
	std::string input1, input2;
	while(inf >> flights >> input1 >> input2) {
		std::cout << gateCounter(flights, input1, input2) << std::endl;
	}
	return 0;
}
