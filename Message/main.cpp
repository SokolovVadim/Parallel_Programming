#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[])
{
	std::cout << "Hello!\n";
	std::cout << "argc = " << argc << std::endl;

	int ProcNum(0), ProcRank(0), RecvRank(0);
	MPI_Status Status = {};
	// MPI code starts here
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	// std::cout << "ProcNum = " << ProcNum << std::endl;
	// fflush(stdout);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	// std::cout << "ProcRank = " << ProcRank << std::endl;
	// fflush(stdout);

	// Process 0 starts conversation via sending msg to Process 1
	if(ProcRank == 0)
	{	
		std::cout << "ProcRank: " << ProcRank << std::endl;
		fflush(stdout); // push data from buffer to stdout
		MPI_Send(&RecvRank, 1, MPI_INT, ProcRank + 1, 0, MPI_COMM_WORLD);
	}
	else{
		// Process n > 0 receives msg from Process n - 1
		MPI_Recv(&RecvRank, 1, MPI_INT, ProcRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
		// Process n prints its rang
		std::cout << "ProcRank: " << ProcRank << std::endl;
		fflush(stdout);
		// Process n != N - 1 sends msg to Process n + 1
		// But actually it's possible to finalize the msg cicle to Process 0
		if(ProcRank < ProcNum - 1)
			MPI_Send(&RecvRank, 1, MPI_INT, ProcRank + 1, 0, MPI_COMM_WORLD);
	}



	MPI_Finalize();
	// MPI code ends here
}

