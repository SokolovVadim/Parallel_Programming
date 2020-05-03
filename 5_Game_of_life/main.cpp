#include <iostream>
#include <mpi.h>
#include <stdexcept>
#include <climits>
#include <cmath>

// -----------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int proc_num(0), proc_rank(0);

	try{

	// MPI code starts here

	int error = MPI_Init(&argc, &argv);
	if(error)
		std::cerr << "MPI_Init crashed" << std::endl;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

	
	
	MPI_Finalize();

	// MPI code ends here

	} catch(std::runtime_error & err){
		std::cout << err.what() << std::endl;
	}
}

