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
#include <stdlib.h>
#include <vector>
#include "mpi.h"

//structs usadas
struct Voo{
    int voo_id = -1;
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
    int aux;
    int origem, destino, chegada_partida, tempo_voo;

    MPI_Status status;
    MPI_Request request = MPI_REQUEST_NULL;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Aeroporto aeroporto = {rank};

    while(opt != 3)
    {
	destino = -1;
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
                if(rank == 0)
                {
                    do
                    {
                        printf("Informe o destino:\n");
                        scanf("%d", &destino);
                    }while(destino == origem || destino < 0 || destino >= size);
                    do
                    {
		        printf("Informe o horario de partida:\n");
		        scanf("%d", &chegada_partida);
		    }while(chegada_partida < 0 || chegada_partida >= 24);

		    do
		    {
                        printf("Informe o tempo previsto de voo:\n");
                        scanf("%d", &tempo_voo);
                    }while(tempo_voo <= 0);

                    voo = {-1, origem, destino, chegada_partida, tempo_voo};

                    MPI_Isend(&voo, sizeof(Voo), MPI_BYTE, origem, 0, MPI_COMM_WORLD, &request);
                }
		MPI_Bcast(&destino, 1, MPI_INT, 0, MPI_COMM_WORLD);
                if(rank == origem)
                {
                    MPI_Recv(&voo, sizeof(Voo), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
   		    aeroporto.numDecolagens++;
                    voo.voo_id = rank * 100 + aeroporto.numDecolagens;
		    aeroporto.decolagens.push_back(voo);
                    MPI_Send(&voo, sizeof(Voo), MPI_BYTE, destino, 1, MPI_COMM_WORLD);
                    aux = aeroporto.numDecolagens - 1; //ultimo elemento inserido
                    for(int i = 0; i < aeroporto.numDecolagens; i++)
                    {
                        if(i == aux)
                            break;
                        if(aeroporto.decolagens[i].chegada_partida == aeroporto.decolagens[aux].chegada_partida)
                        {
                            printf("Duas decolagens simultaneas! Priorizando o de maior tempo de voo...\n");
                            if(aeroporto.decolagens[aux].tempo_voo > aeroporto.decolagens[i].tempo_voo)
                            {
                                aeroporto.decolagens[i].chegada_partida++;
                                printf("Voo de id %d foi adiado devido a outra decolagem de maior prioridade!\n", aeroporto.decolagens[i].voo_id);
                                aux = i; //verifica se há outros conflitos com o novo horário
                            }
                            else
                            {
                                aeroporto.decolagens[aux].chegada_partida++;
				printf("Voo de id %d adiado devido a outra decolagem de maior prioridade!\n", aeroporto.decolagens[aux].voo_id);
                            }
                            i = 0;
                        }
                    }
                }
                if(rank == destino)
                {
                    MPI_Recv(&voo, sizeof(Voo), MPI_BYTE, origem, 1, MPI_COMM_WORLD, &status);
                    voo.chegada_partida += voo.tempo_voo;
                    if(voo.chegada_partida >= 24)
                    {
                        printf("Pouso agendado para o dia seguinte.\n");
                        break;
                    }
                    aeroporto.numPousos++;
                    aeroporto.pousos.push_back(voo);
                    aux = aeroporto.numPousos - 1;
                    for(int i = 0; i < aeroporto.numDecolagens; i++) //verifica conflito de voo com decolagem
                    {
			if(aux == i)
			    break;
                        if(aeroporto.decolagens[i].chegada_partida == aeroporto.pousos[aux].chegada_partida)
                        {
                            aeroporto.decolagens[i].chegada_partida++;
                            aux = i;
                            for(int j = 0; j < aeroporto.numDecolagens; j++)
                            {
                                if(aux == j)
                                    break;
                                if(aeroporto.decolagens[j].chegada_partida == aeroporto.decolagens[aux].chegada_partida){
                                    printf("Decolagem sera adiado por causa de um pouso!\n");
                                    if(aeroporto.decolagens[aux].tempo_voo > aeroporto.decolagens[j].tempo_voo)
                                    {
                                        aeroporto.decolagens[j].chegada_partida++;
                                        printf("Voo %d adiado devido a uma decolagem de maior prioridade", aeroporto.decolagens[j].voo_id);
                                        aux = j;
                                    }
                                    else
                                    {
                                        aeroporto.decolagens[aux].chegada_partida++;
                                        printf("Voo %d adiado devido a uma decolagem de maior prioridade", aeroporto.decolagens[aux].voo_id);
                                    }
                                    j = 0;
                                }
                            }
                        }
                    }
                    aux = aeroporto.numPousos - 1;
                    for(int i = 0; i < aeroporto.numPousos; i++) //Verifica conflitos de pousos
	 	    {
                        if(i == aux)
                            break;
                        if(aeroporto.pousos[i].chegada_partida == aeroporto.pousos[aux].chegada_partida)
                        {
                            printf("Pousos simultaneos! Priorizando o de maior tempo de voo\n");
                            if(aeroporto.pousos[aux].tempo_voo > aeroporto.pousos[i].tempo_voo)
                            {
                                aeroporto.pousos[i].chegada_partida++;
                                aeroporto.pousos[i].tempo_voo++;
                                printf("Voo %d tera que aguardar pouso de prioridade maior!\n", aeroporto.pousos[i].voo_id);
                                aux = i;
                            }
                            else
                            {
                                aeroporto.pousos[aux].chegada_partida++;
                                aeroporto.pousos[aux].tempo_voo++;
                                printf("Voo %d tera que aguardar pouso de prioridade maior!\n", aeroporto.pousos[i].voo_id);
                            }
                            i = 0;
                        }
                    }
                }
                break;
            case 2:
                if(rank == origem)
                    tabela(&aeroporto);
		MPI_Barrier(MPI_COMM_WORLD); //para não atrapalhar o ptint de outros processos que não seja o 0
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
    printf("2 - Consultar decolagens/pousos agendados para hoje\n");
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
        printf("| %03d\t\t| %d\t\t| %d\t\t| %d\t|\n", aero->pousos[i].voo_id, aero->pousos[i].origem, aero->pousos[i].chegada_partida, aero->pousos[i].tempo_voo);
    }
    printf("---------------------------------------------------------\n");
    printf("| Decolagens\t| Destino\t| Partida\t| Tempo\t|\n");
    for(int i = 0; i < (int)aero->decolagens.size(); i++){
        printf("| %03d\t\t| %d\t\t| %d\t\t| %d\t|\n", aero->decolagens[i].voo_id, aero->decolagens[i].destino, aero->decolagens[i].chegada_partida, aero->decolagens[i].tempo_voo);
    }
    printf("---------------------------------------------------------\n");
}
