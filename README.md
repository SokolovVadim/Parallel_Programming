# Parallel_Programming
Parallel programming course at MIPT
## 1st Task
N processes. Each process should print its MPI rank itself  
in right order from 0 to N - 1. Only `MPI_Send` and `MPI_Receive`.  
Two variants of right ordering realisation
- Network ring  
- Process 0 as a manager  
## 2nd Task  
Measure worktime of 4 collective functions: MPI_Bcast, MPI_Reduce, MPI_Gather, MPI_Scatter.  
Accuracy in measurement has to be on the order of MPI_Wtick() precision.  

