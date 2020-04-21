#include <iostream>
#include <mpi.h>
#include <stdexcept>
#include <climits>
#include <assert.h>

long int ParceInput(int argc, char* argv[]);
long int ReadArg(char * str);
double CalculateSeries(long int length);

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
	if(proc_rank == 0)
		std::cout << "Number of processes = " << proc_num << std::endl;

	long int length(0);
	if(proc_rank == 0)
	{
		length = ParceInput(argc, argv);
		std::cout << length << std::endl;
	}


	MPI_Bcast(&length, 1, MPI::LONG, 0, MPI_COMM_WORLD);

	// start time measurement
	double start(0.0), end(0.0);
	MPI_Barrier(MPI_COMM_WORLD);
	if(proc_rank == 0)
		start = MPI_Wtime();
	
	double sum = CalculateSeries(length);
	// printf("sum[%d] = %lf\n", proc_rank, sum);

	

	double total_sum(0.0);
	MPI_Reduce(&sum, &total_sum, 1, MPI::DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	// end time measurement
	MPI_Barrier(MPI_COMM_WORLD);

	if(proc_rank == 0)
	{
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

long int ParceInput(int argc, char* argv[])
{
	if(argc != 2)
		throw std::runtime_error("Invalid number of arguments!");
	long int length = ReadArg(argv[1]);
	return length;
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


double CalculateSeries(long int length)
{
	double sum(0.0);
	int proc_rank(0), proc_num(0);

	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	
	long int interval = length / proc_num;
	long int interval_begin = interval * proc_rank + 1;
	long int interval_end = interval_begin + interval;

	// printf("proc[%d]: %ld, %ld\n", proc_rank, interval_begin, interval_end);

	int i(0);
	for(i = interval_begin; i < interval_end; ++i)
		sum += i;
	if(proc_rank == proc_num - 1)
	{
		for(; i <= length; ++i)
			sum += i;
	}

	return sum;
}
