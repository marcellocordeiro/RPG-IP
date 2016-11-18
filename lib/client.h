#include "default.h"

// VARIÁVEIS GLOBAIS
map_data map;

player_data players[MAX_CLIENTS];
player_data monsters[MAX_MONSTERS];

int qnt_clients;

int network_socket;
struct sockaddr_in server_address;

// FUNÇÕES
int firstAlive();
void game(char battle[30][110], char lose[30][110], char win[30][110]);
void loadFile(char *filename, char frame[30][110]);
void loadMap(char *filename);
void readUpd(upd_msg upd);
void delay(unsigned int secs);
char* color(int id);
void printcchar(char *color,char c);
void drawall();
void drawmenus(char menu[100][100], int height, int width);
int islegal(int x, int y, char sprite, char c);
int mod(int x, int m);
void menu(clientInfo *info);
void createRandomMap(clientInfo *info);

int sendInfoToServer(clientInfo info);
void connectToServer(const char *server_IP);
int readTxtFromServer(char* message);
int readUpdFromServer(upd_msg* message);
int sendMovToServer(mov_msg message);
int getch();