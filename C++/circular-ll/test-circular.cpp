#include <iostream>
#include "circular_ll.h"

int main() {
	// initialize circular ll
	circular_ll *list = new circular_ll;
	list->print(std::cout);

	// initial insertion test
	list->insert(0);
	list->print(std::cout);

	// initial test
	delete list;
	list = new circular_ll;
	std::cout << "\ntest case 1\n";
	for(int i = 1; i < 11; i += 2) list->insert(i);
	list->print(std::cout);
	std::cout << "insert 6\n";
	list->insert(6);
	list->print(std::cout);
	std::cout << "insert 10\n";
	list->insert(10);
	list->print(std::cout);
	std::cout << "insert 0\n";
	list->insert(0);
	list->print(std::cout);

	// another test
	delete list;
	list = new circular_ll;
	std::cout << "\ntest case 2\n";
	for(int i = 0; i < 3; ++i) list->insert(5);
	list->print(std::cout);
	std::cout << "insert 6\n";
	list->insert(6);
	list->print(std::cout);

	// finish and cleanup
	delete list;
	return 0;
}
