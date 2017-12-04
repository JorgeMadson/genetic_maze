#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define LIMITE_MOVIMENTACAO 15
#define LAB_X 10
#define LAB_Y 10

/*
Definição das funções para funcionamento do software
*/


/* Parametros sobre a população */
#define TAM_POPULACAO 500
#define MELHORES_ESPECIMES 7
#define REPRODUTIVOS 150
#define VAL_PENALIDADE 2
#define CHANCE_MUTACAO 0.5f
#define PONT_FINAL 3


typedef struct{
	int x;
	int y;
}ponto_t;

typedef struct{
	int pontuacao;
	int indice;
}pontuacao_t;



void printar_lab();
void printar_lab_caminho();

void resetar_pessoa();
void pessoa_parada();
void pessoa_move_cima();
void pessoa_move_abaixo();
void pessoa_move_esquerda();
void pessoa_move_direita();
void preencher();
int* ordenar(int* vetor, int size);
void ordernar_pontuacao_t();
void crossover();
int fitness(int chromo);
int pontuacao();
void mover_pessoa();
void mutacao();



int vetor[TAM_POPULACAO][LIMITE_MOVIMENTACAO];
int melhor_spec[MELHORES_ESPECIMES][LIMITE_MOVIMENTACAO];

ponto_t pos_final;
ponto_t pessoa;
int penalidade;

pontuacao_t pontos[TAM_POPULACAO];

void (*func_ptr[5])() = {pessoa_parada, pessoa_move_cima, pessoa_move_abaixo, pessoa_move_esquerda, pessoa_move_direita};


int labirinto[LAB_X][LAB_Y] = {	{'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
								{'#', ' ', ' ', ' ', '#', ' ', '#', ' ', ' ', '#'},
								{'#', ' ', '#', ' ', ' ', ' ', '#', ' ', '#', '#'},
								{'#', ' ', '#', ' ', '#', '#', '#', ' ', ' ', '#'},
								{'#', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#', '#'},
								{'#', ' ', ' ', ' ', '#', ' ', '#', ' ', '#', '#'},
								{'#', ' ', '#', ' ', '#', ' ', '#', ' ', ' ', '#'},
								{'#', '#', '#', ' ', '#', ' ', '#', '#', ' ', '#'},
								{'#', ' ', '#', ' ', ' ', '#', '#', '#', ' ', '#'},
								{'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}};


void printar_lab_caminho(){
	//int cnt = 0;
	char temp[LAB_X][LAB_Y] = {};
	memcpy(temp, labirinto, LAB_X*LAB_Y);
	int i;
	for(i = 0; i < LIMITE_MOVIMENTACAO; i++){
		if(vetor[pontos[0].indice][i]){
			func_ptr[vetor[pontos[0].indice][i]](); // melhor pontuacao de caminho
			labirinto[pessoa.x][pessoa.y] = '*';//(char)(i+97);
			//cnt++;
		}
	}
    int x,y;
	for(x = 0; x < LAB_X; x++){
		for(y = 0; y < LAB_Y; y++){
			if(pessoa.x == x && pessoa.y == y)
				printf(" P ");
			else if(pos_final.x == x && pos_final.y == y)
				printf(" F ");
			else
				printf(" %c ", labirinto[x][y]);

		}
		printf("\r\n");
	}
	resetar_pessoa();
	memcpy(labirinto, temp, LAB_X*LAB_Y);
	printf("\r\n");
	//printf("Goal reached in %d moves\r\n", cnt);
}

void printar_lab(){
    int x,y;
	for(x = 0; x < LAB_X; x++){
		for(y = 0; y < LAB_Y; y++){
			if(pessoa.x == x && pessoa.y == y)
				printf(" P ");
			else if(pos_final.x == x && pos_final.y == y)
				printf(" F ");
			else
				printf(" %c ", labirinto[x][y]);

		}
		printf("\r\n");
	}
	printf("\r\n");
}

void resetar_pessoa(){
	pessoa.x = 1;
	pessoa.y = 1;
	penalidade = 0;
}

void pessoa_parada(){
	// add penalidade?
	// penalidade += 1;
	return;
}
void pessoa_move_cima(){
	if(labirinto[pessoa.x-1][pessoa.y] == '#'){
		penalidade += VAL_PENALIDADE;
		return;
	}
	pessoa.x -= 1;
}

void pessoa_move_abaixo(){
	if(labirinto[pessoa.x+1][pessoa.y] == '#'){
		penalidade += VAL_PENALIDADE;
		return;
	}
	pessoa.x += 1;
}

void pessoa_move_esquerda(){
	if(labirinto[pessoa.x][pessoa.y-1] == '#'){
		penalidade += VAL_PENALIDADE;
		return;
	}
	pessoa.y -= 1;
}

void pessoa_move_direita(){
	if(labirinto[pessoa.x][pessoa.y+1] == '#'){
		penalidade += VAL_PENALIDADE;
		return;
	}
	pessoa.y += 1;
}



void preencher(){
    int x,y;
	for(x = 0; x < TAM_POPULACAO; x++){
		for(y = 0; y < LIMITE_MOVIMENTACAO; y++){
			vetor[x][y] = rand() % 5;
		}
	}
}

// bubble ordenar
int* ordenar(int* vetor, int size){
    int x,i;
	for(x= 0; x < size - 1; x++){
		for(i = 0; i < size - x - 1; i++){
			if(vetor[i] > vetor[i+1]){
				int c = vetor[i];
				vetor[i+1] = vetor[i];
				vetor[i] = c;
			}
		}
	}
	return vetor;
}

void ordernar_pontuacao_t(){
    int x,i;
	for(x = 0; x < TAM_POPULACAO - 1; x++){
		for(i = 0; i < TAM_POPULACAO - x - 1; i++){
			if(pontos[i].pontuacao > pontos[i+1].pontuacao){
			    pontuacao_t temp = pontos[i];
			    pontos[i] = pontos[i+1];
			    pontos[i+1] = temp;
			}
		}
	}
}
//crossover
void crossover() {
	//copy to melhor_spec vetor
	int i;
	for (i = 0; i < MELHORES_ESPECIMES; i++) {
		memcpy(melhor_spec[i], vetor[pontos[i].indice], LIMITE_MOVIMENTACAO);
	}

	int firstParent = rand() % MELHORES_ESPECIMES;
	int secondParent = rand() % MELHORES_ESPECIMES;
	int crossOver = rand() % LIMITE_MOVIMENTACAO;

	for (i = MELHORES_ESPECIMES + REPRODUTIVOS; i < TAM_POPULACAO; i++) {
		memcpy(vetor[i], melhor_spec[firstParent], crossOver);
		memcpy(vetor[i], melhor_spec[secondParent], (LIMITE_MOVIMENTACAO - crossOver));
	}

}

int fitness(int chromo){
    int i;
	for(i = 0; i < LIMITE_MOVIMENTACAO; i++){
		func_ptr[vetor[chromo][i]]();
	}

	int pontuacao = abs(pos_final.x - pessoa.x) + abs(pos_final.y - pessoa.y) + penalidade;

	resetar_pessoa();
	return pontuacao;
}

int pontuacao(){
    int i;
	for(i = 0; i < TAM_POPULACAO; i++){
		pontos[i].pontuacao = fitness(i);
		pontos[i].indice = i;
		//printf("%d %d\n", pontos[i].pontuacao, pontos[i].indice);
	}
	ordernar_pontuacao_t();
	return pontos[0].indice;
}
//

void mover_pessoa(){
    int i;
	for(i = 0; i < LIMITE_MOVIMENTACAO; i++){
		func_ptr[vetor[pontos[0].indice][i]]();
	}
}

void mutacao(){
    int i;
	for(i = 0; i < TAM_POPULACAO-1; i++){
		if(CHANCE_MUTACAO > (double)rand() / (double)RAND_MAX){ // random number (0,1)
			vetor[i][rand() % 30] = rand() % 5; // chhose random element and mutacao
		}
	}
}

int main(){
	time_t t;
	/* Inicializa o random number generator */
	srand((unsigned) time(&t));

	pessoa.x = 1;
	pessoa.y = 1;
	pos_final.x = 8;
	pos_final.y = 8;
	preencher();
	printf("Iniciar labirinto: \r\n");
	printar_lab();
    int i;
	for(i = 0; i < 50000; i++){
		mutacao();
		crossover();
		pontuacao();
		if(pontos[0].pontuacao < PONT_FINAL){
			printar_lab_caminho();
			printf("Caminho encontrado na %d geracao\n", i);
			//printf("Pontuacao: %d\n", pontos[0].pontuacao);

			break;
		}
		if(i % 100 == 0){
		    system("cls");
			printf("geracao: %d\n", i);
			mover_pessoa(); // best
			printar_lab();
			//resetar_pessoa();
		}
	}
}
