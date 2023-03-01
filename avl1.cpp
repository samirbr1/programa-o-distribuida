#include <stdio.h>
#include <iostream>
#include <math.h>
#include "mpi.h"


using namespace std;
//sacchi disse pra fazer em etapas
// o número de vezes que ele roda é pela potência de 2, se tem 8 nós são 3 passos, 16, então 4


int main(int argc, char *argv[])
{
	int size, rank;
	int message = 1111;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	/*for(int i = 0; i < rank; rank / 2)
	{
	
	}

	if(rank == 0)
	{
		MPI_Send(&message, 1, MPI_INT, size/2, 0, MPI_COMM_WORLD);
		MPI_Recv(&message, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	else
	{
		MPI_Recv(&message, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	
	*/	
	int num = 8;
    printf("%.0f\n", log2(num));
     
    
    
    for(int i = num/2; i > 0; i = i >> 1)
    {
        for(int j = 0; j < num; j = j + pow(2, i))
            printf("[%d -> %d]", j, j+i);
        printf("\n");
    }
    //for(int i = log2(8); i > 0; i--)
    //    printf("teste2 %d\n", i); 

	MPI_Finalize();
	return 0;
}


void one2all(int msg)
{
	int rank, size, d;
	int destiny, source;
	int mask;
	
	d = log2(size);
	mask = pow(2, d) - 1;

	for(int i = d - 1; i >= 0; i--)
	{
		mask ^= pow(2, i);

		if((rank & pow(2,i)) == 0)
		{
			destiny = rank ^ pow(2,i);
			MPI_Send(&msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
		}
		else
		{
			source = rank ^ pow(2,i);
			MPI_Recv(&msg, 1, MPI_INT, source, 0, MPI_COMM_WORLD);
		}
	}
	
}
