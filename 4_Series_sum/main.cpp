#include <iostream>
#include <mpi.h>


void ParceInput(int argc, char* argv[]);

// -----------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int proc_num(0), proc_rank(0);

	ParceInput(argc, argv);

	// MPI code starts here

	int error = MPI_Init(&argc, &argv);
	if(error)
		std::cerr << "MPI_Init crashed" << std::endl;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	if(proc_rank == 0)
		std::cout << "Number of processes = " << proc_num << std::endl;


	
	
	MPI_Finalize();

	// MPI code ends here
}

// -----------------------------------------------------------------------------------------------------

void ParceInput(int argc, char* argv[])
{
	
}
