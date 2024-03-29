#include "lib/server.h"

int main () {
	srand(time(NULL)); // seed pra usar o rand durante o jogo
	rand();

	init();

	while (1) {
		// espera alguma mensagem
		sleepServer();

		// verificando se foi uma conexão..
		checkConn();

		// se foi alguém se comunicando...
		wasClient();

		// broadcast das modificações do mapa
		if (game_status == 2)
			broadcast();
	}
}

void clientConnected (int id, clientInfo startInfo) {
	FILE *fpmap;
	int i;
	char str[1];
	char map_name[16];

	strcpy(clients[id].name, startInfo.name);

	printf("Client %s connected, id = %d, map = %d\n", startInfo.name, id, startInfo.map);
	game_status = 1;

	str[0] = id + '0';

	sendTxtToClient(clients[id].sockid, str);

	if (id == 0) {
		// mandar o número do mapa para os clients
		map_changes[pos_broad].type = 3;
		map_changes[pos_broad].id = startInfo.map;
		pos_broad++;

		sprintf(map_name, "data/map%d.txt", startInfo.map);

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

	// recebe os stats do player que se conectou e coloca as informações no vetor de clients
	clients[id].hp = startInfo.hp_max;
	clients[id].max_hp = startInfo.hp_max;
	clients[id].atk = startInfo.atk;
	clients[id].def = startInfo.def;
}

void clientMoved (int id, mov_msg mov) {
	usleep(100); // verificado experimentalmente que melhora a dinâmica do jogo
	int i, found = -1, found2 = -1;
	//printf("Client %d moved: %c\n", id, mov.msg); // debug
	
	// update dos monstros
	if (mov.upd_monsters == 1) {
		monsterMove();
		return;
	}

	// update da batalha
	if (clients[id].fight && clients[id].turn) {
		battleUpd(id, mov.msg);
		return;
	}

	// verifica se o movimento é legal
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
				else
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
				else
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
				else
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
				else
					clients[id].sprite = '>';
			break;
	}

	if (map.map[clients[id].x][clients[id].y] == '*') { // player comeu uma árvore
		clients[id].hp += 10; // aumenta o hp em 10
		map.map[clients[id].x][clients[id].y] = ' '; // deleta a árvore do mapa

		if (clients[id].hp > clients[id].max_hp) // não deixa o hp passar de max_hp
			clients[id].hp = clients[id].max_hp;
	}

	if (found != -1 && !monsters[found].fight) { // encontrou um monstro
		clients[id].fight = 1;
		clients[id].whofight = found; // id do monstro!!! --> sim!
		clients[id].turn = 1; // o turno é sempre do player

		monsters[found].fight = 1;
		monsters[found].whofight = id; // id do player

		buildUpd(id, 0, 0); // broadcast das modificações
		buildUpd(found, 1, 0); // broadcast das modificações
	}
	else if (found2 != -1 && !clients[found2].fight) { // encontrou outro player
		clients[id].fight = 2;
		clients[id].whofight = found2; // id do oponente
		clients[id].turn = 1; // o primeiro ataque é de quem começou a batalha

		clients[found2].fight = 2;
		clients[found2].whofight = id; // id do player que iniciou a batalha
		clients[found2].turn = 0;

		buildUpd(id, 0, 0); // broadcast das modificações
		buildUpd(found2, 0, 0); // broadcast das modificações
	}
	else
		buildUpd(id, 0, 0); // broadcast padrão das modificações
}

void startGame(){
	int id;
	char startMsg[40];

	// status inicial dos clientes
	initClients();
	for (id = 0; id < clients_connected; id++) // broadcast dos stats dos players
		buildUpd(id, 0, 4);

	// status inicial dos monstros
	initMonsters();
	for (id = 0; id < map.qnt_monsters; id++) // broadcast dos stats dos monstros
		buildUpd(id, 1, 4);

	qnt_total = clients_connected + map.qnt_monsters;
	sprintf(startMsg, "%d players conectados, começando o jogo!", clients_connected);

	printf("Client 0 confirmed, the game will start now...\n");
	broadcastTxt(startMsg, -1); // avisa para os clientes que o jogo vai começar
	game_status = 2;
}