/*
 * Personal FizzBuzz in C++.
 */
#include <iostream>

int main() {
	bool fb = false;
    for(int i = 1; i < 100; i++) {
        if(i % 3 == 0) {
        	std::cout << "Fizz";
        	fb = true;
        }
        if(i % 5 == 0) {
        	std::cout << "Buzz";
        	fb = true;
        }
        if(!fb) {
        	std::cout << i;
        }
        std::cout << std::endl;
        fb = false;
    }
    return 0;
}
