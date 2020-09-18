#include <iostream>
#include <string>
#include <omp.h>
#include <sys/time.h>

enum NUMBERS
{
	CORE_NUM = 4,
	THREAD_NUM = 8,
	INTERNAL_THREAD_NUM = 16,
	FOR_MAX = 100
};

void process_routine()
{
	int counter(0);
	for(int i(0); i < FOR_MAX; ++i)
		counter++;
}

void create_threads(int thread_num)
{

	// benchmark code

	#pragma omp parallel num_threads(thread_num)
	{
		/*std::string msg = "Internal process " + std::to_string(omp_get_thread_num()) + "\n";
		std::cout << msg;*/
		process_routine();
	}
}

int main(int argc, char* argv[])
{
	omp_set_nested(true);
	omp_set_num_threads(THREAD_NUM);

	double start = omp_get_wtime();

	// Parallel code starts here

	#pragma omp parallel num_threads(CORE_NUM)
	{
		for(int i(0); i < FOR_MAX; ++i)
		{
			std::string msg = "External process " + std::to_string(omp_get_thread_num()) + "\n";
			create_threads(INTERNAL_THREAD_NUM);
			std::cout << msg;
		}
	}

	// Parallel code ends here

	double end = omp_get_wtime();

	double delta = end - start;
	std::cout << delta << std::endl;

	return 0;
}
