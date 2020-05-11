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
  status_t status = FAIL;
  int tasksNumber = 0;
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
    if (argc != 2 || (tasksNumber = atoi(argv[1])) < 1)
    {
      printf("Usage:\n required 1 argument (> 0) for tasks number\n");
      // Exit with fail status
      status = FAIL;
      MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Finalize();
      return 0;
    }

    // Send status of arguments
    status = OK;
    MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Prepare slave's tasks
    int localLen = tasksNumber / (size - 1);
    int rest = tasksNumber % (size - 1);
    task_t* taskShedule = malloc(size * sizeof(task_t));

    int taskPointer = 0;
    printf("Shedule : ");
    for (i = 1; i < size; i++)
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
  }
  else
  {
    // Slave branch
    //
    // Check status
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
    startTime = MPI_Wtime();
    
    for (i = localTask.start; i < localTask.end; i++)
    {
      sleep(rank);
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

