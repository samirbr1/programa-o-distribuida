/*
            Disciplina: Tópicos Avançados em Computação III
            Curso: Engenharia de Computação - UFGD - FACET
            Atividade Avaliativa 1
            Implementação de programação paralela para controle de aeroportos
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
};

struct Aeroporto{
    int id;
    int numPousos = 0;
    int numDecolagens = 0;
    std::vector<Voo> pousos;
    std::vector<Voo> decolagens;
};


//protótipos das funções
void menu();
void tabela(const Aeroporto* aero);



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
            if(opt == 1 || opt == 2)
            {
                printf("Informe o aeroporto de origem(0 a %d):\n", (size - 1) );
                scanf("%d", &origem);
                if(origem >= size || origem < 0){
                    printf("Este aeroporto nao existe!\n");
                    opt = -1;
               }
            }
        }
	MPI_Bcast(&opt, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&origem, 1, MPI_INT, 0, MPI_COMM_WORLD);

        switch(opt)
        {
            case 1:
                if(rank == origem)
                {
                    fflush(stdin);
                    aeroporto.numDecolagens++;
                    printf("Informe o destino:\n");
                    scanf("%d", &destino);
                    fflush(stdin);
		    printf("Informe o horario de partida:\n");
		    scanf("%d", &chegada_partida);
		    fflush(stdin);
                    printf("Informe o tempo previsto de voo:\n");
                    scanf("%d", &tempo_voo);
                    voo_id = rank*100 + aeroporto.numDecolagens;
                    voo = {voo_id, origem, destino, chegada_partida, tempo_voo};
                    MPI_Send(&voo, sizeof(Voo), MPI_BYTE, destino, 0, MPI_COMM_WORLD);
                    aeroporto.decolagens.push_back(voo);
                }
		MPI_Bcast(&destino, 1, MPI_INT, origem, MPI_COMM_WORLD);
                if(rank == destino)
                {
                    aeroporto.numPousos++;
                    MPI_Recv(&voo, sizeof(Voo), MPI_BYTE, origem, 0, MPI_COMM_WORLD, &status);
                    voo.chegada_partida += voo.tempo_voo;
                    //tratar colisões aqui
                    aeroporto.pousos.push_back(voo);
                }
                break;
            case 2:
                if(rank == origem)
                    tabela(&aeroporto);
                break;
            case 3:
                if(rank == 0)
                    printf("Finalizando...\n");
                break;
            default:
                if(rank == 0)
                    printf("Selecione uma opcao valida!\n");
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

void tabela(const Aeroporto* aero){
    printf("---------------------------------------------------------\n");
    printf("| Codigo:\t|\t\t%d\t\t\t|\n", aero->id);
    printf("---------------------------------------------------------\n");
    printf("| Pousos:\t| %d\t\t| Decolagens:\t| %d\t|\n", aero->numPousos, aero->numDecolagens);
    printf("---------------------------------------------------------\n");
    printf("| Pousos\t| Origem\t| Chegada\t| Tempo\t|\n");
    printf("---------------------------------------------------------\n");
    for(int i = 0; i < (int)aero->pousos.size(); i++){
        printf("| %d\t\t| %d\t\t| %d\t\t| %d\t|\n", aero->pousos[i].voo_id, aero->pousos[i].origem, aero->pousos[i].chegada_partida, aero->pousos[i].tempo_voo);
    }
    printf("---------------------------------------------------------\n");
    printf("| Decolagens\t| Destino\t| Partida\t| Tempo\t|\n");
    for(int i = 0; i < (int)aero->decolagens.size(); i++){
        printf("| %d\t\t| %d\t\t| %d\t\t| %d\t|\n", aero->decolagens[i].voo_id, aero->decolagens[i].destino, aero->decolagens[i].chegada_partida, aero->decolagens[i].tempo_voo);
    }
    printf("---------------------------------------------------------\n");
}

