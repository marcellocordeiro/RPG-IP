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

#define qnt_menu 3 // quantidade de opcoes no menu principal
#define menu_positions 20, 22, 24 // linhas de cada opcao do menu principal
#define qnt_options 5 // quantidade de opcoes no menu de opcoes
#define options_positions 20, 22, 24, 26, 28 // linhas de cada opcao no menu de opcoes
#define cursor_pos 22 // coluna da seta >

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KGRY  "\x1B[90m"

#define MAX_CLIENTS 3
#define MAX_MONSTERS 10

#define PLAYER_HP 100 // player lvl 1
#define PLAYER_ATK 20
#define PLAYER_DEF 10

#define MONSTER_HP 50 // monstro lvl 1
#define MONSTER_ATK 10
#define MONSTER_DEF 5

struct clientInfo {
	int mapa;
	char nome[NAME_SIZE];
};

struct player_data { // MUDAR NOME!! (ainda?)
	int x, y;
	int ismonster;
	int hp, max_hp, atk, def, turn; // int informações do game, hp, ataque, defesa, pontos...
	int sockid, fight, whofight;
	char nome[NAME_SIZE];
	char sprite;
	char *color;
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

struct upd_msg { //Mensagem de atualização do jogo
	int type;
	int id;
	int x, y, hp;
	int fight, whofight;
	int ismonster;
	char dir;
	char sprite;
};

/*
	tipo
	= 0 atualiza o mapa, (x,y) é a  posição, e dir é o novo char. Se dir == -1,  vida é a quantidade de clientes conectados
	= 1 mesagem para avisar que vai entrar em batalha/atualiza status do oponente vida é a vida do oponente, x é o ataque, y é a defesa
	= 2 atualiza a meus status, vida é a minha nova vida, x é meu novo ataque, y é minha nova defesa
	= 3 mensagem para avisar que saiu de batalha
	= 4 mensagem para avisar que morreu
	= 5 mensagem para avisar que venceu
	= 6 mensagem para dizer qual o mapa que vai ser jogado (id = num do mapa)
	= 6 mensagem para inicializar os vetores de players e monstros
*/

typedef struct map_data map_data;
typedef struct clientInfo clientInfo;
typedef struct client_data client_data;
typedef struct mov_msg mov_msg;
typedef struct upd_msg upd_msg;
typedef struct player_data player_data;
#endif