#ifndef SERVER_H
#define SERVER_H

#include "default.h"

/// VARIÁVEIS GLOBAIS
map_data map;

player_data clients[MAX_CLIENTS]; // vetor com clientes connectados e suas informações
int clients_connected; // quantidade de clientes connectados

player_data monsters[MAX_MONSTERS];

int qnt_total; // qnt de clientes + qnt de monstros

int game_status;

/* 
	game status 
	= 0 nenhum player conectado
	= 1 esperando confirmação do player de id == 0
	= 2 o jogo está em andamento
*/

int pos_broad; // posição livre no vetor do broadcast declarado abaixo
upd_msg map_changes[BUFFER_SIZE]; // vetor com as alterações do mapa

int sock; // id do socket do server

fd_set active_fd_set, read_fd_set; // variáveis auxiliares do server
struct sockaddr_in clientname;

/// FUNÇÕES
void clientConnected(int id, clientInfo startInfo); // função que é chamada quando um cliente é connectado
void clientMoved(int id, mov_msg mov); // função que é chamada quando um cliente manda mensagem de movimento
void startGame(); // função que é chamada quando cliente 0 confirma o inicio do jogo

void buildUpd(int id, int ismonster, int type);
int dmg(int atk, int def);
void battleUpd(int id, char move);
int findPlayer(int x, int y);
int findMonster(int x, int y);
int islegal(int x, int y, char sprite, char c);
int islegalMonster(int x, int y, char c);
void initClients();
int monsterStat(int min, int max);
void initMonsters();
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