#ifndef DEFAULT_H
#define DEFAULT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <termios.h>
#include <time.h>
#include <assert.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 512
#define PORT 9004
#define NAME_SIZE 40

#define up 'w'
#define down 's'
#define left 'a'
#define right 'd'

#define qnt_menu 3 // quantidade de opções no menu principal
#define menu_positions 17, 19, 21 // linhas de cada opção do menu principal
#define qnt_options 6 // quantidade de opções no menu de opções
#define options_positions 17, 19, 21, 23, 25, 27 // linhas de cada opção no menu de opções

#define qnt_lose 2 // quantidade de opções da tela de game over
#define lose_positions 15, 17 // linhas de cada opção da tela de game over
#define qnt_win 2 // quantidade de opções da tela de you won
#define win_positions 15, 17 // linhas de cada opção da tela de you won

#define cursor_pos 22 // coluna da seta >

#define CNRM  "\x1B[0m"
#define CRED  "\x1B[91m"
#define CGRN  "\x1B[92m"
#define CYEL  "\x1B[93m"
#define CBLU  "\x1B[94m"
#define CMAG  "\x1B[95m"
#define CWHT  "\x1B[97m"

#define FRED  "\x1B[101m"
#define FGRN  "\x1B[102m"
#define FYEL  "\x1B[103m"
#define FBLU  "\x1B[104m"
#define FMAG  "\x1B[105m"
#define FCYN  "\x1B[106m"
#define FWHT  "\x1B[107m"

#define MAX_CLIENTS 3
#define MAX_MONSTERS 10

#define STATSMAX 250

#define PLAYER_HP 150
#define PLAYER_ATK 55
#define PLAYER_DEF 45

#define asteristico 1 // esses números são usados para proporção, exemplo asteristico 1 de 5 = 20%
#define espaco 4 // espaco 4 de 5 = 80%
#define maxrepetidos 5 //numero max de vezes que um caractere pode ser repetido em sequncia

enum {
	MAIN,
	OPTIONS,
	LOSE,
	WIN
};

struct clientInfo {
	char name[NAME_SIZE];
	int map;
	int hp_max;
	int atk;
	int def;
};

struct player_data {
	char name[NAME_SIZE];
	int x, y;
	int hp, max_hp, atk, def, turn; // int informações do game, hp, ataque, defesa, pontos...
	int fight, whofight;
	char sprite;
	int sockid;
};

/*
	fight
	= 0 se não está lutando
	= 1 se lutando contra monstro, e whofight = id do monstro
	= 2 se lutando contra outro player, e whofight = id do player, turn diz se é a vez de tal client
*/

struct map_data {
	int height;
	int width;
	int qnt_monsters;
	char map[100][100];
};

struct mov_msg {
	char msg;
	int upd_monsters;
};

// mensagem de atualização do jogo
struct upd_msg {
	int type;
	int id;
	int x, y;
	int ismonster;
	int hp, max_hp, atk, def;
	int fight, whofight;
	char sprite;
};

/*
	tipo
	= 0 atualiza o mapa, (x,y) é a  posição, e dir é o novo char. Se dir == -1,  vida é a quantidade de clientes conectados
	= 1 mensagem para avisar que morreu
	= 2 mensagem para avisar que venceu
	= 3 mensagem para dizer qual o mapa que vai ser jogado (id = num do mapa)
	= 4 mensagem para inicializar os vetores de players e monstros
*/

typedef struct map_data map_data;
typedef struct clientInfo clientInfo;
typedef struct client_data client_data;
typedef struct mov_msg mov_msg;
typedef struct upd_msg upd_msg;
typedef struct player_data player_data;
#endif
