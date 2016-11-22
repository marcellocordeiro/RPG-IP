#include "server.h"

void buildUpd (int id, int ismonster, int type) { // retorna uma struct de update a partir de uma struct de player/monstro
	if (!ismonster) {
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = clients[id].x;
		map_changes[pos_broad].y = clients[id].y;
		map_changes[pos_broad].hp = clients[id].hp;
		map_changes[pos_broad].max_hp = clients[id].max_hp;
		map_changes[pos_broad].atk = clients[id].atk;
		map_changes[pos_broad].def = clients[id].def;
		map_changes[pos_broad].fight = clients[id].fight;
		map_changes[pos_broad].whofight = clients[id].whofight;
		map_changes[pos_broad].sprite = clients[id].sprite;
	}
	else {
		map_changes[pos_broad].id = id;
		map_changes[pos_broad].x = monsters[id].x;
		map_changes[pos_broad].y = monsters[id].y;
		map_changes[pos_broad].hp = monsters[id].hp;
		map_changes[pos_broad].max_hp = monsters[id].max_hp;
		map_changes[pos_broad].atk = monsters[id].atk;
		map_changes[pos_broad].def = monsters[id].def;
		map_changes[pos_broad].fight = monsters[id].fight;
		map_changes[pos_broad].whofight = monsters[id].whofight;
		map_changes[pos_broad].sprite = monsters[id].sprite;
	}

	map_changes[pos_broad].type = type;
	map_changes[pos_broad].ismonster = ismonster;
	pos_broad++;
}

int dmg (int atk, int def) { // sugestão 3
	int chance = rand()%101;
	int damage;

	damage = (20*atk)/def;

	if (chance < 5) // 5%
		return damage*2;
	else if (chance < 10) // 5%
		return damage*0.5;
	else if (chance < 50) // 40%
		return damage*1.5;
	else // 50%
		return damage;
}

void battleUpd (int id, char move) {
	int chance;
	int opponent = clients[id].whofight; // id do oponente é o whofight do player que atacou

	if (move == 'r') { // se o player tentou fugir
		chance = rand()%101;

		if (clients[id].fight == 1 && chance <= 70) { // 70% de chance contra monstros
			clients[id].fight = 0;
			monsters[opponent].fight = 0;

			buildUpd(id, 0, 0); // broadcast dos novos stats do player
			buildUpd(opponent, 1, 0); // broadcast dos novos stats do monstro

			return;
		}
		else if (clients[id].fight == 2 && chance <= 30) { // 30% de chance contra players
			clients[id].fight = 0;
			clients[opponent].fight = 0;

			buildUpd(id, 0, 0); // broadcast dos novos stats do player
			buildUpd(opponent, 0, 0); // broadcast dos novos stats do oponente

			return;
		}
	}

	if (clients[id].fight == 1) { // se a batalha for contra monstro
		monsters[opponent].hp -= dmg(clients[id].atk, monsters[opponent].def); // dano calculado a partir do ataque do player e da defesa do monstro
		
		if (monsters[opponent].hp > 0) // evita que os dois morram ao mesmo tempo
			clients[id].hp -= dmg(monsters[opponent].atk, clients[id].def); // dano calculado a partir do ataque do monstro e da defesa do player

		if (clients[id].hp <= 0 || monsters[opponent].hp <= 0) { // se alguém morreu, a batalha termina
			clients[id].fight = 0;
			monsters[opponent].fight = 0;

			qnt_total--;

			if (qnt_total == 1) {
				map_changes[pos_broad].type = 2;
				sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);
				disconnectClient(id);
			} else {
				if (clients[id].hp <= 0) {
					map_changes[pos_broad].type = 1;
					sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);
					disconnectClient(id);
				}
			}
		}

		buildUpd(id, 0, 0); // broadcast dos novos stats do player
		buildUpd(opponent, 1, 0); // broadcast dos novos stats do monstro
	}
	else if (clients[id].fight == 2) { // se a batalha for contra player
		clients[opponent].hp -= dmg(clients[id].atk, clients[opponent].def); // dano calculado a partir do ataque do player que atacou e defesa do outro player
		clients[id].turn = 0; 
		clients[opponent].turn = 1; // passa o turno para o oponente

		if (clients[id].hp <= 0 || clients[opponent].hp <= 0) { // se alguém morreu, a batalha termina
			clients[id].fight = 0;
			clients[opponent].fight = 0;

			qnt_total--;

			if (qnt_total == 1) {
				if (clients[opponent].hp <= 0) {
					map_changes[pos_broad].type = 1;
					sendUpdToClient(clients[opponent].sockid, map_changes[pos_broad]);
					disconnectClient(opponent);
					map_changes[pos_broad].type = 2;
					sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);
					disconnectClient(id);
				}
				else {
					map_changes[pos_broad].type = 1;
					sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);
					disconnectClient(id);
					map_changes[pos_broad].type = 2;
					sendUpdToClient(clients[opponent].sockid, map_changes[pos_broad]);
					disconnectClient(opponent);
				}
			}
			else {
				if (clients[id].hp <= 0) {
					map_changes[pos_broad].type = 1;
					sendUpdToClient(clients[id].sockid, map_changes[pos_broad]);
					disconnectClient(id);
				}
				else {
					map_changes[pos_broad].type = 1;
					sendUpdToClient(clients[opponent].sockid, map_changes[pos_broad]);
					disconnectClient(opponent);
				}
			}
		}

		buildUpd(id, 0, 0); // broadcast dos novos stats do player
		buildUpd(opponent, 0, 0); // broadcast dos novos stats do oponente
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
	int i, chance;

	for (i = 0; i < map.qnt_monsters; i++) {
		if (monsters[i].hp > 0 && !monsters[i].fight) {
			chance = rand()%101;
	
			// chances iguais de escolher a direcao
			if (chance < 25 && islegalMonster(monsters[i].x, monsters[i].y, up)) {
				monsters[i].x--;
				buildUpd(i, 1, 0);
			}
			else if (chance < 50 && islegalMonster(monsters[i].x, monsters[i].y, down)) {
				monsters[i].x++;
				buildUpd(i, 1, 0);
			}
			else if (chance < 75 && islegalMonster(monsters[i].x, monsters[i].y, left)) {
				monsters[i].y--;
				buildUpd(i, 1, 0);
			}
			else if (chance <= 100 && islegalMonster(monsters[i].x, monsters[i].y, right)) {
				monsters[i].y++;
				buildUpd(i, 1, 0);
			}
		}
	}
}

void init () {
	clients_connected = 0;
	pos_broad = 0;
	game_status = 0;
	sock = makeSocket (PORT);
	
	if (listen (sock, 100) < 0) {
		perror ("listen");
		exit (EXIT_FAILURE);
	}

	FD_ZERO (&active_fd_set);
	FD_SET (sock, &active_fd_set);

	memset(clients, 0, sizeof clients);
	memset(monsters, 0, sizeof monsters);
}

int makeSocket (uint16_t port) {
	int sock;
	struct sockaddr_in name;

	sock = socket (AF_INET, SOCK_STREAM, 0);
	
	if (sock < 0) {
		perror ("socket");
		exit (EXIT_FAILURE);
	}

	name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl (INADDR_ANY);
	
	if (bind(sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
		perror ("bind");
		exit (EXIT_FAILURE);
	}

	return sock;
}

int readMovFromClient (int filedes, mov_msg *message) {
	int nbytes = read (filedes, message, sizeof(mov_msg));
	
	if (nbytes <= 0)
		return -1;

	return 0;
}

int readClientInfo (int filedes, clientInfo *message) {
	int nbytes = read (filedes, message, sizeof(clientInfo));
	
	if (nbytes <= 0)
		return -1;

	return 0;
}

void sendTxtToClient (int filedes, const char msg[]) {
	write (filedes, msg, BUFFER_SIZE);
}

void sendUpdToClient (int filedes, upd_msg change) {
	write (filedes, &change, sizeof(upd_msg));
}

void sleepServer () {
	read_fd_set = active_fd_set;
	
	if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
		perror ("select");
		exit (EXIT_FAILURE);
	}
}

void checkConn () {
	int i, sd;
	
	if (FD_ISSET(sock, &read_fd_set)) { //connect
		int new;
		new = accept (sock, NULL, NULL);
		
		if (new < 0){
			perror ("accept");
			exit (EXIT_FAILURE);
		}

		clientInfo startInfo;
		char msg[BUFFER_SIZE];
		
		if (readClientInfo(new, &startInfo) < 0)
			return;

		if (clients_connected == MAX_CLIENTS || game_status == 2) {
			sendTxtToClient(new, "O jogo já começou ou já temos muitos clientes conectados");
			close(new);
		}
		else {
			FD_SET(new, &active_fd_set);
			
			for (i = 0; i < MAX_CLIENTS; i++) {
				if (clients[i].sockid == 0) {
					clients[i].sockid = new;
					clientConnected(i, startInfo);
					clients_connected++;
					break;
				}
			}
		}
	}
}

void wasClient () {
	int i, sd;
	
	for (i = 0; i < MAX_CLIENTS; i++) {
		sd = clients[i].sockid;

		if (FD_ISSET (sd, &read_fd_set)) {
			mov_msg new_mov;

			if (readMovFromClient(sd, &new_mov) < 0) //o usuário de desconectou.
				disconnectClient(i);
			else {
				if (game_status == 1 && i == 0)
					startGame();
				else if (game_status == 2)
					clientMoved(i, new_mov);
			}
		}
	}
}

void broadcast () {
	assert(game_status == 2);
	int i, j, sd;

	for (i = 0; i < MAX_CLIENTS; i++) {
		sd = clients[i].sockid;

		for (j = 0; sd > 0 && j < pos_broad; j++)
			sendUpdToClient(sd, map_changes[j]);
	}

	pos_broad = 0;
}

void broadcastTxt (const char msg[], int s) {
	int i, sd;

	for (i = 0; i < MAX_CLIENTS; i++) {
		sd = clients[i].sockid;
		
		if (sd > 0 && i != s)
			sendTxtToClient(sd, msg);
	}
}

void disconnectClient (int id) {
	printf("Client %d disconnected\n", id);
	close (clients[id].sockid);
	FD_CLR (clients[id].sockid, &active_fd_set);
	clients[id].sockid = 0;
	clients_connected--;

	if (clients_connected == 0) {
		game_status = 0;
		pos_broad = 0;

		memset(clients, 0, sizeof clients);
		memset(monsters, 0, sizeof monsters);
	}
}
