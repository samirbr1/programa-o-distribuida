/*
            Disciplina: Tópicos Avançados em Computação III
            Curso: Engenharia de Computação - UFGD - FACET
            Atividade Avaliativa 1
            Implementação do algoritmo one-to-all broadcast para a topologia de dispersão linear e hipercubo
            Docente: Rodrigo Porfírio da Silva Sacchi
            
            Discentes:
            Felipe Emanuel Ferreira     RGA: 20200712175441
            Samir Barcelos Murad        RGA: 20200712181691
*/

//inclusão de bibliotecas
#include <stdio.h> 
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "mpi.h"

//structs usadas
struct Voo{
    int voo_id;
    int origem;
    int destino;
    int chegada_partida;
    int tempo_voo = 0;
}

struct Aeroporto{
    int id;
    int numPousos = 0;
    int numDecolagens = 0;
    std::vector<Voo> pousos;
    std::vector<Voo> decolagens;
}


//protótipos das funções
void menu();




int main(int argc, char* argv[])
{
    int size, rank;
    int opt = -1;
    Voo voo;
    
    int voo_id, origem, destino, chegada_partida, tempo_voo;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Aeroporto aeroporto = {rank};

    while(opt != 3)
    {
        if(rank == 0)
        {
            menu();
            scanf("%d", &opt);
            MPI_Bcast(&opt, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if(opt == 1)
            {
                printf("Informe o aeroporto de origem(0 a %d): ", (size - 1) )
                scanf("%d", &origem);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        switch(opt)
        {
            case 1:
                if(rank == origem)
                {
                    aeroporto.numDecolagens++;
                    printf("\nInforme o destino: ");
                    scanf("%d, &destino")
                    MPI_Bcast(&destino, 1, MPI_INT, origem, MPI_COMM_WORLD);
                    fflush(stdin);
                    printf("\n informe o tempo de voo: ");
                    scanf("%d, &tempo_voo")
                    voo_id = rank*10 + aeroporto.numDecolagens;
                    voo = {voo_id, origem, destino, 0, tempo_voo};
                    MPI_Send(&voo, sizeof(Voo), MPI_BYTE, destino, 0, MPI_COMM_WORLD);
                    aeroporto.decolagens.push_back(voo);
                }
                MPI_Barrier(MPI_COMM_WORLD);
                if(rank == destino)
                {
                    MPI_Recv(&voo, sizeof(Voo), MPI_BYTE, origem, 0, MPI_comm_world, &status);
                }
        }
    }
    MPI_Finalize();
    return 0;
}

void menu()
{
    printf("Selecione uma opcao\n");
    printf("1 - Informar dados do voo\n");
    printf("2 - Consultar tabela aeroporto\n");
    printf("3 - Sair\n");
}