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
	FILE *fpmap;
	int i;
	char str[1];
	char map_name[16];

	printf("Client %s connected, id = %d, map = %d\n", startInfo.nome, id, startInfo.mapa);
	game_status = 1;

	str[0] = id + '0';

	sendTxtToClient(clients[id].sockid, str);

	if (id == 0) {
		// mandar o número do mapa para os clients
		map_changes[pos_broad].type = 6;
		map_changes[pos_broad].id = startInfo.mapa;
		pos_broad++;

		strcpy(map_name, "data/mapa");
		sprintf(map_name, "%s%d.txt", map_name, startInfo.mapa);

		fpmap = fopen(map_name, "rt");
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
	int i, found = -1, found2 = -1;
	usleep(100); // verificado experimentalmente que melhora a dinâmica do jogo
	//printf("Client %d moved: %c\n", id, mov.msg); // debug

	if (mov.upd_monsters == 1) { // update dos monstros
		monsterMove();
		return;
	}

	if (clients[id].fight) {
		if (clients[id].turn)
			battleUpd(id, mov.msg);
		return;
	}

	if (islegal(clients[id].x, clients[id].y, clients[id].sprite, mov.msg) == 0)
		return;

	// update dos players
	switch (mov.msg) {
		case up:
			if (clients[id].sprite == '^') {
				// encontrar monstro
				found = findMonster(clients[id].x - 1, clients[id].y);
	
				// encontrar player
				if (found == -1)
					found2 = findPlayer(clients[id].x - 1, clients[id].y);
			}
	
			// alterar posição
			if (found == -1 && found2 == -1)
				if (clients[id].sprite == '^')
					clients[id].x--;
	
			clients[id].sprite = '^';
			break;

		case down:
			if (clients[id].sprite == 'v') {
				// encontrar monstro
				found = findMonster(clients[id].x + 1, clients[id].y);
		
				// encontrar player
				if (found == -1)
					found2 = findPlayer(clients[id].x + 1, clients[id].y);
			}
	
			// alterar posição
			if (found == -1 && found2 == -1)
				if (clients[id].sprite == 'v')
					clients[id].x++;
	
			clients[id].sprite = 'v';
			break;

		case left:
			if (clients[id].sprite == '<') {
				// encontrar monstro
				found = findMonster(clients[id].x, clients[id].y - 1);
		
				// encontrar player
				if (found == -1)
					found2 = findPlayer(clients[id].x, clients[id].y - 1);
			}
	
			// alterar posição
			if (found == -1 && found2 == -1)
				if (clients[id].sprite == '<')
					clients[id].y--;
	
			clients[id].sprite = '<';
			break;

		case right:
			if (clients[id].sprite == '>') {
				// encontrar monstro
				found = findMonster(clients[id].x, clients[id].y + 1);
		
				// encontrar player
				if (found == -1)
					found2 = findPlayer(clients[id].x, clients[id].y + 1);
			}
	
			// alterar posição
			if (found == -1 && found2 == -1)
				if (clients[id].sprite == '>')
					clients[id].y++;
			
			clients[id].sprite = '>';
			break;
	}

	map.map[clients[id].x][clients[id].y] = ' ';

	if (found != -1) { // flag de batalha
		clients[id].fight = 1;
		clients[id].whofight = found; // id do monstro!!! --> sim!
		clients[id].turn = 1;

		monsters[found].fight = 1;
		monsters[found].whofight = id;
		monsters[found].turn = 0;

		map_changes[pos_broad] = buildUpd(id, 0);
		map_changes[pos_broad].type = 0;
		pos_broad++;

		map_changes[pos_broad] = buildUpd(found, 1);
		map_changes[pos_broad].type = 0;
		pos_broad++;
	}
	else if (found2 != -1) {
		clients[id].fight = 2;
		clients[id].whofight = found2; // id do monstro!!! --> sim!
		clients[id].turn = 1;

		clients[found2].fight = 2;
		clients[found2].whofight = id;
		clients[found2].turn = 0;

		map_changes[pos_broad] = buildUpd(id, 0);
		map_changes[pos_broad].type = 0;
		pos_broad++;

		map_changes[pos_broad] = buildUpd(found2, 0);
		map_changes[pos_broad].type = 0;
		pos_broad++;
	}
	else {
		map_changes[pos_broad] = buildUpd(id, 0);
		map_changes[pos_broad].type = 0;
		pos_broad++;
	}
}

void startGame(){
	int id;
	long int oldtime;

	printf("Client 0 confirmed, the game will start now...\n");
	broadcastTxt("O JOGO VAI COMEÇAR", -1); // avisar para os clientes que o jogo vai começar
	game_status = 2;

	oldtime = ((long int) time(NULL));
	srand(time(NULL));

	// status inicial dos clientes
	initClients();

	for (id = 0; id < clients_connected; id++) {
		map_changes[pos_broad] = buildUpd(id, 0);
		map_changes[pos_broad].type = 7;
		map_changes[pos_broad].whofight = clients_connected; // passar a quantidade de players
		map_changes[pos_broad].dir = -1;
		pos_broad++;
	}

	// status inicial dos monstros
	initMonsters();

	for (id = 0; id < map.qnt_monsters; id++) {
		map_changes[pos_broad] = buildUpd(id, 1);
		map_changes[pos_broad].type = 7;
		map_changes[pos_broad].dir = -2;
		pos_broad++;
	}
}

void MyBroadcast(char *s){
	int id;
	
	for(id = 0; id < MAX_CLIENTS; id++)
		if(clients[id].sockid > 0)
			sendTxtToClient(clients[id].sockid, s);
}