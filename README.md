# Parallel_Programming

Parallel programming course at MIPT

```
ssh s71316@remote.vdi.mipt.ru -p 52960
scp -P 52960 -r build s71316@remote.vdi.mipt.ru:/home/s71316/
```

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

## Task 4

Calculate sum of the series via N processes.

![task](https://github.com/SokolovVadim/Parallel_Programming/blob/master/4_Series_sum/resources/series.png)

The task is to get the most out of accuracy in calculations according to float data representation standart

## Task 5
#### Static balancing
 - Input  
Program reads from file lenght of two numbers then each number. Numbers are divided into blocks.
 - Speculative sum  
Each process performs speculative calculations. Number of blocks is equal to number of processes  
 - Output  
 Root process is to gather the result and process numbers in sum.
![spec](https://github.com/SokolovVadim/Parallel_Programming/blob/master/5_Static_balancing/resources/Speculative.png)

## Task 6
#### Dynamic balancing
 - Input  
Program reads from file lenght of two numbers then each number. Numbers are divided into blocks.
 - Speculative sum  
Each process performs speculative calculations. Number of blocks is bigger than number of processes  
 - Output  
 Root process is to gather the result and process numbers in sum.
