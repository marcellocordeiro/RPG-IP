#include "default.h"

typedef struct {
	int linha;
	int coluna;
	int monstros;
	char mapa[100][100];
} MAP;

typedef struct {
	char sprite;
	char *color;
	int x;
	int y;
}updt_player; // MUDAR NOME!!

MAP field;

int network_socket;
struct sockaddr_in server_address;

char *color(int id);
void printcchar(char *color,char c);
void drawall(updt_player *hero, int qnt_clients, MAP f/*,MOB monster*/);

int islegal(int x, int y, char c);

int mod(int x, int m);

void menu(clientInfo *info);

void connectToServer(const char *server_IP);

int sendMovToServer(mov_msg message);

int readTxtFromServer(char* message);

int getch();

int sendInfoToServer(clientInfo info);

int readUpdFromServer(upd_msg* message);