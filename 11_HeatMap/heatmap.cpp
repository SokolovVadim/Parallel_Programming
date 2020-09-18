#include <iostream>
#include <string>
#include <omp.h>
#include <sys/time.h>
#include <climits>

enum NUMBERS
{
	CORE_NUM = 4,
	THREAD_NUM = 8,
	INTERNAL_THREAD_NUM = 16,
	FOR_MAX = 100,
	COUNTER_MAX = 10000
};

long int ReadArg(char * str);
void create_threads(int thread_num);
void process_routine();

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		std::cout << "Program requires two input parameters!\n";
		exit(EXIT_FAILURE);
	}
	int external_thread_num = ReadArg(argv[1]);
	int internal_thread_num = ReadArg(argv[2]);
	omp_set_nested(true);
	omp_set_num_threads(THREAD_NUM);

	double start = omp_get_wtime();

	// Parallel code starts here

	#pragma omp parallel num_threads(external_thread_num)
	{
		for(int i(0); i < FOR_MAX; ++i)
		{
			// std::string msg = "External process " + std::to_string(omp_get_thread_num()) + "\n";
			create_threads(internal_thread_num);
			// std::cout << msg;
		}
	}

	// Parallel code ends here

	double end = omp_get_wtime();

	double delta = end - start;
	std::cout << delta << std::endl;

	return 0;
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

void process_routine()
{
	int counter(0);
	for(int i(0); i < COUNTER_MAX; ++i)
		counter++;
}


long int ReadArg(char * str)
{
	char* endptr;
	errno = 0;

	long int number = strtol(str, &endptr, 10);

	
	if ((errno == ERANGE && (number == LONG_MAX || number == LONG_MIN)) || (errno != 0 && number == 0)) 
	{
       		perror("strtol");
        	exit(EXIT_FAILURE);
   	}

	if (endptr == str)
	{
        	fprintf(stderr, "Error!\n");
        	exit(EXIT_FAILURE);
   	}
	if (*endptr != '\0')
	{
        	fprintf(stderr, "Error!\n");
        	exit(EXIT_FAILURE);
   	}

	return number;
}