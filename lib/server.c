#include "server.h"

upd_msg buildUpd (int id, int ismonster) { // retorna uma struct de update a partir de uma struct de player/monstro
	upd_msg temp;

	if (!ismonster) {
		temp.ismonster = 0;
		temp.type = -1;
		temp.id = id;
		temp.x = clients[id].x;
		temp.y = clients[id].y;
		temp.hp = clients[id].hp;
		temp.atk = clients[id].atk;
		temp.def = clients[id].def;
		temp.fight = clients[id].fight;
		temp.whofight = clients[id].whofight;
		temp.sprite = clients[id].sprite;
	}
	else {
		temp.ismonster = 1;
		temp.type = -1;
		temp.id = id;
		temp.x = monsters[id].x;
		temp.y = monsters[id].y;
		temp.hp = monsters[id].hp;
		temp.atk = monsters[id].atk;
		temp.def = monsters[id].def;
		temp.fight = monsters[id].fight;
		temp.whofight = monsters[id].whofight;
		temp.sprite = monsters[id].sprite;
	}

	return temp;
}

int dmg (int atk, int def) { // fazer!!
	return atk > def ? atk : atk/2;
}

void battleUpd (int id, char move) {
	srand(time(NULL));
	int opponent = clients[id].whofight; // id do oponente é o whofight do player que atacou
	int chance = rand()%101;

	if (move == 'r') { // se o player tentou fugir
		if (clients[id].fight == 1 && chance <= 70) { // 70% de chance contra monstros
				clients[id].fight = 0;
				monsters[opponent].fight = 0;

				map_changes[pos_broad] = buildUpd(id, 0); // broadcast dos novos stats do player
				map_changes[pos_broad].type = 0; // mudar para 1?
				pos_broad++;
		
				map_changes[pos_broad] = buildUpd(opponent, 1); // broadcast dos novos stats do monstro
				map_changes[pos_broad].type = 0; // mudar para 1?
				pos_broad++;

				return;
		}
		else if (clients[id].fight == 2 && chance <= 30) { // 30% de chance contra players
				clients[id].fight = 0;
				clients[opponent].fight = 0;

				map_changes[pos_broad] = buildUpd(id, 0); // broadcast dos novos stats do player
				map_changes[pos_broad].type = 0; // mudar para 1?
				pos_broad++;
		
				map_changes[pos_broad] = buildUpd(opponent, 0); // broadcast dos novos stats do oponente
				map_changes[pos_broad].type = 0; // mudar para 1?
				pos_broad++;

				return;
		}
	}

	if (clients[id].fight == 1) { // se a batalha for contra monstro
		clients[id].hp -= dmg(monsters[opponent].atk, clients[id].def); // dano calculado a partir do ataque do monstro e da defesa do player
		monsters[opponent].hp -= dmg(clients[id].atk, monsters[opponent].def); // dano calculado a partir do ataque do player e da defesa do monstro

		if (clients[id].hp <= 0 || monsters[opponent].hp <= 0) { // se alguém morreu, a batalha termina
			clients[id].fight = 0;
			monsters[opponent].fight = 0;

			if (clients[id].hp <= 0)
				disconnectClient(id);

			qnt_total--;
		}

		map_changes[pos_broad] = buildUpd(id, 0); // broadcast dos novos stats do player
		map_changes[pos_broad].type = 0; // mudar para 1?
		pos_broad++;
		
		map_changes[pos_broad] = buildUpd(opponent, 1); // broadcast dos novos stats do monstro
		map_changes[pos_broad].type = 0; // mudar para 1?
		pos_broad++;
	}
	else if (clients[id].fight == 2) { // se a batalha for contra player
		clients[opponent].hp -= dmg(clients[id].atk, clients[opponent].def); // dano calculado a partir do ataque do player que atacou e defesa do outro player
		clients[id].turn = 0; 
		clients[opponent].turn = 1; // passa o turno para o oponente

		if (clients[id].hp <= 0 || clients[opponent].hp <= 0) { // se alguém morreu, a batalha termina
			clients[id].fight = 0;
			clients[opponent].fight = 0;

			if (clients[id].hp <= 0) {
				map_changes[pos_broad].type = 4;
				sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);
				disconnectClient(id);
			}
			else {
				map_changes[pos_broad].type = 4;
				sendUpdToClient(clients[opponent].sockid, map_changes[pos_broad]);
				disconnectClient(opponent);
			}

			qnt_total--;
		}

		map_changes[pos_broad] = buildUpd(id, 0); // broadcast dos novos stats do player
		map_changes[pos_broad].type = 0; // mudar para 1?
		pos_broad++;
		
		map_changes[pos_broad] = buildUpd(opponent, 0); // broadcast dos novos stats do oponente
		map_changes[pos_broad].type = 0; // mudar para 1?
		pos_broad++;
	}
}

int findPlayer (int x, int y) {
	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
		if ((x == clients[i].x && y == clients[i].y) && clients[i].sockid > 0)
			return i;

	return -1;
}

int findMonster (int x, int y) {
	int i;

	for (i = 0; i < map.qnt_monsters; i++)
		if ((x == monsters[i].x && y == monsters[i].y) && monsters[i].hp > 0)
			return i;

	return -1;
}

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

int islegalMonster (int x, int y, char c) {
	switch (c) {
		case up:
			if ((x - 1 == 0 || map.map[x - 1][y] != ' ') || findPlayer(x - 1, y) != findMonster(x - 1, y))
				return 0;
			break;

		case down:
			if (x + 1 == map.height - 1 || map.map[x + 1][y] != ' ' || findPlayer(x + 1, y) != findMonster(x + 1, y))
				return 0;
			break;

		case left:
			if (y - 1 == 0 || map.map[x][y - 1] != ' ' || findPlayer(x, y - 1) != findMonster(x, y - 1))
				return 0;
			break;

		case right:
			if (y + 1 == map.width - 1 || map.map[x][y + 1] != ' ' || findPlayer(x, y + 1) != findMonster(x, y + 1))
				return 0;
			break;

		default:
			return 0;
			break;
	}

	return 1;
}

// colocando os jogadores no mapa, aleatoriamente
void initClients () {
	int id;
	int qnt_players = clients_connected; // quantidade de jogadores a serem criados
	int posX, posY;
	
	for (id = 0; id < qnt_players; id++) {
		do {
			// gerar ints aleatorias para X e Y
			posX = rand() % (map.height - 2) + 1;
			posY = rand() % (map.width - 2) + 1;
		} while (findPlayer(posX, posY) != -1 || map.map[posX][posY] == '*');

		// inicializar o jogador no vetor clients!
		clients[id].x = posX;
		clients[id].y = posY;
		clients[id].hp = PLAYER_HP;
		clients[id].max_hp = PLAYER_HP;
		clients[id].atk = PLAYER_ATK;
		clients[id].def = PLAYER_DEF;
		clients[id].sprite = '^';
		//clients[id].sprite = id + '0';
	}
}

// colocando os monstros no mapa, aleatoriamente
void initMonsters () {
	int id;
	int qnt_monsters = map.qnt_monsters; // quantidade de monstros a serem criados
	int posX, posY;

	for (id = 0; id < qnt_monsters; id++) {
		do {
			// gerar ints aleatorias para X e Y
			posX = rand()%(map.height - 2) + 1;
			posY = rand()%(map.width - 2) + 1;
		} while (findPlayer(posX, posY) != -1 || findMonster(posX, posY) != -1 || map.map[posX][posY] == '*');

		// inicializar o jogador no vetor monsters!
		monsters[id].x = posX;
		monsters[id].y = posY;
		monsters[id].hp = MONSTER_HP;
		monsters[id].max_hp = MONSTER_HP;
		monsters[id].atk = MONSTER_ATK;
		monsters[id].def = MONSTER_DEF;
		monsters[id].sprite = 'm';
	}
}

void monsterMove () {
	int i, flag = 0;
	float chance;
	srand(time(NULL));

	for (i = 0; i < map.qnt_monsters; i++) {
		if (monsters[i].hp > 0 && !monsters[i].fight) {
			chance = rand()%101;
			chance /= 100;
	
			// chances iguais de escolher a direcao
			if (chance < 0.25 && islegalMonster(monsters[i].x, monsters[i].y, up)) {
				monsters[i].x--;
				flag = 1;
			}
			else if (chance < 0.5 && islegalMonster(monsters[i].x, monsters[i].y, down)) {
				monsters[i].x++;
				flag = 1;
			}
			else if (chance < 0.75 && islegalMonster(monsters[i].x, monsters[i].y, left)) {
				monsters[i].y--;
				flag = 1;
			}
			else if (chance <= 1.0 && islegalMonster(monsters[i].x, monsters[i].y, right)) {
				monsters[i].y++;
				flag = 1;
			}
	
			if (flag) {
				map_changes[pos_broad] = buildUpd(i, 1);
				map_changes[pos_broad].type = 0;
				pos_broad++;
			}
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
	memset(monsters, 0, sizeof monsters);
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
	if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0){
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
				if(clientDisconnected != NULL){
					clientDisconnected(i);
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
