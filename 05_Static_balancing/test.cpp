#include <iostream>
#include <mpi.h>
#include <unistd.h>


int main(int argc, char* argv[])
{
  int error = MPI_Init(&argc, &argv);
  if(error){
    std::cout << "MPI_Init crashed" << std::endl;
  }
  int rank = 0;
  int size = 0;

  error = MPI_Comm_size(MPI_COMM_WORLD, &size);
  if(error != 0)
  {
    perror("MPI_Comm_size");
    exit(EXIT_FAILURE);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(rank == 0){
  	std::cout << size << std::endl;
    printf("pid = %d, parent_id = %d\n", getpid(), getppid());
  }
  MPI_Finalize();
  return 0;
}
