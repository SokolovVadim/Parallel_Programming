#include <iostream>
#include <string>
#include <omp.h>

enum NUMBERS
{
	CORE_NUM = 4,
	THREAD_NUM = 8,
	CUR_THREAD_NUM = 16
};

int main()
{
	std::cout << "Hello!\n";

	omp_set_num_threads(THREAD_NUM);

	// Parallel code starts here

	#pragma omp parallel num_threads(CUR_THREAD_NUM)
	{
		std::string msg = "Hello from process ";
		msg += std::to_string(omp_get_thread_num()) + "\n";
		std::cout << msg;
	}

	return 0;
}
