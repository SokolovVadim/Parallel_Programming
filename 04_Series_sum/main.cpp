#include <iostream>
#include <mpi.h>
#include <stdexcept>
#include <climits>
#include <cmath>

enum { MULTYPLIER = 2 << 20 };

long int ReadArg        (char * str);
double   CalculateSeries(long int length);

// -----------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int proc_num(0), proc_rank(0);

	try{

	// MPI code starts here

	int error = MPI_Init(&argc, &argv);
	if(error)
		std::cerr << "MPI_Init crashed" << std::endl;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

	long int length(0);
	if(proc_rank == 0)
	{
		if(argc != 2)
			throw std::runtime_error("Invalid number of arguments!");
		length = ReadArg(argv[1]);
	}

	MPI_Bcast(&length, 1, MPI::LONG, 0, MPI_COMM_WORLD);

	// start time measurement
	double start(0.0), end(0.0);
	MPI_Barrier(MPI_COMM_WORLD);
	if(proc_rank == 0)
		start = MPI_Wtime();
	
	double sum = CalculateSeries(length);

	double total_sum(0.0);
	MPI_Reduce(&sum, &total_sum, 1, MPI::DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	// end time measurement
	MPI_Barrier(MPI_COMM_WORLD);

	if(proc_rank == 0)
	{
		total_sum = (total_sum * 6.0) / ( MULTYPLIER * M_PI * M_PI);
		end = MPI_Wtime();
		double time = end - start;
		printf("total_sum = %lf, time = %lf\n", total_sum, time);
	}
	
	MPI_Finalize();

	// MPI code ends here

	} catch(std::runtime_error & err){
		std::cout << err.what() << std::endl;
	}
}

// -----------------------------------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------------------------------

double CalculateSeries(long int length)
{
	double sum(0.0);
	int proc_rank(0), proc_num(0);

	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	
	long int interval = length / proc_num;
	long int interval_begin = interval * proc_rank + 1;
	long int interval_end = interval_begin + interval;

	// MULTYPLIER is used to reduce calculating error when length is very big

	// calcule local sum on interval [interval_begin; interval_end]
	for(int i = interval_end - 1; i >= interval_begin; --i)
		sum += MULTYPLIER / (double(i) * double(i));
	// calculate the rest of interval that is determined by length % proc_num
	if(proc_rank == proc_num - 1)
	{
		for(int j = length; j >= interval_end; --j)
			sum += MULTYPLIER / (double(j) * double(j));
	}
	return sum;
}
