#include <iostream>
#include <fstream>
#include <unistd.h>
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
void ClientRoutine(int rank);
	
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
		ClientRoutine(rank);
	}

	MPI_Finalize();
	return 0;
}

void RootRoutine(int size, Numbers* numbers, int token_number)
{
	//int process_worked(0);
	int number_of_iterations = token_number / (size - 1);
	std::cout << "token_number = " << token_number << " size = " << size << std::endl;
	int i(0);
	for(i = 1; i < size; ++i)
	{
		MPI_Send(&number_of_iterations,  1, MPI_INT, i, OK, MPI_COMM_WORLD);
		MPI_Send(&numbers[i - 1].first,  1, MPI_INT, i, OK, MPI_COMM_WORLD);
    	MPI_Send(&numbers[i - 1].second, 1, MPI_INT, i, OK, MPI_COMM_WORLD);
	}
	while(number_of_iterations > 1)
	{
		for(i = 1; i < size; ++i)
		{
			std::cout << "process " << i << " started it's job\n";
			MPI_Send(&number_of_iterations,  1, MPI_INT, i, OK, MPI_COMM_WORLD);
			MPI_Send(&numbers[i - 1].first,  1, MPI_INT, i, OK, MPI_COMM_WORLD);
	    	MPI_Send(&numbers[i - 1].second, 1, MPI_INT, i, OK, MPI_COMM_WORLD);
		}
    	number_of_iterations--;
	}
	/*int status(0);
	do
	{
		status = (process_worked + 1 >= size ? FAIL: OK);
		std::cout << "process " << process_worked + 1 << " started it's job with status "
		 << status << " pw + 1 % size = " << (process_worked + 1) % size << std::endl;
		MPI_Send(&numbers[process_worked].first,  1, MPI_INT, (process_worked + 1) % size, status, MPI_COMM_WORLD);
    	MPI_Send(&numbers[process_worked].second, 1, MPI_INT, (process_worked + 1) % size, status, MPI_COMM_WORLD);
    	process_worked++;
	} while(process_worked != token_number);*/
}

void ClientRoutine(int rank)
{
	MPI_Status mpi_status = {};
	int first(0), second(0);//, digit_transfer(0);
	int counter(0);
	int number_of_iterations(0);
	MPI_Recv(&number_of_iterations, 1,  MPI_INT, ROOT, 0, MPI_COMM_WORLD, &mpi_status);
	while(number_of_iterations != 0)
	{
		MPI_Recv(&first, 1,  MPI_INT, ROOT, 0, MPI_COMM_WORLD, &mpi_status);
		MPI_Recv(&second, 1,  MPI_INT, ROOT, 0, MPI_COMM_WORLD, &mpi_status);
		std::cout << "process " << rank << std::endl;
/*		if(mpi_status.MPI_SOURCE == FAIL)
		{
			std::cout << counter;
			break;
		}*/
		counter++;
		number_of_iterations--;
	}
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