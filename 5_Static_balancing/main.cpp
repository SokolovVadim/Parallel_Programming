#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <malloc.h>
#include <climits>
#include <fstream>

typedef enum {
  OK = 0,
  FAIL = 1
} status_t;

typedef struct {
  int start;
  int end;
} task_t;

long int ReadArg(char * str);
void* RootRoutine(int size, int tasksNumber, status_t status, task_t & localTask, double & startTime);
int SlaveRoutine(int rank);

int main(int argc, char* argv[])
{

  status_t status = FAIL;
  int tasksNumber = 0;
  task_t localTask = {};
  int rank = 0;
  int size = 0;
  double startTime = 0;
  double endTime = 0;
  task_t* taskShedule(nullptr);
  std::ifstream fin;
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
    if (argc != 4 || (tasksNumber = ReadArg(argv[1])) < 1)
    {
      printf("Usage:\n required 1 argument (> 0) for tasks number\n");
      // Exit with fail status
      status = FAIL;
      MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Finalize();
      return 0;
    }
    fin.open(argv[2]);
    RootRoutine(size, tasksNumber, status, localTask, startTime);
  }
  else
  {
  	SlaveRoutine(rank);
  }


  // Finish
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0)
  {
    endTime = MPI_Wtime();
    printf("[TIME RES] %lf\n", endTime - startTime);
    fin.close();
    delete[] taskShedule;
  }

  MPI_Finalize();
  return 0;
}

void* RootRoutine(int size, int tasksNumber, status_t status, task_t & localTask, double & startTime)
{
	// Root branch
    // Send status of arguments
    status = OK;
    MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);

   
    // Prepare slave's tasks
    int localLen = tasksNumber / (size - 1);
    int rest = tasksNumber % (size - 1);
  
    task_t* taskShedule = new task_t[size];
    if(taskShedule == nullptr)
    {
    	std::cout << "memory allocation failed!" << std::endl;
    	return NULL;
    }

    int taskPointer = 0;
    printf("Shedule : ");

    for (int i = 1; i < size; i++)
    {
      taskShedule[i].start = taskPointer;
      taskPointer += localLen;
      if (i < (rest + 1))
      {
        taskPointer++;
      }
      taskShedule[i].end = taskPointer;
      printf("[%d] %d(%d..%d) ", i, taskShedule[i].end - taskShedule[i].start\
                                ,taskShedule[i].start, taskShedule[i].end - 1);
    }
    printf("\n");

    // Send tasks for work begin
    startTime = MPI_Wtime();
    MPI_Scatter(taskShedule, 2, MPI_INT, &localTask, 2, MPI_INT, 0, MPI_COMM_WORLD);
    return taskShedule;
}

int SlaveRoutine(int rank)
{
	status_t status = FAIL;
	task_t localTask = {};
	MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (status == FAIL)
    {
      MPI_Finalize();
      return 0;
    }
    
    // Get tasks
    MPI_Scatter(NULL, 0, 0, &localTask, 2, MPI_INT, 0, MPI_COMM_WORLD);
    //printf("[%d] Get %d..%d\n", rank, localTask.start, localTask.end - 1);

    // Calc task
    double startTime = MPI_Wtime();
    
    for (int i = localTask.start; i < localTask.end; i++)
    {
      sleep(rank);
    }
    double endTime = MPI_Wtime();
    printf("[TIME %d] %lf\n", rank, endTime - startTime);
    return 0;
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