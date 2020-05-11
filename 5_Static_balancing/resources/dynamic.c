#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <malloc.h>

typedef enum {
  OK = 0,
  FAIL = 1
} status_t;

typedef struct {
  int start;
  int end;
} task_t;

int main(int argc, char* argv[])
{
  status_t argStatus = FAIL;
  MPI_Status status = {};
  int tasksNumber = 0;
  int blockSize = 0;
  task_t localTask = {};
  int rank = 0;
  int size = 0;
  double startTime = 0;
  double endTime = 0;
  int i = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    // Root branch
    //
    // Check arguments
    if (argc != 3 || (tasksNumber = atoi(argv[1])) < 1\
        || (blockSize = atoi(argv[2])) < 1)
    {
      printf("Usage:\n required 2 arguments (> 0)\
              \n for tasks number and block size\n");
      // Exit with fail status
      argStatus = FAIL;
      MPI_Bcast(&argStatus, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Finalize();
      return 0;
    }

    // Send status of arguments
    argStatus = OK;
    MPI_Bcast(&argStatus, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Prepare slave's tasks
    int blocksNumber = tasksNumber / blockSize;
    int rest = tasksNumber % blockSize;
    printf("Shedule : %d blocks(%d)", blocksNumber, blockSize);
    if (rest != 0)
    {
      blocksNumber++;
      printf(", 1 block(%d)", rest);
    } else {
      // size of first block
      rest = blockSize;
    }
    printf("\n");

    task_t* taskShedule = malloc(blocksNumber * sizeof(task_t));
    
    // Add first block
    taskShedule[0].start = 0;
    taskShedule[0].end = rest;
    
    // Add other blocks
    int taskPtr = rest;
    for (i = 1; i < blocksNumber; i++)
    {
      taskShedule[i].start = taskPtr;
      taskPtr += blockSize;
      taskShedule[i].end = taskPtr;
    }

    // Send blockss for work begin
    int blockPtr = 0;
    int buf = 0;
    int slave = 0;
    startTime = MPI_Wtime();

    for (blockPtr = 0; blockPtr < blocksNumber; blockPtr++)
    {
      // Find slave
      MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      slave = status.MPI_SOURCE;
      // Send block
      MPI_Send(&taskShedule[blockPtr], 2, MPI_INT, slave, OK, MPI_COMM_WORLD);
    }

    // End calc
    for (i = 1; i < size; i++)
    {
      MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      slave = status.MPI_SOURCE;
      MPI_Send(taskShedule, 2, MPI_INT, slave, FAIL, MPI_COMM_WORLD);
    }      

  }
  else
  {
    // Slave branch
    //
    // Check status
    MPI_Bcast(&argStatus, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (argStatus == FAIL)
    {
      MPI_Finalize();
      return 0;
    }
    

    // Calc loop
    startTime = MPI_Wtime();
    
    while (1)
    {
      // Ready for tasks
      MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      // Get block
      MPI_Recv(&localTask, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      if (status.MPI_TAG == OK)
      {
        printf("[%d] Get %d..%d\n", rank, localTask.start, localTask.end - 1);
        // Calc block
        for (i = localTask.start; i < localTask.end; i++)
        {
          sleep(rank);
        }
      } else {
        break;
      }
    }
    endTime = MPI_Wtime();
    printf("[TIME %d] %lf\n", rank, endTime - startTime);
  
  }

  // Finish
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0)
  {
    endTime = MPI_Wtime();
    printf("[TIME RES] %lf\n", endTime - startTime);
  }

  MPI_Finalize();
  return 0;
}

