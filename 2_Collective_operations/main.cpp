#include <iostream>
#include <mpi.h>
#include <chrono>
#include <cmath>

enum MEASUREMENT{
	ITERATIONS = 1000000
};

inline void Calculate_disp(double begin, double end, double & average, double & squares)
{
	double diff = end - begin;
	average += diff;
	squares += diff * diff;
}

// Measure performance of collective routine including the cost of barrier
double MeasurePerformanceBcast(){

	int BcastRank(0);
	double squares(0.0), average(0.0);
	
	for(int i(0); i < ITERATIONS; ++i)
	{
		double begin = MPI_Wtime();

		MPI_Bcast(&BcastRank, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);

		double end = MPI_Wtime();
		Calculate_disp(begin, end, average, squares);	
			
	}
	double disp = ((squares / ITERATIONS) - (average / ITERATIONS) * (average / ITERATIONS));

	return disp;
}

double MeasurePerformanceReduce(){

	int snd_buf(0), recv_buf(0);
	double squares(0.0), average(0.0);

	for(int i(0); i < ITERATIONS; ++i)
	{
		double begin = MPI_Wtime();
		MPI_Reduce(&snd_buf, &recv_buf, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		double end = MPI_Wtime();

		Calculate_disp(begin, end, average, squares);
	}

	double disp = ((squares / ITERATIONS) - (average / ITERATIONS) * (average / ITERATIONS));
	return disp;
}

double MeasurePerformanceGather(){
	double squares(0.0), average(0.0);
	int snd_buf(0);
	int proc_rank(0);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

	int * recv_buf = new int[proc_rank];
	
	for(int i(0); i < ITERATIONS; ++i)
	{
		double begin = MPI_Wtime();
		MPI_Gather(&snd_buf, 1, MPI_INT, recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		double end = MPI_Wtime();

		Calculate_disp(begin, end, average, squares);	
	}
	
	delete[] recv_buf;

	double disp = ((squares / ITERATIONS) - (average / ITERATIONS) * (average / ITERATIONS));
	return disp;
}

double MeasurePerformanceScatter(){

	int snd_buf(0), recv_buf(0);
	double squares(0.0), average(0.0);
	
	for(int i(0); i < ITERATIONS; ++i)
	{
		double begin = MPI_Wtime();
		MPI_Scatter(&snd_buf, 1, MPI_INT, &recv_buf, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		double end = MPI_Wtime();

		Calculate_disp(begin, end, average, squares);	
	}

	double disp = ((squares / ITERATIONS) - (average / ITERATIONS) * (average / ITERATIONS));
	return disp;
}




int main(int argc, char* argv[])
{
	int proc_num(0), proc_rank(0);
	// MPI code starts here
	int error = MPI_Init(&argc, &argv);
	if(error)
		std::cerr << "MPI_Init crashed" << std::endl;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
	if(proc_rank == 0)
		std::cout << "Number of processes = " << proc_num << std::endl;


	double time_error = MeasurePerformanceScatter();
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(proc_rank == 0){
		printf("MPI_Wtick \t= %.17g\ntime error \t= %.17g\n", MPI_Wtick(), time_error);
		fflush(stdout);
	}
	
	
	MPI_Finalize();

	// MPI code ends here
}
