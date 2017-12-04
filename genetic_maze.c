#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define LIMITE_MOVIMENTACAO 25
#define MAZE_X 10
#define MAZE_Y 10

/*
Definição das funções para funcionamento do software
*/


/* Parametros sobre a população */
#define TAM_POPULACAO 500
#define BEST_CNT 7
#define REPRODUCTIVE 150
#define PENALTY_VAL 2
#define MUTATION_CHANCE 0.5f
#define FINAL_SCORE 3


typedef struct{
	int x;
	int y;
}ponto_t;

typedef struct{
	int score;
	int index;
}pontuacao_t;



void print_maze();
void print_maze_path();

void player_reset();
void player_stay();
void player_move_up();
void player_move_down();
void player_move_left();
void player_move_right();
void fill();
int* sort(int* array, int size);
void sort_pontuacao_t();
void cross();
int fitness(int chromo);
int score();
void move_player();
void mutate();



int array[TAM_POPULACAO][LIMITE_MOVIMENTACAO];
int best_spec[BEST_CNT][LIMITE_MOVIMENTACAO];

ponto_t finish_pos;
ponto_t player;
int penalty;

pontuacao_t scores[TAM_POPULACAO];

void (*func_ptr[5])() = {player_stay, player_move_up, player_move_down, player_move_left, player_move_right};


int maze[MAZE_X][MAZE_Y] = {	{'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
								{'#', ' ', ' ', ' ', '#', ' ', '#', ' ', ' ', '#'},
								{'#', ' ', '#', ' ', ' ', ' ', '#', ' ', '#', '#'},
								{'#', ' ', '#', ' ', '#', '#', '#', ' ', ' ', '#'},
								{'#', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#', '#'},
								{'#', ' ', ' ', ' ', '#', ' ', '#', ' ', '#', '#'},
								{'#', ' ', '#', ' ', '#', ' ', '#', ' ', ' ', '#'},
								{'#', '#', '#', ' ', '#', ' ', '#', '#', ' ', '#'},
								{'#', ' ', '#', ' ', ' ', '#', '#', '#', ' ', '#'},
								{'#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}};


void print_maze_path(){
	//int cnt = 0;
	char temp[MAZE_X][MAZE_Y] = {};
	memcpy(temp, maze, MAZE_X*MAZE_Y);
	int i;
	for(i = 0; i < LIMITE_MOVIMENTACAO; i++){
		if(array[scores[0].index][i]){
			func_ptr[array[scores[0].index][i]](); // best score path
			maze[player.x][player.y] = '*';//(char)(i+97);
			//cnt++;
		}
	}
    int x,y;
	for(x = 0; x < MAZE_X; x++){
		for(y = 0; y < MAZE_Y; y++){
			if(player.x == x && player.y == y)
				printf(" P ");
			else if(finish_pos.x == x && finish_pos.y == y)
				printf(" F ");
			else
				printf(" %c ", maze[x][y]);

		}
		printf("\r\n");
	}
	player_reset();
	memcpy(maze, temp, MAZE_X*MAZE_Y);
	printf("\r\n");
	//printf("Goal reached in %d moves\r\n", cnt);
}

void print_maze(){
    int x,y;
	for(x = 0; x < MAZE_X; x++){
		for(y = 0; y < MAZE_Y; y++){
			if(player.x == x && player.y == y)
				printf(" P ");
			else if(finish_pos.x == x && finish_pos.y == y)
				printf(" F ");
			else
				printf(" %c ", maze[x][y]);

		}
		printf("\r\n");
	}
	printf("\r\n");
}

void player_reset(){
	player.x = 1;
	player.y = 1;
	penalty = 0;
}

void player_stay(){
	// add penalty?
	// penalty += 1;
	return;
}
void player_move_up(){
	if(maze[player.x-1][player.y] == '#'){
		penalty += PENALTY_VAL;
		return;
	}
	player.x -= 1;
}

void player_move_down(){
	if(maze[player.x+1][player.y] == '#'){
		penalty += PENALTY_VAL;
		return;
	}
	player.x += 1;
}

void player_move_left(){
	if(maze[player.x][player.y-1] == '#'){
		penalty += PENALTY_VAL;
		return;
	}
	player.y -= 1;
}

void player_move_right(){
	if(maze[player.x][player.y+1] == '#'){
		penalty += PENALTY_VAL;
		return;
	}
	player.y += 1;
}



void fill(){
    int x,y;
	for(x = 0; x < TAM_POPULACAO; x++){
		for(y = 0; y < LIMITE_MOVIMENTACAO; y++){
			array[x][y] = rand() % 5;
		}
	}
}

// bubble sort
int* sort(int* array, int size){
    int x,i;
	for(x= 0; x < size - 1; x++){
		for(i = 0; i < size - x - 1; i++){
			if(array[i] > array[i+1]){
				int c = array[i];
				array[i+1] = array[i];
				array[i] = c;
			}
		}
	}
	return array;
}

void sort_pontuacao_t(){
    int x,i;
	for(x = 0; x < TAM_POPULACAO - 1; x++){
		for(i = 0; i < TAM_POPULACAO - x - 1; i++){
			if(scores[i].score > scores[i+1].score){
			    pontuacao_t temp = scores[i];
			    scores[i] = scores[i+1];
			    scores[i+1] = temp;
			}
		}
	}
}

void cross() {
	//copy to best_spec array
	int i;
	for (i = 0; i < BEST_CNT; i++) {
		memcpy(best_spec[i], array[scores[i].index], LIMITE_MOVIMENTACAO);
	}

	int firstParent = rand() % BEST_CNT;
	int secondParent = rand() % BEST_CNT;
	int crossOver = rand() % LIMITE_MOVIMENTACAO;

	for (i = BEST_CNT + REPRODUCTIVE; i < TAM_POPULACAO; i++) {
		memcpy(array[i], best_spec[firstParent], crossOver);
		memcpy(array[i], best_spec[secondParent], (LIMITE_MOVIMENTACAO - crossOver));
	}

}

int fitness(int chromo){
    int i;
	for(i = 0; i < LIMITE_MOVIMENTACAO; i++){
		func_ptr[array[chromo][i]]();
	}

	int score = abs(finish_pos.x - player.x) + abs(finish_pos.y - player.y) + penalty;

	player_reset();
	return score;
}

int score(){
    int i;
	for(i = 0; i < TAM_POPULACAO; i++){
		scores[i].score = fitness(i);
		scores[i].index = i;
		//printf("%d %d\n", scores[i].score, scores[i].index);
	}
	sort_pontuacao_t();
	return scores[0].index;
}
//

void move_player(){
    int i;
	for(i = 0; i < LIMITE_MOVIMENTACAO; i++){
		func_ptr[array[scores[0].index][i]]();
	}
}

void mutate(){
    int i;
	for(i = 0; i < TAM_POPULACAO-1; i++){
		if(MUTATION_CHANCE > (double)rand() / (double)RAND_MAX){ // random number (0,1)
			array[i][rand() % 30] = rand() % 5; // chhose random element and mutate
		}
	}
}

int main(){
	time_t t;
	/* Intializes random number generator */
	srand((unsigned) time(&t));

	player.x = 1;
	player.y = 1;
	finish_pos.x = 8;
	finish_pos.y = 8;
	fill();
	printf("Iniciar labirinto: \r\n");
	print_maze();
    int i;
	for(i = 0; i < 50000; i++){
		mutate();
		cross();
		score();
		if(scores[0].score < FINAL_SCORE){
			print_maze_path();
			printf("Caminho encontrado na %d generacao\n", i);
			printf("Score: %d\n", scores[0].score);

			break;
		}
		if(i % 100 == 0){
			printf("geracao: %d\n", i);
			//move_player(); // best
			//print_maze();
			//player_reset();
		}
	}
}
