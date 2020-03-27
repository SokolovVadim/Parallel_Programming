#include <iostream>
#include <mpi.h>
#include <chrono>


/*time_start = timer();
for (i=0 ; i < iterations; i++) {
  collective(msg_size);
  barrier();
}
time_end = timer();
time = (time_end - time_start) / iterations;*/

/*
//C++11 Style:***
#include <chrono>

std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;*/

/*
std::cout << "Time difference (sec) = " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0 <<std::endl;*/

enum MEASUREMENT{
	ITERATIONS = 10000
};

int main(int argc, char* argv[])
{
	std::cout << "Hello!\n";
	// std::cout << "argc = " << argc << std::endl;

	int ProcNum(0), ProcRank(0); //, RecvRank(0);
	// MPI_Status Status = {};
	// MPI code starts here
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for(int i(0); i < ITERATIONS; ++i)
	{
		for(int j(0); j < CLOCKS_PER_SEC; ++j)
		{
			volatile int p = 1000 / 12 + 4;
			p++;
		}
		// collective;
		// MPI_Barrier();
		MPI_Barrier(MPI_COMM_WORLD);
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	// std::cout << "Time difference (sec) = " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0 <<std::endl;
	double time_difference = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0;
	std::cout << "Time difference (sec) = " << time_difference << std::endl;
	double time_difference_by_oper = time_difference / ITERATIONS;
	std::cout << "Time difference by operation (sec) = " << time_difference_by_oper << std::endl;

	MPI_Finalize();
	// MPI code ends here
}
