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

#define qtdmenu 3 // quantidade de opcoes no menu principal
#define opcoesmenu 20, 22, 24 // linhas de cada opcao do menu principal
#define qtdop 5 // quantidade de opcoes no menu de opcoes
#define opcoesoptions 20, 22, 24, 26, 28 // linhas de cada opcao no menu de opcoes
#define colseta 22 // coluna da seta >

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

struct MAP {
	int linha;
	int coluna;
	int monstros;
	char mapa[100][100];
};

struct mov_msg {
	char msg;
};

struct upd_msg {//Mensagem de atualização do jogo
	int tipo, id, vida;
	int x, y;
	char new;
	char sprite;
};

/*
	tipo
	= 0 atualiza o mapa, (x,y) é a  posição, e new é o novo char. Se new == -1,  vida é a quantidade de clientes conectados
	= 1 mesagem para avisar que vai entrar em batalha/atualiza status do oponente vida é a vida do oponente, x é o ataque, y é a defesa
	= 2 atualiza a meus status, vida é a minha nova vida, x é meu novo ataque, y é minha nova defesa
	= 3 mensagem para avisar que saiu de batalha
	= 4 mensagem para avisar que morreu
	= 5 mensagem para avisar que venceu
	= 6 mensagem para dizer qual o mapa que vai ser jogado (id = num do mapa)
*/

struct clientInfo{
	int mapa;
	char nome[NAME_SIZE];
};

typedef struct MAP MAP;
typedef struct clientInfo clientInfo;
typedef struct mov_msg mov_msg;
typedef struct upd_msg upd_msg;
#endif