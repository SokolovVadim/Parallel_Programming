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

	if(ProcRank == 0)
	{	//MPI_Send(&ProcRank, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
		/*for(int i(0); i < ProcNum; ++i)
		{
			// std::cout << "I'm here!\n";
			std::cout << "ProcRank: " << ProcRank << std::endl;
			MPI_Send(&RecvRank, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Recv(&RecvRank, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
			std::cout << "I'm here!\n";
			fflush(stdout);
		}*/
		std::cout << "ProcRank: " << ProcRank << std::endl;
		fflush(stdout);
		MPI_Send(&RecvRank, 1, MPI_INT, ProcRank + 1, 0, MPI_COMM_WORLD);
	}
	else{
		MPI_Recv(&RecvRank, 1, MPI_INT, ProcRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
		std::cout << "ProcRank: " << ProcRank << std::endl;
		fflush(stdout);
		if(ProcRank < ProcNum - 1)
			MPI_Send(&RecvRank, 1, MPI_INT, ProcRank + 1, 0, MPI_COMM_WORLD);
	}



	MPI_Finalize();
	// MPI code ends here
}

