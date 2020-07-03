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

int ReadNumbers(char* in, int size, Numbers** numbers_);
void RootRoutine(int size, Numbers* numbers, int token_number);
void ClientRoutine();
	
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

		int token_number = ReadNumbers(argv[1], size, &numbers);
    	RootRoutine(size, numbers, token_number);
    	delete[] numbers;
	}
	else // Worker
	{
		MPI_Bcast(&arg_status, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if(arg_status == FAIL)
		{
			MPI_Finalize();
			return EXIT_FAILURE;
		}
		ClientRoutine();
	}

	MPI_Finalize();
	return 0;
}

void RootRoutine(int size, Numbers* numbers, int token_number)
{
	
	for(int i(1); i < token_number; ++i)
	{
		// MPI_Send(&token_number, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    	MPI_Send(&numbers[i - 1].first,  1, MPI_INT, i, 0, MPI_COMM_WORLD);
    	MPI_Send(&numbers[i - 1].second, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    	// MPI_Send(numbers[i - 1].digit_transfer, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}

}

void ClientRoutine()
{
	MPI_Status mpi_status = {};
	int first(0), second(0);//, digit_transfer(0);
	MPI_Recv(&first, 1,  MPI_INT, ROOT, 0, MPI_COMM_WORLD, &mpi_status);
	MPI_Recv(&second, 1,  MPI_INT, ROOT, 0, MPI_COMM_WORLD, &mpi_status);
	//MPI_Recv(digit_transfer, 1,  MPI_INT, ROOT, 0, MPI_COMM_WORLD, &mpi_status);
}

int ReadNumbers(char* in, int size, Numbers** numbers_)
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

	*numbers_ = numbers;

	return num_length / TOKEN_SIZE;
}