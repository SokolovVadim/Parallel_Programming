#include <iostream>
#include <mpi.h>
#include <chrono>
#include <cmath>
#include <typeinfo>

enum {ITERATIONS = 1000000 };

// -----------------------------------------------------------------------------------------------------

std::pair<double, double> MeasurePerformanceBcast();
std::pair<double, double> MeasurePerformanceReduce();
std::pair<double, double> MeasurePerformanceGather();
std::pair<double, double> MeasurePerformanceScatter();

// -----------------------------------------------------------------------------------------------------

inline void Calculate_disp(double begin, double end, double & average, double & squares)
{
	double diff = end - begin;
	average += diff;
	squares += diff * diff;
}

// -----------------------------------------------------------------------------------------------------

/*int main(int argc, char* argv[])
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


	std::pair<double, double> time = MeasurePerformanceBcast();
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(proc_rank == 0){
		printf("MPI_Bcast performance measurement\n"
			"MPI_Wtick \t= %.17g\nTime elapsed \t= %.17g\ntime error \t= %.17g\n",
		 MPI_Wtick(), time.first, time.second);
		fflush(stdout);
	}
	time = MeasurePerformanceReduce();
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(proc_rank == 0){
		printf("MPI_Reduce performance measurement\nTime elapsed \t= %.17g\ntime error \t= %.17g\n",
		 time.first, time.second);
		fflush(stdout);
	}
	time = MeasurePerformanceGather();
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(proc_rank == 0){
		printf("MPI_Gather performance measurement\nTime elapsed \t= %.17g\ntime error \t= %.17g\n",
		 time.first, time.second);
		fflush(stdout);
	}
	time = MeasurePerformanceScatter();
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(proc_rank == 0){
		printf("MPI_Scatter performance measurement\nTime elapsed \t= %.17g\ntime error \t= %.17g\n",
		 time.first, time.second);
		fflush(stdout);
	}
	
	
	MPI_Finalize();

	// MPI code ends here
}*/

// Measure performance of collective routine including the cost of barrier
std::pair<double, double> MeasurePerformanceBcast(){

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

	return std::make_pair(average / ITERATIONS, disp);
}

// -----------------------------------------------------------------------------------------------------

std::pair<double, double> MeasurePerformanceReduce(){

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
	return std::make_pair(average / ITERATIONS, disp);
}

// -----------------------------------------------------------------------------------------------------

std::pair<double, double> MeasurePerformanceGather(){
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
	return std::make_pair(average / ITERATIONS, disp);
}

// -----------------------------------------------------------------------------------------------------

std::pair<double, double> MeasurePerformanceScatter(){

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
	return std::make_pair(average / ITERATIONS, disp);
}

// -----------------------------------------------------------------------------------------------------

/*
MPI_STATUS_IGNORE passed to a receive function,
inform the implementation that the status fields are not to be filled in.
Because allocating a status object is particularly wasteful
for the MPI implementation to fill in fields in this object.
*/

void My_Bcast(
	void* data,
    int count,
    MPI_Datatype datatype,
    int root,
    MPI_Comm communicator)
{
	int world_rank(0), world_size(0);
	MPI_Comm_rank(communicator, &world_rank);
	MPI_Comm_size(communicator, &world_size);

	// root sends data to every process

	if(world_rank == root)
	{
		for(int i(0); i < world_size; ++i)
		{
			if(i != world_rank)
				MPI_Send(data, count, datatype, i, 0, communicator);
		}
	}
	else // other processes receive data from root
		MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
}

// -----------------------------------------------------------------------------------------------------

void My_Reduce(
	void* send_data,
    void* recv_data,
    int count,
    MPI_Datatype datatype,
    MPI_Op op,
    int root,
    MPI_Comm communicator)
{
	if(datatype != MPI::INT)
	{
		printf("error! this function requires only MPI::INT datatype!\n");
	}
	int world_rank(0), world_size(0);
	MPI_Comm_rank(communicator, &world_rank);
	MPI_Comm_size(communicator, &world_size);

	// root receive data from other processes and show 
	if(world_rank == root)
	{
		int prod(*(int*)send_data);
		for(int i(0); i < world_size; ++i)
		{
			if(i != root){
				MPI_Recv(send_data, count, datatype, i, MPI_ANY_TAG, communicator, MPI_STATUS_IGNORE);
				prod *= *(int*)send_data;
			}
		}
		*(int*)recv_data = prod;
	}
	else // other processes send data to root
	{
		MPI_Send(send_data, count, datatype, root, world_rank, communicator);
	}

}


// -----------------------------------------------------------------------------------------------------

void My_Scatter(
	void* send_data,
    int send_count,
    MPI_Datatype send_datatype,
    void* recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    int root,
    MPI_Comm communicator)
{
	if(send_datatype != MPI::INT)
	{
		printf("error! this function requires only MPI::INT datatype!\n");
	}
	int world_rank(0), world_size(0);
	MPI_Comm_rank(communicator, &world_rank);
	MPI_Comm_size(communicator, &world_size);


	// root sends pieces of data to every process
	void* ptr_to_send(nullptr);

	if(world_rank == root)
	{
		for(int i(0); i < world_size; ++i)
		{
			if(i != root){
				ptr_to_send = (void*)((int*)send_data + i * send_count);
				MPI_Send(ptr_to_send, send_count, send_datatype, i, world_rank, communicator);
			}
		}
		recv_data = send_data;
	}
	else// other processes receive data from root
		MPI_Recv(recv_data, recv_count, recv_datatype, root, MPI_ANY_TAG, communicator, MPI_STATUS_IGNORE);
}

// -----------------------------------------------------------------------------------------------------

void My_Gather(
    void* send_data,
    int send_count,
    MPI_Datatype send_datatype,
    void* recv_data,
    int recv_count,
    MPI_Datatype recv_datatype,
    int root,
    MPI_Comm communicator)
{
	if(send_datatype != MPI::INT)
	{
		printf("error! this function requires only MPI::INT datatype!\n");
	}
	int world_rank(0), world_size(0);
	MPI_Comm_rank(communicator, &world_rank);
	MPI_Comm_size(communicator, &world_size);

	// root receive pieces of data from other processes 
	if(world_rank == root)
	{
		int* data = (int*)recv_data;
		data[root] = *(int*)send_data;
		for(int i(0); i < world_size; ++i)
		{
			if(i != root){
				MPI_Recv(send_data, recv_count, recv_datatype, i, MPI_ANY_TAG, communicator, MPI_STATUS_IGNORE);
				data[i] = *(int*)send_data;
			}
		}
	}
	else // other processes send data to root
	{
		MPI_Send(send_data, send_count, send_datatype, root, world_rank, communicator);
	}
}



void TestOK(int argc, char** argv)
{
	int proc_num(0), proc_rank(0);
	// MPI code starts here
	int error = MPI_Init(&argc, &argv);
	if(error)
		std::cerr << "MPI_Init crashed" << std::endl;

	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

	// test My_Bcast here

	/*int value(713);
	My_Bcast(&value, 1, MPI::INT, 0, MPI_COMM_WORLD);
	printf("value: %d\n", value);*/

	// test My_Reduce here
/*
	int local(proc_rank + 1);
	int prod(0);
	My_Reduce(&local, &prod, 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);
	if(proc_rank == 0)
	{
		std::cout << "Prod: " << prod << std::endl;
	}*/

	// Test My_Scatter here

	/*int* local(nullptr);
	if(proc_rank == 0){
		local = new int[proc_num];
		for(int i(0); i < proc_num; ++i){
			local[i] = i;
		}
	}
	
	int recv(0);
	My_Scatter(local, 1, MPI::INT, &recv, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	printf("received: %d\n", recv);
	
	if(proc_rank == 0)
		delete[] local;*/

	// Test My_Gather here

	/*int* data(nullptr);
	if(proc_rank == 0){
		data = new int[proc_num];
	}
	
	int send_data(proc_rank);
	My_Gather(&send_data, 1, MPI::INT, data, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(proc_rank == 0){
		for(int i(0); i < proc_num; ++i)
			printf("data[%d] = %d\n", i, data[i]);

		delete[] data;
	}*/



	MPI_Finalize();
}

// -----------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
	TestOK(argc, argv);
}
