#include <iostream>
#include <fstream>
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
	int first;
	int second;
	int digit_transfer;
	Numbers():
		first(0),
		second(0),
		digit_transfer(0)
	{}
};

int ReadNumbers(char* in, int size);
	
int main(int argc, char* argv[])
{
	// Numbers* numbers(nullptr);
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

		ReadNumbers(argv[1], size);
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

int ReadNumbers(char* in, int size)
{
	// Open the stream and read size of number
	// and two lines with two numbers
	std::ifstream fin(in);
	int num_length(0);
	std::string s_first, s_second;

	fin >> num_length >> s_first >> s_second;

	fin.close();

	// Allocate memory for num_length tokens

	Numbers* numbers = new Numbers[num_length];

	for(int i(0); i < num_length / TOKEN_SIZE; i++)
	{
		std::string s_token = s_first.substr(i * TOKEN_SIZE, TOKEN_SIZE);
		numbers[i].first = std::stoi(s_token);

		s_token = s_second.substr(i * TOKEN_SIZE, TOKEN_SIZE);
		numbers[i].second = std::stoi(s_token);
	}

	for(int i(0); i < num_length / TOKEN_SIZE; ++i)
	{
		std::cout << numbers[i].first << " " << numbers[i].second << std::endl;
	}

	return 0;
}