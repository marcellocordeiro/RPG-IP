#include "default.h"

// VARIÁVEIS GLOBAIS
map_data map;

player_data players[MAX_CLIENTS];
player_data monsters[MAX_MONSTERS];

int qnt_clients;

int network_socket;
struct sockaddr_in server_address;

// INTERFACES
char mainmenu[50][110], options[50][110];
char battle[50][110], lose[50][110], win[50][110];

int main_height, options_height;
int battle_height, lose_height, win_height;


// FUNÇÕES
void game();
int firstAlive();
int loadFile(char *filename, char frame[50][110]);
void loadMap(char *filename);
void readUpd(upd_msg upd);
void delay(unsigned int secs);
char* color(int id);
void printHpBar(int hp, int max_hp);
void printChar(char *color, char c);
void printMap();
void printMenu(char menu[50][110], int height);
int islegal(int x, int y, char sprite, char c);
int mod(int x, int m);
void loadAll ();
void menu(int draw);
void createRandomMap(clientInfo *info);

int sendInfoToServer(clientInfo info);
void connectToServer(const char *server_IP);
int readTxtFromServer(char* message);
int readUpdFromServer(upd_msg* message);
int sendMovToServer(mov_msg message);
int getch();