#include "lib/server.h"

void MyClientConnected(int id, clientInfo startInfo); // função que é chamada quando um cliente é connectado
void MyClientMoved(int id, mov_msg mov);// função que é chamada quando cleinte manda mensagem de movimento
void startGame(void);// função que é chamada quando cliente 0 confirma o inicio do jogo
void MyBroadcast(char *str);// um exemplo de como mandar uma mensagem para todos os usuários

int main () {
	srand(time(NULL)); // seed pra usar o rand durante o jogo

	clientMoved = MyClientMoved;
	clientConnected = MyClientConnected;
	clientConfirmed = startGame;

	init();

	while (1) {
		// espera alguma mensagem
		sleepServer();

		// verifando se foi uma conexão..
		checkConn();

		// se foi alguém se comunicando...
		wasClient();

		//broadcasting das modificações do mapa
		if(game_status == 2)
			broadcast();
	}
}

void MyClientConnected(int id, clientInfo startInfo){
	printf("Client %s connected, id = %d, map = %d\n", startInfo.nome, id, startInfo.mapa);
	if (id == 0)
		game_status = 1;
}

void MyClientMoved(int id, mov_msg mov){
	usleep(100); // verificado experimentalmente que melhora a dinâmica do jogo
	printf("Client %d moved: %c\n", id, mov.msg);
	map_changes[pos_broad].tipo = 0;
	map_changes[pos_broad].new = mov.msg;
	map_changes[pos_broad].x = clients[id].x;
	map_changes[pos_broad].y = clients[id].y;
	map_changes[pos_broad].id = id;
	pos_broad++;

	// assumindo que o movimento e legal
	if (mov.msg == up)
		clients[id].x--;
	else if (mov.msg == down)
		clients[id].x++;
	else if (mov.msg == left)
		clients[id].y--;
	else if (mov.msg == right)
		clients[id].y++;
}

void startGame(){
	printf("Client 0 confirmed, the game will start now...\n");
	MyBroadcast("1  - O jogo vai comecar");
	game_status = 2;
	// avisar para os clientes que o jogo vai começar
	// enviar algumas informações, como mapa, status inicial do cliente, etc..
}

void MyBroadcast(char *str){
	int id;
	
	for(id = 0; id < MAX_CLIENTS; id++)
		if(clients[id].sockid > 0)
			sendTxtToClient(clients[id].sockid, str);
}