#include <iostream>
#include <mpi.h>

enum TOKEN
{
	TOKEN_SIZE = 9,
	MAX_TOKEN_VALUE = 999999999
};

enum { ROOT = 0 };

enum Status_t
{
	OK   = 0,
	FAIL = 1
};

struct Numbers{
	int* first;
	int* second;
	Numbers() {}
	Numbers(int size)
	{
		first  = new int[size];
		second = new int[size];
	}
};

int ReadNumbers(char* in, int size, Numbers** numbers_);
	
int main(int argc, char* argv[])
{
	Numbers* numbers(nullptr);
	int error = MPI_Init(&argc, &argv);
  	if(error)
		std::cerr << "MPI_Init crashed" << std::endl;

	Status_t arg_status(FAIL);

	int size(0), rank(0);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == ROOT) // Root
	{
		std::cout << "rank == 0!" << std::endl;
		if(argc != 3)
		{
			arg_status = FAIL;
			MPI_Bcast(&arg_status, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Finalize();
			return EXIT_FAILURE;
		}
		arg_status = OK;
		MPI_Bcast(&arg_status, 1, MPI_INT, 0, MPI_COMM_WORLD);

		ReadNumbers(argv[1], size, &numbers);
    	// RootRoutine(size, status, numbers, token_number);
	}
	else // Worker
	{
		MPI_Bcast(&arg_status, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if(arg_status == FAIL)
		{
			MPI_Finalize();
			return EXIT_FAILURE;
		}
	}

	MPI_Finalize();
	return 0;
}

int ReadNumbers(char* in, int size, Numbers** numbers_)
{
	return 0;
}