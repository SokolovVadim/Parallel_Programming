#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <malloc.h>
#include <climits>
#include <fstream>
#include <string>

// -----------------------------------------------------------------------------------------------------

enum { TOKEN_SIZE = 9 };

typedef enum {
  OK = 0,
  FAIL = 1
} status_t;

typedef struct {
  int start;
  int end;
} task_t;

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

// -----------------------------------------------------------------------------------------------------

long int ReadArg     (char * str);
void*    RootRoutine (int size, status_t status, double & startTime, Numbers* numbers, int token_number);
int      SlaveRoutine(int rank);
int  	 ReadNumbers (char* in, int size, Numbers* numbers);


// -----------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  Numbers* numbers(nullptr);
  status_t status = FAIL;
  //task_t localTask = {};
  int rank = 0;
  int size = 0;
  double startTime = 0;
  // double endTime = 0;

  int error = MPI_Init(&argc, &argv);
  if(error)
	std::cerr << "MPI_Init crashed" << std::endl;

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(rank == 0)
  	std::cout << size << std::endl;
  if (rank == 0)
  {
    // Root branch
    //
    // Check arguments
    if (argc != 3)
    {
      printf("Usage:\n required 1 argument (> 0) for tasks number\n");
      // Exit with fail status
      status = FAIL;
      MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Finalize();
      return 0;
    }
    int token_number = ReadNumbers(argv[1], size, numbers);
    // RootRoutine(size, status, startTime, numbers, token_number);
  }
  else
  {
  	// SlaveRoutine(rank);
  }


  // Finish
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0)
  {
    // endTime = MPI_Wtime();
    // printf("[TIME RES] %lf\n", endTime - startTime);

 /*   int * data = new int[size - 1];
    MPI_Status mpi_status;
    for(int i(1); i < size; i++)
    {
    	MPI_Recv(&data[i - 1], 2, MPI_INT, i, 0, MPI_COMM_WORLD, &mpi_status);
    }
    for(int i(0); i < size - 1; ++i)
    {
    	std::cout << "data[i] = " << data[i] << std::endl;
    }

    delete[] data;*/
 
  }

  MPI_Finalize();
  return 0;
}

// -----------------------------------------------------------------------------------------------------

void* RootRoutine(int size, status_t status, double & startTime, Numbers* numbers, int token_number)
{
	// Root branch
    // Send status of arguments
    status = OK;
    MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for(int i(0); i < size - 1; ++i)
		for(int j(0); j < token_number; ++j)
		{
			std::cout << numbers[i].first[j] << " " << numbers[i].second[j] << std::endl;
		}

    // MPI_Scatter(first, 2, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
	/*int * send_container = new int[2 * token_number];

	for(int i(0), j(0); i < token_number; ++i, j += 2)
	{
		send_container[i] = numbers[i].first[]
	}*/
    for(int i(1); i < size; ++i)
    {
    	MPI_Send(numbers[i].first, token_number, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    

    // Send tasks for work begin
    startTime = MPI_Wtime();
    // MPI_Scatter(taskShedule, 2, MPI_INT, &localTask, 2, MPI_INT, 0, MPI_COMM_WORLD);
    return NULL;//taskShedule;
}

// -----------------------------------------------------------------------------------------------------

int SlaveRoutine(int rank)
{
	status_t status = FAIL;
	
	MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (status == FAIL)
    {
      MPI_Finalize();
      return 0;
    }

    int* numbers = new int[2];
    
    // Get tasks
    // MPI_Scatter(nullptr, 0, 0, numbers, 2, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Status mpi_status;
    MPI_Recv(numbers, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &mpi_status);

    // Calc task
    // double startTime = MPI_Wtime();

    int result = numbers[0] + numbers[1];
    printf("result = %d\n", result);
    
   
    // double endTime = MPI_Wtime();

    // send result to root
    MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    return 0;
}

// -----------------------------------------------------------------------------------------------------

int ReadNumbers(char* in, int size, Numbers* numbers)
{
	std::ifstream fin(in);
	int length(0);
	std::string s_first, s_second;

	fin >> length >> s_first >> s_second;

	fin.close();

	std::cout << length << std::endl << s_first << std::endl << s_second << std::endl;
	int token_size = length / (size - 1);
	int token_number = token_size / TOKEN_SIZE;

	numbers = new Numbers[size - 1];
	for(int i(0); i < size - 1; ++i)
		numbers[i] = Numbers(token_number);

	std::cout << "token_size = " << token_size << " token_num = " << token_number << std::endl;

	for(int i(0); i < size - 1; ++i)
	{
		for(int j(0); j < token_number; ++j)
		{
			std::string s_token = s_first.substr(i * token_size + j * TOKEN_SIZE, TOKEN_SIZE);
			numbers[i].first[j] = std::stoi(s_token);

			s_token = s_second.substr(i * token_size + j * TOKEN_SIZE, TOKEN_SIZE);
			numbers[i].second[j] = std::stoi(s_token);
		}
	}
	/*for(int i(0); i < size - 1; ++i)
		for(int j(0); j < token_number; ++j)
		{
			std::cout << numbers[i].first[j] << " " << numbers[i].second[j] << std::endl;
		}*/
	return token_number;
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