#include "default.h"

typedef struct {
	char sprite;
	char *color;
	int x;
	int y;
} updt_player; // MUDAR NOME!!

MAP field;

updt_player players[MAX_CLIENTS];
updt_player monsters[MAX_MONSTERS];

int qnt_clients;

int network_socket;
struct sockaddr_in server_address;

void delay (unsigned int secs);

char *color(int id);
void printcchar(char *color,char c);
void drawall();

int islegal(int x, int y, char c);

int mod(int x, int m);

void menu(clientInfo *info);

void connectToServer(const char *server_IP);

int sendMovToServer(mov_msg message);

int readTxtFromServer(char* message);

int getch();

int sendInfoToServer(clientInfo info);

int readUpdFromServer(upd_msg* message);