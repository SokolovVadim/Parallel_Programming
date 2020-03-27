#include <iostream>
#include <mpi.h>
#include <chrono>
#include <cmath>


/*time_start = timer();
for (i=0 ; i < iterations; i++) {
  collective(msg_size);
  barrier();
}
time_end = timer();
time = (time_end - time_start) / iterations;*/


enum MEASUREMENT{
	ITERATIONS = 1000000
};

// Measure performance of collective routine including the cost of barrier
double MeasurePerformanceBcast(){

	int BcastRank(0);
	double squares(0.0), diff(0.0), average(0.0);
	
	// double begin = MPI_Wtime();

	for(int i(0); i < ITERATIONS; ++i)
	{
		double begin = MPI_Wtime();

		MPI_Bcast(&BcastRank, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);

		double end = MPI_Wtime();
		diff = end - begin;
		average += diff;
		squares += diff * diff;
	}
	double disp = ((squares / ITERATIONS) - (average / ITERATIONS) * (average / ITERATIONS));


	// double end = MPI_Wtime();
	
	// double time_difference = (end - begin) / ITERATIONS;
	return disp;// time_difference;
}

double MeasurePerformanceReduce(){

	int snd_buf(0), recv_buf(0);
	MPI_Barrier(MPI_COMM_WORLD); // !!!!!!!!!!!!
	
	double begin = MPI_Wtime();

	for(int i(0); i < ITERATIONS; ++i)
	{
		MPI_Reduce(&snd_buf, &recv_buf, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
	}

	double end = MPI_Wtime();
	
	double time_difference = (end - begin) / ITERATIONS;
	return time_difference;
}

double MeasurePerformanceGather(){
	int snd_buf(0);

	int proc_rank(0);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

	int * recv_buf = new int[proc_rank];
	MPI_Barrier(MPI_COMM_WORLD); // !!!!!!!!!!!!
	
	double begin = MPI_Wtime();

	for(int i(0); i < ITERATIONS; ++i)
	{
		// MPI_Barrier(MPI_COMM_WORLD);
		MPI_Gather(&snd_buf, 1, MPI_INT, recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
	}
	std::cout << "I'm here\n";
	delete[] recv_buf;

	double end = MPI_Wtime();
	
	double time_difference = (end - begin) / ITERATIONS;
	return time_difference;
}

double MeasurePerformanceScatter(){

	int snd_buf(0), recv_buf(0);
	MPI_Barrier(MPI_COMM_WORLD); // !!!!!!!!!!!!
	
	double begin = MPI_Wtime();

	for(int i(0); i < ITERATIONS; ++i)
	{
		MPI_Scatter(&snd_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
	}

	double end = MPI_Wtime();
	
	double time_difference = (end - begin) / ITERATIONS;
	return time_difference;
}




int main(int argc, char* argv[])
{
	int proc_num(0), proc_rank(0);
	// MPI code starts here
	int error = MPI_Init(&argc, &argv);
	// std::cout << MPI_Wtick() << std::endl; 
	if(error)
		std::cerr << "MPI_Init crashed" << std::endl;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	if(proc_rank == 0)
		std::cout << "Number of processes = " << proc_num << std::endl;


	double time_error = MeasurePerformanceBcast();
	MPI_Barrier(MPI_COMM_WORLD);
	//std::cout << "time difference = " << time_difference << std::endl;
	if(proc_rank == 0){
		printf("MPI_Wtick \t= %.17g\ntime error \t= %.17g\n", MPI_Wtick(), time_error);
		fflush(stdout);
	}
	
	
	MPI_Finalize();

	// MPI code ends here
}


/*
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
	std::cout << "Time difference by operation (sec) = " << time_difference_by_oper << std::endl;*/