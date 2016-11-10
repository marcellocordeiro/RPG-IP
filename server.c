#include "lib/server.h"

void MyClientConnected(int id, clientInfo startInfo); // função que é chamada quando um cliente é connectado
void MyClientMoved(int id, mov_msg mov);// função que é chamada quando cleinte manda mensagem de movimento
void startGame(void);// função que é chamada quando cliente 0 confirma o inicio do jogo
void MyBroadcast(char *s);// um exemplo de como mandar uma mensagem para todos os usuários

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
	char str[1];

	printf("Client %s connected, id = %d, map = %d\n", startInfo.nome, id, startInfo.mapa);
	game_status = 1;

	str[0] = id + '0';

	sendTxtToClient(clients[id].sockid, str);

	if (id == 0) {
		map_changes[pos_broad].tipo = 6;
		map_changes[pos_broad].id = startInfo.mapa;
		pos_broad++;
	}
}

void MyClientMoved(int id, mov_msg mov){
	int i, found = 0, x, y;
	usleep(100); // verificado experimentalmente que melhora a dinâmica do jogo
	//printf("Client %d moved: %c\n", id, mov.msg); // debug

	player monsters[10];
	monsters[0].x = 5;
	monsters[0].y = 5;
	int qnt_monsters = 1;

	map_changes[pos_broad].tipo = 0;
	map_changes[pos_broad].new = mov.msg;
	map_changes[pos_broad].x = clients[id].x;
	map_changes[pos_broad].y = clients[id].y;
	map_changes[pos_broad].id = id;
	
	x = clients[id].x;
	y = clients[id].y;

	if (clients[id].fight == 1) {
		//sendBattleUpdate(clients[id].fight, clients[id].whofight)
	}

	// assumindo que o movimento e legal
	if (mov.msg == up) {
		for (i = 0; i < qnt_monsters && !found; i++)
			if (x - 1 == monsters[i].x && y == monsters[i].y)
				found = 1;
	
		if (found == 0) {
			clients[id].x--;
			clients[id].sprite = '^';
		}
	}
	else if (mov.msg == down) {
		for (i = 0; i < qnt_monsters && !found; i++)
			if (x + 1 == monsters[i].x && y == monsters[i].y)
				found = 1;

		if (found == 0) {
			clients[id].x++;
			clients[id].sprite = 'v';
		}
	}
	else if (mov.msg == left) {
		for (i = 0; i < qnt_monsters && !found; i++)
			if (x == monsters[i].x && y - 1 == monsters[i].y)
				found = 1;

		if (found == 0) {
			clients[id].y--;
			clients[id].sprite = '<';
		}
	}
	else if (mov.msg == right) {
		for (i = 0; i < qnt_monsters && !found; i++)
			if (x == monsters[i].x && y + 1 == monsters[i].y)
				found = 1;

		if (found == 0) {
			clients[id].y++;
			clients[id].sprite = '>';
		}
	}

	if (found == 1 || clients[id].fight == 1) {
		map_changes[pos_broad].tipo = 1;
		clients[id].fight = 1;
		sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);
	}
	else { // podre
		map_changes[pos_broad].x = clients[id].x;
	map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].sprite = clients[id].sprite;
		pos_broad++;
	}


}

void startGame(){
	int id;

	printf("Client 0 confirmed, the game will start now...\n");
	broadcastTxt("O JOGO VAI COMEÇAR", -1); // avisar para os clientes que o jogo vai começar
	game_status = 2;

	for(id = 0; id < clients_connected; id++) { // enviar algumas informações, como mapa, status inicial do cliente, etc..
		clients[id].x = rand()%10 + 1;	//	clients[id].x = rand()%(field.linhaupdt.id) + 1;
		clients[id].y = rand()%10 + 1;  //	clients[id].y = rand()%(field.linha) + 1;
		clients[id].sprite = '^';

		map_changes[pos_broad].tipo = 0;
		map_changes[pos_broad].x = clients[id].x;
		map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].new = -1;
		map_changes[pos_broad].sprite = clients[id].sprite;
		map_changes[pos_broad].vida = clients_connected;
		pos_broad++;
	}
}

void MyBroadcast(char *s){
	int id;
	
	for(id = 0; id < MAX_CLIENTS; id++)
		if(clients[id].sockid > 0)
			sendTxtToClient(clients[id].sockid, s);
}