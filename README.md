# Parallel_Programming
Parallel programming course at MIPT
## Task 1
N processes. Each process should print its MPI rank itself  
in right order from 0 to N - 1. Only `MPI_Send` and `MPI_Receive`.  
Two variants of right ordering realisation
- Network ring  
- Process 0 as a manager 

## Task 2
Measure worktime of 4 collective functions: `MPI_Bcast`, `MPI_Reduce`, `MPI_Gather`, `MPI_Scatter`.  
Accuracy in measurement has to be on the order of `MPI_Wtick()` precision.  

## Task 3
Write own realization of 4 collective functions: `MPI_Bcast`, `MPI_Reduce`, `MPI_Gather`, `MPI_Scatter`
using pairwise message exchange and `MPI_Barrier()`. Measure it's worktime.  
Accuracy in measurement has to be on the order of `MPI_Wtick()` precision.   
