//
//  Created by Andrew on 5/02/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>
#include <stdbool.h>

#define INPUT_FILENAME "data.txt"
#define OUTPUT_FILENAME "output.txt"

// по сколько цифр дробить число в массив
const int OPERANDSIZE = 9;

const int ROOT_PROCESS = 0;


typedef struct operands {
    int length;

    int* num1;
    int* num2;
} opers;


typedef struct calculationResults {
    bool allNines;

    int* resWithoutTransfer;
    int* resWithTransfer;

    int transferToNext;
} calculationResults;


void stringToArray(int* dest, const char* str, const int blockSize) {
    int i = 0;
    char* substring = malloc(OPERANDSIZE);

    for(i = 0; i < blockSize; i ++) {
        unsigned pos = OPERANDSIZE * i;
        strncpy(substring, str + pos, OPERANDSIZE);

        int num = atoi(substring);

        //printf("stringToArray: %d\n", num);
        dest[i] = num;
    }
    free(substring);
}

void readFromFile(const char* file_name, opers* ops) {
    // First number in input - length of operands
    int numberLength = 0;
    // used to skip symbols separating numbers
    char nextLine = '\n';

    FILE* file = fopen (file_name, "r");
    if(!file){
        printf("Bad file\n");
        exit(1);
    }

    fscanf(file, "%d", &numberLength);

    if(numberLength % 9 != 0) {
        printf("Wrong numbers length\n");
        exit(1);
    }

    fscanf(file, "%c", &nextLine);

    char* number1 = malloc(numberLength);
    char* number2 = malloc(numberLength);

    fread (number1, sizeof(char), numberLength, file);
    fscanf(file, "%c", &nextLine);
    fread (number2, sizeof(char), numberLength, file);

    fclose (file);

    // printf("First number: %s, second number: %s\n", number1, number2);
    ops->length = numberLength / OPERANDSIZE;
    ops->num1 = malloc(sizeof(int) * numberLength / OPERANDSIZE);
    stringToArray(ops->num1, number1, numberLength / OPERANDSIZE);

    ops->num2 = malloc(sizeof(int) * numberLength / OPERANDSIZE);
    stringToArray(ops->num2, number2, numberLength / OPERANDSIZE);

    int* ArrayNum1 = malloc(sizeof(int) * numberLength / OPERANDSIZE);
    stringToArray(ArrayNum1, number1, numberLength / OPERANDSIZE);

    int* ArrayNum2 = malloc(sizeof(int) * numberLength / OPERANDSIZE);
    stringToArray(ArrayNum2, number2, numberLength / OPERANDSIZE);

    opers Opers = {numberLength, ArrayNum1, ArrayNum2};

    free(number1);
    free(number2);
}

void printNumber(const int* arr, int len) {
    int i;
    for(i = 0; i < len; i ++) {
        printf("%09d", arr[i]);
    }
    printf("\n");
}

void printResultIntoFile(const char* file_name, const int first, const int* results, const int len) {
    FILE *output_file = fopen(file_name, "w");
    if(!output_file) {
        printf("Bad output file\n");
        exit(1);
    }

    if(first == 1) {
        fprintf(output_file, "%d", first);
    }
    int i;
    for(i = 0; i < len; i ++) {
        fprintf(output_file, "%09d", results[i]);
    }

    fclose(output_file);
}


void Calculate(calculationResults* res, const int* num1, const int* num2, int blockSize) {
    res->resWithoutTransfer = malloc(blockSize);
    res->resWithTransfer = malloc(blockSize);

    // перенос внутри одного блока
    int local_transfer = 0;
    bool twoResults = false;

    int i;
    //идем от наименьшего разряда к наибольшему
    for(i = blockSize - 1; i >= 0; i --) {

        if(i == blockSize - 1) {
            int result = num1[i] + num2[i];
            if(result == 999999999) {
                // не знаем, есть ли перенос =>
                res->resWithTransfer[i] = 0;
                res->resWithoutTransfer[i] = result;
                twoResults = true;
            } else {
                // обычный ход вычислений
                res->resWithoutTransfer[i] = result % 1000000000;
                // res->resWithTransfer[i] = 1 + result % 1000000000;
                local_transfer = result / 1000000000;
                twoResults = false;
            }

        } else if(i > 0) {
            // однозначные вычисления
            if(!twoResults) {
                int result = num1[i] + num2[i] + local_transfer;
                local_transfer = result / 1000000000;
                res->resWithoutTransfer[i] = result % 1000000000;
                // res->resWithTransfer[i] = res->resWithoutTransfer[i];
            } else {
                // неоднозначные - не знаем, что придет из предыдущего
                int result = num1[i] + num2[i];
                if(result != 999999999) {
                    local_transfer = result / 1000000000;       // можем однозначно его посчитать
                    res->resWithTransfer[i] = 1 + result % 1000000000;
                    res->resWithoutTransfer[i] = result % 1000000000;
                    // twoResults = false;
                } else {
                    // не знаем, есть ли перенос =>
                    res->resWithTransfer[i] = 0;
                    res->resWithoutTransfer[i] = result;
                    twoResults = true;
                }
            }

        } else {            // первый разряд (который самый болшьой)
            if(!twoResults) {
                int result = num1[i] + num2[i] + local_transfer;
                res->resWithoutTransfer[i] = result % 1000000000;
                // res->resWithTransfer[i] = res->resWithoutTransfer[i];
                local_transfer = result / 1000000000;
                // res->allNines = false;
            } else {
                int result = num1[i] + num2[i];
                if(result != 999999999) {
                    res->resWithTransfer[i] = result % 1000000000;
                    local_transfer = result / 1000000000;
                    // twoResults = false;
                } else {
                    // не знаем, есть ли перенос =>
                    res->resWithTransfer[i] = 0;
                    res->resWithoutTransfer[i] = result;
                    twoResults = true;
                }
            }
        }
    }
    res->transferToNext = local_transfer;
    res->allNines = twoResults;
}


int main(int argc, char* argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status Status;

    double start_time = 0.0, finish_time = 0.0;

    int BLOCKSIZE = 0;
    opers ops;

    if(rank == ROOT_PROCESS) {
        readFromFile(INPUT_FILENAME, &ops);

        printf("Length of numbers: %d\n", ops.length);
        printf("Numbers:\n");
        printNumber(ops.num1, ops.length);
        printNumber(ops.num2, ops.length);

        start_time = MPI_Wtime();

        BLOCKSIZE = ops.length / (size - 1);
        printf("BLOCK SIZE: %d\n", BLOCKSIZE);
    }

    MPI_Bcast(&BLOCKSIZE, 1, MPI_INT, ROOT_PROCESS, MPI_COMM_WORLD);
    //printf("Process num: %d, got block size: %d\n", rank, BLOCKSIZE);
    //MPI_Scatter(ops.num1, BLOCKSIZE, MPI_INT, receiver, BLOCKSIZE, MPI_INT, ROOT_PROCESS, MPI_COMM_WORLD);

    if(rank == ROOT_PROCESS) {
        int i;
        // root не занимается непосредственно вычислениями => рассылаем всем, кроме него самого
        for(i = 1; i < size; i ++) {
            MPI_Send(ops.num1 + BLOCKSIZE * (i-1), BLOCKSIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(ops.num2 + BLOCKSIZE * (i-1), BLOCKSIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        // не в root'е структура используется для храниения блоков
        ops.length = BLOCKSIZE;
        ops.num1 = malloc(BLOCKSIZE);
        ops.num2 = malloc(BLOCKSIZE);

        MPI_Recv(ops.num1, BLOCKSIZE, MPI_INT, ROOT_PROCESS, 0, MPI_COMM_WORLD, &Status);
        MPI_Recv(ops.num2, BLOCKSIZE, MPI_INT, ROOT_PROCESS, 0, MPI_COMM_WORLD, &Status);

        printf("------------------------\n");
        printf("Process num: %d\n", rank);
        printNumber(ops.num1, BLOCKSIZE);
        printNumber(ops.num2, BLOCKSIZE);

        calculationResults CR;
        Calculate(&CR, ops.num1, ops.num2, ops.length);
        printf("Process num: %d, result ", rank);
        printNumber(CR.resWithoutTransfer, BLOCKSIZE);
        printf("transfer %d\n", CR.transferToNext);

        //вычисления проведены, теперь нужно обменяться переносами и отправить нужный вариант в root
        if(rank == size - 1) {
            MPI_Send(&CR.transferToNext, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
            MPI_Send(CR.resWithoutTransfer, BLOCKSIZE, MPI_INT, ROOT_PROCESS, 0, MPI_COMM_WORLD);
        }
        else {
            int transfer;
            MPI_Recv(&transfer, 1, MPI_INT, (rank + 1), 0, MPI_COMM_WORLD, &Status);

            if(transfer == 0) {
                if(CR.allNines) {
                    CR.transferToNext = 0;
                }
                //if(rank != 1)        //последний пересылает дальше в root
                MPI_Send(&CR.transferToNext, 1, MPI_INT, (rank - 1), 0, MPI_COMM_WORLD);

                MPI_Send(CR.resWithoutTransfer, BLOCKSIZE, MPI_INT, ROOT_PROCESS, 0, MPI_COMM_WORLD);
            }

            if(transfer == 1) {
                if(CR.allNines) {
                    CR.transferToNext = 1;
                }
                //if(rank != 1)        //последний пересылает дальше в root
                MPI_Send(&CR.transferToNext, 1, MPI_INT, (rank - 1), 0, MPI_COMM_WORLD);

                if(CR.allNines) {       // есть блок со всеми девятками => пересылка "особого" массива
                    MPI_Send(CR.resWithTransfer, BLOCKSIZE, MPI_INT, ROOT_PROCESS, 0, MPI_COMM_WORLD);
                } else {
                    CR.resWithoutTransfer[BLOCKSIZE - 1] += transfer;
                    MPI_Send(CR.resWithoutTransfer, BLOCKSIZE, MPI_INT, ROOT_PROCESS, 0, MPI_COMM_WORLD);
                }
            }
        }
        free(ops.num1);
        free(ops.num2);
        free(CR.resWithoutTransfer);
        free(CR.resWithTransfer);
    }

    if(rank == ROOT_PROCESS) {
        int first;
        // перенос разряда в root
        MPI_Recv(&first, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &Status);
        //printf("ROOT got transfer: %d\n", first);

        finish_time = MPI_Wtime();
        double runtime = (finish_time - start_time) * 100000;
        printf("Time: %lg\n", runtime);

        int i;
        int* results = malloc(BLOCKSIZE * (size-1));

        for(i = 1; i < size; i ++) {
            MPI_Recv(results + (i - 1) * BLOCKSIZE, BLOCKSIZE, MPI_INT, i, 0, MPI_COMM_WORLD, &Status);
        }

        printResultIntoFile(OUTPUT_FILENAME, first, results, (size - 1) * BLOCKSIZE);

        printf("\n");

        free(results);
    }

    MPI_Finalize();
    return 0;
}