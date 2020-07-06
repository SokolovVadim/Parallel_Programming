#include <iostream>
#include <fstream>
#include <unistd.h>
#include <mpi.h>

enum TOKEN
{
	TOKEN_SIZE = 9,
	MAX_TOKEN_VALUE = 999999999,
	DIGIT_TRANSFER = 1,
	NO_DIGIT_TRANSFER = 0
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
	int sum;
	int speculative_sum;
	int digit_transfer;

	Numbers():
		first(0),
		second(0),
		sum(0),
		speculative_sum(0),
		digit_transfer(NO_DIGIT_TRANSFER)
	{}
};

int ReadNumbers(char* in, int size, Numbers** numbers_);
void RootRoutine(int size, Numbers* numbers, int token_number, MPI_Datatype& mpi_numbers_type);
void ClientRoutine(int rank, MPI_Datatype& mpi_numbers_type);
	
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

	// Create a type for struct Numbers

	int nitems = 5;
	int blocklength[5] = {1, 1, 1, 1, 1};
	MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
	MPI_Datatype mpi_numbers_type;
	MPI_Aint offsets[5];
	offsets[0] = offsetof(Numbers, first);
	offsets[1] = offsetof(Numbers, second);
	offsets[2] = offsetof(Numbers, sum);
	offsets[3] = offsetof(Numbers, speculative_sum);
	offsets[4] = offsetof(Numbers, digit_transfer);

	MPI_Type_create_struct(nitems, blocklength, offsets, types, &mpi_numbers_type);
	MPI_Type_commit(&mpi_numbers_type);

	if(rank == ROOT) // Root
	{
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
    	RootRoutine(size, numbers, token_number, mpi_numbers_type);
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
		ClientRoutine(rank, mpi_numbers_type);
	}
	MPI_Type_free(&mpi_numbers_type);
	MPI_Finalize();
	return 0;
}

// fill the Numbers with sum and digit_transfer fields

int Calculate_sum(Numbers& numbers)
{
	int sum = numbers.first + numbers.second;
	if(sum == MAX_TOKEN_VALUE)
	{
		numbers.sum = sum;
		numbers.speculative_sum = 0;
		numbers.digit_transfer = DIGIT_TRANSFER;
	} else if(sum > MAX_TOKEN_VALUE)
	{
		sum -= (MAX_TOKEN_VALUE + 1);
		numbers.sum = sum;
		numbers.digit_transfer = DIGIT_TRANSFER;
	} else{
		numbers.sum = sum;
	}
	return sum;
}

void print_numbers(Numbers* numbers, int token_number)
{
	for(int i(0); i < token_number; ++i)
	{
		std::cout << numbers[i].sum << " " << numbers[i].digit_transfer << std::endl;
	}
}

void process_result(Numbers* numbers, int token_number)
{
	
}

void RootRoutine(int size, Numbers* numbers, int token_number, MPI_Datatype& mpi_numbers_type)
{
	int number_of_iterations = token_number / (size - 1);
	MPI_Status mpi_status = {};
	// std::cout << "token_number = " << token_number << " size = " << size << std::endl;
	int i(0);
	for(i = 1; i < size; ++i)
	{
		MPI_Send(&number_of_iterations,  1, MPI_INT, i, OK, MPI_COMM_WORLD);
	}
	int array_index(token_number - 1);
	while(number_of_iterations >= 1)
	{
		for(i = size - 1; i > 0; i--)
		{
			// Send two numbers to process i and return the sum of them
	
	    	MPI_Send(&numbers[array_index], 1, mpi_numbers_type, i, OK, MPI_COMM_WORLD);
	    	MPI_Recv(&numbers[array_index], 1, mpi_numbers_type, i, 0, MPI_COMM_WORLD, &mpi_status);
	    	std::cout << "sum = " << numbers[array_index].sum << std::endl;

	    	array_index--;
		}
    	number_of_iterations--;
	}
	std::cout << "SUM:\n";
	print_numbers(numbers, token_number);
	process_result(numbers, token_number);
}

void ClientRoutine(int rank, MPI_Datatype& mpi_numbers_type)
{
	MPI_Status mpi_status = {};
	int number_of_iterations(0);
	MPI_Recv(&number_of_iterations, 1,  MPI_INT, ROOT, 0, MPI_COMM_WORLD, &mpi_status);
	while(number_of_iterations != 0)
	{
		Numbers numbers;
		MPI_Recv(&numbers, 1, mpi_numbers_type, ROOT, 0, MPI_COMM_WORLD, &mpi_status);
		Calculate_sum(numbers);
		
		MPI_Send(&numbers, 1, mpi_numbers_type, ROOT, OK, MPI_COMM_WORLD);
		number_of_iterations--;
	}
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