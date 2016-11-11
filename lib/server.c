#include "server.h"

int islegal (int x, int y, char sprite, char c) {
	switch (c) {
		case up:
			if (x - 1 == 0 && sprite == '^')
				return 0;

			break;
		case down:
			if (x + 1 == map.height - 1 && sprite == 'v')
				return 0;

			break;
		case left:
			if (y - 1 == 0 && sprite == '<')
				return 0;

			break;
		case right:
			if (y + 1 == map.width - 1 && sprite == '>')
				return 0;

			break;
		default:
			return 0;

			break;
	}

	return 1;
}

int findPlayer (int x, int y) {
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
		if (clients[i].sockid > 0 && (clients[i].x == x && clients[i].y == y))
			return 1;

	for (i = 0; i < map.qnt_monsters; i++)
		if (monsters[i].x == x && monsters[i].y == y)
			return 1;

	return 0;
}

int islegalMonster (int x, int y, char c) {
	switch (c) {
		case up:
			if (x - 1 == 0 || map.map[x - 1][y] != ' ' || findPlayer(x - 1, y))
				return 0;
			break;

		case down:
			if (x + 1 == map.height - 1 || map.map[x + 1][y] != ' ' || findPlayer(x + 1, y))
				return 0;
			break;

		case left:
			if (y - 1 == 0 || map.map[x][y - 1] != ' ' || findPlayer(x, y - 1))
				return 0;
			break;

		case right:
			if (y + 1 == map.width - 1 || map.map[x][y + 1] != ' ' || findPlayer(x, y + 1))
				return 0;
			break;

		default:
			return 0;
			break;
	}

	return 1;
}

// colocando os jogadores no mapa, aleatoriamente
void initClients() {
	int i, k, j = 0, colisao = 0;
	int qnt_players = clients_connected; // quantidade de jogadores a serem criados
	int posX, posY;

	do {
		// gerar ints aleatorias para X e Y
		posX = rand() % (map.height - 2) + 1;
		posY = rand() % (map.width - 2) + 1;

		//printf("posX = %d  posY = %d\n", posX, posY);
		
		// conferir se colide com player no mapa
		for (i = 0; i < (clients_connected - qnt_players); i++) {
			if (posX == clients[i].x && posY == clients[i].y)
				colisao = 1;
		}

		//printf("colisao player: %d\n", colisao);

	/*	// conferir se tem uma arvore nessa posicao
		if (map.map[posX][posY] == '*')
			colisao = 1;
	*/
		//printf("colisao *: %d\n", colisao);

		// inicializar o jogador no vetor monster[qnt_monsters]!
		if (colisao == 0) {
			clients[j].x = posX;
			clients[j].y = posY;
			clients[j].ismonster = 0;
			clients[j].hp = MAX_HP;
			clients[id].max_hp = MAX_HP;
			//clients[j].ataque = ;
			//clients[j].defesa = ;
			clients[j].fight = 0;
			clients[j].sprite = '^';
			strcpy(clients[id].color, color(j));
			j++;
			qnt_players--; // um jogador a menos para ser criado
		}
	}while (colisao == 1 || qnt_players > 0);
}

// colocando os montros no mapa, aleatoriamente
void initMonsters() {
	int i, k, j = 0, colisao = 0;
	int qnt_monsters = map.qnt_monsters; // quantidade de monstros a serem criados
	int posX, posY;

	do {
		// gerar ints aleatorias para X e Y
		posX = rand() % (map.height - 2) + 1;
		posY = rand() % (map.width - 2) + 1;

		//printf("posX = %d  posY = %d\n", posX, posY);
		
		// conferir se colide com player no mapa
		for (i = 0; i < clients_connected; i++) {
			if (posX == clients[i].x && posY == clients[i].y)
				colisao = 1;
		}

		//printf("colisao client: %d\n", colisao);

		// conferir se colide com monstro no mapa
		for (i = 0; i < (map.qnt_monsters - qnt_monsters); i++) {
			if (posX == monsters[i].x && posY == monsters[i].y)
				colisao = 1;
		}

		//printf("colisao monster: %d\n", colisao);

		// conferir se tem uma arvore nessa posicao
		if (map.map[posX][posY] == '*')
			colisao = 1;

		//printf("colisao *: %d\n", colisao);

		// inicializar o monstro no vetor monster[qnt_monsters]!
		if (colisao == 0) {
			monsters[j].x = posX;
			monsters[j].y = posY;
			monsters[j].ismonster = 1;
			monsters[j].hp = VIDA_1;
			monsters[j].max_hp = VIDA_1;
			monsters[j].ataque = ATK_1;
			monsters[j].defesa = DEF_1;
			monsters[j].fight = 0;
			monsters[j].sprite = 'm'; // chefoes devem ter sprite maiusculo (M)
			j++;
			qnt_monsters--; // um monstro a menos para ser criado
		}
	}while (colisao == 1 || qnt_monsters > 0);
}

void monsterMove () {
	int i, flag = 0;
	float chance;
	srand(time(NULL));

	for (i = 0; i < map.qnt_monsters; i++) {
		//srand48(time(NULL));
		//float chance = drand48();
		chance = rand()%101;
		chance /= 100;

		// chances iguais de escolher a direcao
		if (chance < 0.25 && islegalMonster(monsters[i].x, monsters[i].y, up)) {
			(monsters[i].x)--;
			flag = 1;
		}
		else if (chance < 0.5 && islegalMonster(monsters[i].x, monsters[i].y, down)) {
			(monsters[i].x)++;
			flag = 1;
		}
		else if (chance < 0.75 && islegalMonster(monsters[i].x, monsters[i].y, left)) {
			(monsters[i].y)--;
			flag = 1;
		}
		else if (chance <= 1.0 && islegalMonster(monsters[i].x, monsters[i].y, right)) {
			(monsters[i].y)++;
			flag = 1;
		}

		if (flag) {
			map_changes[pos_broad].type = 0;
			map_changes[pos_broad].x = monsters[i].x;
			map_changes[pos_broad].y = monsters[i].y;
			map_changes[pos_broad].id = i;
			map_changes[pos_broad].dir = -10;
			map_changes[pos_broad].sprite = monsters[i].sprite;
			map_changes[pos_broad].ismonster = 1;
			pos_broad++;
		}
	}
}

void init(){
	clients_connected = 0;
	pos_broad = 0;
	game_status = 0;
	sock = makeSocket (PORT);
	if (listen (sock, 100) < 0){
		perror ("listen");
		exit (EXIT_FAILURE);
	}

	FD_ZERO (&active_fd_set);
	FD_SET (sock, &active_fd_set);
	memset(clients, 0, sizeof clients);
}

int makeSocket (uint16_t port){
	int sock;
	struct sockaddr_in name;

	sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		perror ("socket");
		exit (EXIT_FAILURE);
	}
	name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0){
		perror ("bind");
		exit (EXIT_FAILURE);
	}

	return sock;
}

int readMovFromClient (int filedes, mov_msg *message){
	int nbytes = read (filedes, message, sizeof(mov_msg));
	if (nbytes <= 0)
		return -1;
	return 0;
}

int readClientInfo (int filedes, clientInfo *message){
	int nbytes = read (filedes, message, sizeof(clientInfo));
	if (nbytes <= 0)
		return -1;
	return 0;
}

void sendTxtToClient(int filedes, const char msg[]){
	write (filedes, msg, BUFFER_SIZE);
}

void sendUpdToClient(int filedes, upd_msg change) {
	write (filedes, &change, sizeof(upd_msg));
}

void sleepServer() {
	read_fd_set = active_fd_set;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 2;
	if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, &tv) < 0){
		perror ("select");
		exit (EXIT_FAILURE);
	}
}

void checkConn() {
	int i, sd;
	if(FD_ISSET(sock, &read_fd_set)){//connect
		int new;
		new = accept (sock, NULL, NULL);
		if (new < 0){
			perror ("accept");
			exit (EXIT_FAILURE);
		}

		clientInfo startInfo;
		char msg[BUFFER_SIZE];
		if(readClientInfo(new, &startInfo) < 0)
			return;

		if(clients_connected == MAX_CLIENTS || game_status == 2){
			sendTxtToClient(new, "- O jogo já começou ou já temos muitos clientes conectados");
			close(new);
		}
		else{
			FD_SET (new, &active_fd_set);
			for(i = 0; i < MAX_CLIENTS; ++i){
				if(clients[i].sockid == 0){
					clients[i].sockid = new;
					clientConnected(i, startInfo);
					++clients_connected;
					break;
				}
			}
		}
	}
}

void wasClient() {
	int i, sd;
	for (i = 0; i < MAX_CLIENTS; ++i){
		sd = clients[i].sockid;
		if (FD_ISSET (sd, &read_fd_set)){
			mov_msg new_mov;
			if (readMovFromClient (sd, &new_mov) < 0){ //o usuário de desconectou.
				printf("Client %d disconnected\n",  i);
				close (sd);
				FD_CLR (sd, &active_fd_set);
				clients[i].sockid = 0;
				--clients_connected;
				if(clientDesconnected != NULL){
					clientDesconnected(i);
				}
				if(clients_connected == 0)
					game_status = 0;
			}else{
				if(game_status == 1 && i == 0){
					clientConfirmed();
				}
				else if(game_status == 2){
					clientMoved(i, new_mov);
				}
			}
		}
	}
}

void broadcast() {
	assert(game_status == 2);
	int i, sd, j = 0;
	for (i = 0; i < MAX_CLIENTS; ++i){
		sd = clients[i].sockid;
		if (sd > 0){
			for (j = 0; j < pos_broad; ++j) {
				sendUpdToClient(sd, map_changes[j]);
			}
		}
	}

	pos_broad = 0;
}

void broadcastTxt(const char msg[], int s){
	int i, sd;
	for (i = 0; i < MAX_CLIENTS; ++i){
		sd = clients[i].sockid;
		if (sd > 0 && i != s){
			sendTxtToClient(sd, msg);
		}
	}
}

void disconnectClient(int id){
	shutdown(clients[id].sockid, SHUT_RD);
	clients[id].sockid = 0;
}
