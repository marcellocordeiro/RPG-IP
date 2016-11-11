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
	int i, found = 0;
	usleep(100); // verificado experimentalmente que melhora a dinâmica do jogo
	//printf("Client %d moved: %c\n", id, mov.msg); // debug

	if (mov.upd_monsters == 1) { // update dos monstros
		monsterMove();
		return;
	}

	if (clients[id].fight) {
		//sendBattleUpdate(clients[id].fight, clients[id].whofight);
		return;
	}

	if (islegal(clients[id].x, clients[id].y, clients[id].sprite, mov.msg) == 0)
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
		clients[id].fight = 1;
		clients[id].whofight = 1; // id do monstro!!!

		map_changes[pos_broad].type = 1;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = clients[id].x;
		map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].hp = clients[id].hp;
		map_changes[pos_broad].fight = clients[id].fight;
		map_changes[pos_broad].whofight = clients[id].whofight;
		map_changes[pos_broad].ismonster = 0;
		map_changes[pos_broad].sprite = clients[id].sprite;
		//pos_broad++; ///////////////// PQ COMENTASSE ISSO?
		sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);

	}
	else {
		map_changes[pos_broad].type = 0;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = clients[id].x;
		map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].hp = clients[id].hp;
		map_changes[pos_broad].fight = clients[id].fight;
		map_changes[pos_broad].whofight = clients[id].whofight; // PRECISA?
		map_changes[pos_broad].ismonster = 0;
		map_changes[pos_broad].dir = mov.msg;
		map_changes[pos_broad].sprite = clients[id].sprite;
		pos_broad++;

		/*
		map_changes[pos_broad].type;
		map_changes[pos_broad].id;
		map_changes[pos_broad].x, 
		map_changes[pos_broad].y
		map_changes[pos_broad].hp;
		map_changes[pos_broad].fight, 
		map_changes[pos_broad].whofight;
		map_changes[pos_broad].ismonster;
		map_changes[pos_broad].dir;
		map_changes[pos_broad].sprite;
		*/

		/*
		clients[id].x;
		clients[id].y;
		clients[id].ismonster;
		clients[id].hp;
		clients[id].max_hp;
		clients[id].ataque;
		clients[id].defesa;
		clients[id].turn;
		clients[id].fight;
		clients[id].whofight;
		clients[id].nome[NAME_SIZE];
		clients[id].sprite;
		clients[id].*color;
		*/
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
		map_changes[pos_broad].type = 7;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = clients[id].x;
		map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].hp = clients[id].hp;
		map_changes[pos_broad].fight = clients[id].fight;
		map_changes[pos_broad].whofight = clients_connected; // passar a quantidade de players
		map_changes[pos_broad].ismonster = clients[id].ismonster;
		map_changes[pos_broad].dir = -1;
		map_changes[pos_broad].sprite = clients[id].sprite;
		pos_broad++;
	}

	// status inicial dos monstros
	initMonsters();
	for (id = 0; id < map.qnt_monsters; id++) {
		map_changes[pos_broad].type = 7;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = monsters[id].x;
		map_changes[pos_broad].y = monsters[id].y;
		map_changes[pos_broad].hp = monsters[id].hp;
		map_changes[pos_broad].fight = monsters[id].fight;
		map_changes[pos_broad].whofight = monsters[id].whofight;
		map_changes[pos_broad].ismonster = monsters[id].ismonster;
		map_changes[pos_broad].dir = -2;
		map_changes[pos_broad].sprite = monsters[id].sprite;
		pos_broad++;
	}

/*
	for (id = 0; id < clients_connected; id++) { // enviar algumas informações, como mapa, status inicial do cliente, etc..
		clients[id].x = rand()%(map.height - 2) + 1;
		clients[id].y = rand()%(map.width - 2) + 1;
		clients[id].ismonster = 0;
		clients[id].hp = MAX_HP;
		clients[id].max_hp = MAX_HP;
		//clients[id].ataque = ??;
		//clients[id].defesa = ??;
		//clients[id].turn = ??;
		clients[id].fight = 0;
		clients[id].whofight = 0;
		clients[id].sprite = '^';
		//clients[id].*color; // definir aqui?


		map_changes[pos_broad].type = 7;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = clients[id].x;
		map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].hp = clients[id].hp;
		map_changes[pos_broad].fight = clients[id].fight;
		map_changes[pos_broad].whofight = clients_connected; // passar a quantidade de players
		map_changes[pos_broad].ismonster = clients[id].ismonster;
		map_changes[pos_broad].dir = -1;
		map_changes[pos_broad].sprite = clients[id].sprite;
		pos_broad++;
	}

	for (id = 0; id < map.qnt_monsters; id++) {
		monsters[id].x = rand()%(map.height - 2) + 1;
		monsters[id].y = rand()%(map.width - 2) + 1;
		monsters[id].ismonster = 1;

		monsters[id].hp = MAX_HP;
		monsters[id].max_hp = MAX_HP;
		//monsters[id].ataque = ??;
		//monsters[id].defesa = ??;
		//monsters[id].turn = ??;
		monsters[id].sprite = 'm';
		monsters[id].fight = 0;

		map_changes[pos_broad].type = 7;
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = monsters[id].x;
		map_changes[pos_broad].y = monsters[id].y;
		map_changes[pos_broad].hp = monsters[id].hp;
		map_changes[pos_broad].fight = monsters[id].fight;
		map_changes[pos_broad].whofight = monsters[id].whofight;
		map_changes[pos_broad].ismonster = monsters[id].ismonster;
		map_changes[pos_broad].dir = -2;
		map_changes[pos_broad].sprite = monsters[id].sprite;
		pos_broad++;
	}
*/
}

void MyBroadcast(char *s){
	int id;
	
	for(id = 0; id < MAX_CLIENTS; id++)
		if(clients[id].sockid > 0)
			sendTxtToClient(clients[id].sockid, s);
}