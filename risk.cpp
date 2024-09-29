#include <chrono>
#include <cstdlib>
#include <iostream> // for now; obviously, we're using unreal engine
#include "risk.h"

int main(int argc, char *argv[]) {
	// set the seed for randomness based off of time
	std::srand(std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now().time_since_epoch()).count());
	
	return 0;
}
