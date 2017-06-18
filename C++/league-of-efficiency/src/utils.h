#ifndef UTILS_H
#define UTILS_H

#include <ostream>
#include <fstream>
namespace Utils {

	/**
	 * Sorting function for descending order.
	 */
	struct greater {
		template<class T>
		bool operator()(T const &a, T const &b) const { return a > b; }
	};

	int validateInput(const float, const float, const float, std::ostream &);
	std::string getData(const std::string, const std::string, std::ostream &);
	std::string readFile(std::ifstream &);
}

#endif
