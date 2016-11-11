#ifndef SERVER_H
#define SERVER_H

#include "default.h"

FILE *fpmap;
map_data map;

int sock; // id do socket do server

fd_set active_fd_set, read_fd_set; // variáveis auxiliares do server
struct sockaddr_in clientname;

player_data clients[MAX_CLIENTS]; // vetor com clientes connectados e suas informações
int clients_connected; // quantidade de clientes connectados

player_data monsters[MAX_MONSTERS];

int game_status;

/* 
	game status 
	= 0 se ngm conectou
	= 1 se esperando confirmação do "criador do game" 
	= 2 se jogo rodando
*/

int pos_broad; // posição livre no vetor do broadcast declarado abaixo
upd_msg map_changes[BUFFER_SIZE]; // vetor com as alterações do mapa

void MyClientConnected(int id, clientInfo startInfo); // função que é chamada quando um cliente é connectado
void MyClientMoved(int id, mov_msg mov);// função que é chamada quando cleinte manda mensagem de movimento
void startGame(void);// função que é chamada quando cliente 0 confirma o inicio do jogo
void MyBroadcast(char *s);// um exemplo de como mandar uma mensagem para todos os usuários

void (*clientMoved)(int, mov_msg);
void (*clientConnected)(int, clientInfo);
void (*clientDesconnected)(int);
void (*clientConfirmed)(void);

int islegal(int x, int y, char sprite, char c);
int findPlayer(int x, int y);
int islegalMonster(int x, int y, char c);
void monsterMove();

int makeSocket(uint16_t);
int readMovFromClient(int filedes, mov_msg *message);
int readClientInfo(int filedes, clientInfo *message);
void sendTxtToClient(int filedes, const char msg[]);
void sendUpdToClient(int filedes, upd_msg change);
void init();
void sleepServer();
void checkConn();
void wasClient();
void broadcast();
void broadcastTxt(const char msg[], int s);
void disconnectClient(int id);

#endif