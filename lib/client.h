#include "default.h"

/// VARIÁVEIS GLOBAIS
map_data map;

player_data players[MAX_CLIENTS];
player_data monsters[MAX_MONSTERS];

int qnt_clients;

int network_socket;
struct sockaddr_in server_address;


/// FUNÇÕES
void delay(unsigned int secs);
char* color(int id);
void printcchar(char *color,char c);
void drawall();
void drawmenus(char menu[100][100], int height, int width);
int islegal(int x, int y, char sprite, char c);
int mod(int x, int m);
void menu(clientInfo *info);
int createRandomMap();

int sendInfoToServer(clientInfo info);
void connectToServer(const char *server_IP);
int readTxtFromServer(char* message);
int readUpdFromServer(upd_msg* message);
int sendMovToServer(mov_msg message);
int getch();