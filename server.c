#include "lib/server.h"

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
		if (game_status == 2)
			broadcast();
	}
}

void MyClientConnected (int id, clientInfo startInfo) {
	int i;
	char str[1];
	char nome_mapa[16];

	printf("Client %s connected, id = %d, map = %d\n", startInfo.nome, id, startInfo.mapa);
	game_status = 1;

	str[0] = id + '0';

	sendTxtToClient(clients[id].sockid, str);

	if (id == 0) {
		map_changes[pos_broad].tipo = 6;
		map_changes[pos_broad].id = startInfo.mapa;
		pos_broad++;

		strcpy(nome_mapa, "data/mapa");
		sprintf(nome_mapa, "%s%d.txt", nome_mapa, startInfo.mapa);

		fpmap = fopen(nome_mapa, "rt");
			if (fpmap == NULL) {
				system("clear");
				printf("ERRO: MAPA NÃO ENCONTRADO\n");
				exit(1);
			}

		fscanf(fpmap, "%d %d %d", &map.height, &map.width, &map.qnt_monsters);

		for(i = 0; i < map.height; i++)
			fscanf(fpmap, " %[^\n]", map.map[i]);

		fclose(fpmap);
	}
}

void MyClientMoved (int id, mov_msg mov) {
	int i, found = 0;
	usleep(100); // verificado experimentalmente que melhora a dinâmica do jogo
	//printf("Client %d moved: %c\n", id, mov.msg); // debug

	if (mov.upd_monsters == 1) { // update dos monstros
		monsterMove();
		return;
	}

	if (clients[id].fight != 0) {
		//sendBattleUpdate(clients[id].fight, clients[id].whofight)
	}

	if (islegal(clients[id].x, clients[id].y, mov.msg) == 0)
		return;

	// update dos players
	if (mov.msg == up) {
		for (i = 0; i < map.qnt_monsters && !found; i++)
			if ((clients[id].x - 1 == monsters[i].x && clients[id].y == monsters[i].y) && clients[id].sprite == '^')
				found = 1;
	
		if (found == 0)
			if (clients[id].sprite == '^')
				clients[id].x--;

		clients[id].sprite = '^';
	}
	else if (mov.msg == down) {
		for (i = 0; i < map.qnt_monsters && !found; i++)
			if ((clients[id].x + 1 == monsters[i].x && clients[id].y == monsters[i].y) && clients[id].sprite == 'v')
				found = 1;

		if (found == 0)
			if (clients[id].sprite == 'v')
				clients[id].x++;

		clients[id].sprite = 'v';
	}
	else if (mov.msg == left) {
		for (i = 0; i < map.qnt_monsters && !found; i++)
			if ((clients[id].x == monsters[i].x && clients[id].y - 1 == monsters[i].y) && clients[id].sprite == '<')
				found = 1;

		if (found == 0)
			if (clients[id].sprite == '<')
				clients[id].y--;

		clients[id].sprite = '<';
	}
	else if (mov.msg == right) {
		for (i = 0; i < map.qnt_monsters && !found; i++)
			if ((clients[id].x == monsters[i].x && clients[id].y + 1 == monsters[i].y) && clients[id].sprite == '>')
				found = 1;

		if (found == 0)
			if (clients[id].sprite == '>')
				clients[id].y++;
		
		clients[id].sprite = '>';
	}

	map.map[clients[id].x][clients[id].y] = ' ';

	if (found == 1) { // flag de batalha
		//map_changes[pos_broad].tipo = 1;
		//clients[id].fight = 1;
		//sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);
	}
	else {
		map_changes[pos_broad].tipo = 0;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = clients[id].x;
		map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].dir = mov.msg;
		map_changes[pos_broad].sprite = clients[id].sprite;

		map_changes[pos_broad].ismonster = 0; // !!!!!!!!!!

		pos_broad++;
	}
}

void startGame(){
	int i, id;

	printf("Client 0 confirmed, the game will start now...\n");
	broadcastTxt("O JOGO VAI COMEÇAR", -1); // avisar para os clientes que o jogo vai começar
	game_status = 2;

	for (id = 0; id < clients_connected; id++) { // enviar algumas informações, como mapa, status inicial do cliente, etc..
		clients[id].x = rand()%(map.height - 2) + 1;
		clients[id].y = rand()%(map.width - 2) + 1;
		clients[id].sprite = '^';

		map_changes[pos_broad].tipo = 0;
		map_changes[pos_broad].x = clients[id].x;
		map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].dir = -1;
		map_changes[pos_broad].sprite = clients[id].sprite;
		map_changes[pos_broad].vida = clients_connected;
		map_changes[pos_broad].ismonster = 0;
		pos_broad++;
	}

	for (i = 0; i < map.qnt_monsters; i++) {
		monsters[i].x = rand()%(map.height - 2) + 1;
		monsters[i].y = rand()%(map.width - 2) + 1;
		monsters[i].sprite = 'm';
		monsters[i].fight = 0;

		map_changes[pos_broad].tipo = 0;
		map_changes[pos_broad].x = monsters[i].x;
		map_changes[pos_broad].y = monsters[i].y;
		map_changes[pos_broad].id = i;
		map_changes[pos_broad].dir = -2;
		map_changes[pos_broad].sprite = monsters[i].sprite;
		map_changes[pos_broad].ismonster = 1;
		pos_broad++;

		//printf("monsters[%d]:\nx:  %d y: %d  sprite: %c\n", i, monsters[i].x, monsters[i].y, monsters[i].sprite);
	}
}

void MyBroadcast(char *s){
	int id;
	
	for(id = 0; id < MAX_CLIENTS; id++)
		if(clients[id].sockid > 0)
			sendTxtToClient(clients[id].sockid, s);
}