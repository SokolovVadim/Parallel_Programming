#include <iostream>
#include <mpi.h>


int main(int argc, char* argv[])
{
  int error = MPI_Init(&argc, &argv);
  if(error){
    std::cout << "MPI_Init crashed" << std::endl;
  }
  int rank = 0;
  int size = 0;

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(rank == 0)
  	std::cout << size << std::endl;
  MPI_Finalize();
  return 0;
}
