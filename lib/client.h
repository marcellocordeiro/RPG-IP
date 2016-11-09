#include "default.h"

typedef struct
{
	int linha;
	int coluna;
	int monstros;
	char **mapa;
}MAP;

MAP field;

int network_socket;
struct sockaddr_in server_address;

int islegal(int x, int y, char c);

int mod(int x, int m);

void menu(clientInfo *info);

void connectToServer(const char *server_IP);

int sendMovToServer(mov_msg message);

int readTxtFromServer(char* message);

int getch();

int sendInfoToServer(clientInfo info);

int readUpdFromServer(upd_msg* message);