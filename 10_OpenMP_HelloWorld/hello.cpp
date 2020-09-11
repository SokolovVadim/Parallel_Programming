#include <iostream>
#include <string>
#include <omp.h>

int main()
{
	std::cout << "Hello!\n";

	// Parallel code starts here

	#pragma omp parallel
	{
		std::string msg = "Hello from process ";
		msg += std::to_string(omp_get_thread_num()) + "\n";
		std::cout << msg;
	}

	return 0;
}
