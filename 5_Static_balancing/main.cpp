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

// -----------------------------------------------------------------------------------------------------

long int ReadArg     (char * str);
void*    RootRoutine (int size, status_t status, task_t & localTask, double & startTime, int* first, int* second);
int      SlaveRoutine(int rank);
void  ReadNumbers (char* in, int size, int** first, int* second);


// -----------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  int* first(nullptr);
  int* second(nullptr);
  status_t status = FAIL;
  task_t localTask = {};
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
    ReadNumbers(argv[1], size, &first, second);
    RootRoutine(size, status, localTask, startTime, first, second);
  }
  else
  {
  	SlaveRoutine(rank);
  }


  // Finish
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0)
  {
    // endTime = MPI_Wtime();
    // printf("[TIME RES] %lf\n", endTime - startTime);
    
    delete[] first;
    delete[] second;
  }

  MPI_Finalize();
  return 0;
}

// -----------------------------------------------------------------------------------------------------

void* RootRoutine(int size, status_t status, task_t & localTask, double & startTime, int* first, int* second)
{
	// Root branch
    // Send status of arguments
    status = OK;
    MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
    for(int i(0); i < 2 * (size - 1); ++i)
    	std::cout << first[i] << std::endl;

    // MPI_Scatter(first, 2, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
    for(int i(1), j(0); i < size; ++i, j += 2)
    	MPI_Send(&first[j], 2, MPI_INT, i, 0, MPI_COMM_WORLD);
    

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
    return 0;
}

// -----------------------------------------------------------------------------------------------------

void ReadNumbers(char* in, int size, int** first, int* a_second)
{
	std::ifstream fin(in);
	int length(0);
	std::string s_first, s_second;

	fin >> length >> s_first >> s_second;
	std::cout << length << std::endl << s_first << std::endl << s_second << std::endl;
	int token_size = length / (size - 1);

	int* a_first  = new int[token_size];
	a_second = new int[token_size];

	for(int i(0), j(0); i < size - 1; ++i)
	{
		std::string s_token = s_first.substr(i * token_size, token_size);
		a_first[j]  = std::stoi(s_token);

		s_token = s_second.substr(i * token_size, token_size);
		a_second[i] = std::stoi(s_token);
		a_first[j + 1] = std::stoi(s_token);
		j += 2;
	}
/*	for(int i(0); i < 2 * (size - 1); ++i)
	{
		std::cout << a_first[i] <<  std::endl;
	}*/
	*first = a_first;

	fin.close();
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