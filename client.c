#include "lib/client.h"

int main () {
	char battle[30][110], lose[30][110], win[30][110];

	// leitura da tela de batalha
	loadFile("data/fight_frame.txt", battle);

	// leitura da tela de game over
	loadFile("data/lose_frame.txt", lose);

	// leitura da tela de game over
	loadFile("data/win_frame.txt", win);

	while (1)
		game(battle, lose, win); // sugestão (o nome tá ok?)

	return 0;
}

void game (char battle[30][110], char lose[30][110], char win[30][110]) {
	// variáveis do servidor
	clientInfo info;
	char msg[BUFFER_SIZE];
	upd_msg upd;
	mov_msg mov;

	// variáveis do cliente
	int id = -1;
	int i, start = 0, playing = 1;
	long int newtime, oldtime;
	char choice;
	
	// menu
	menu(&info);

	// mapa
	FILE *fpmap; // ponteiro para o arquivo do mapa
	char map_name[16];

	while (id == -1) // receber o id do client
		if (readTxtFromServer(msg) > 0)
			id = msg[0] - '0';

	while (!start) { // espera o servidor começar o jogo
		if (readTxtFromServer(msg) > 0) {
			if (msg[0] > '0' && msg[0] <= '9') {
				qnt_clients = msg[0] - '0';
				start = 1;
			}
		}

		mov.msg = getch();
		if (mov.msg != -1)
			sendMovToServer(mov);
	}

	// formatação e mensagem de start game
	system("clear");

	for (i = 0; i < 13; i++) 
		printf("\n");
	for (i = 0; i < 3; i++)
		printf("\t");

	printf("%s\n", msg);
	
	for (i = 0; i < 13; i++) 
		printf("\n");

	//delay(2);

	newtime = time(NULL);
	oldtime = time(NULL);

	//receber informações iniciais do jogo(mapa, status inicial, etc...)
	//depois desse ponto, todas as mensagens recebidas serão de update, e as enviadas são de movimento.
	while (playing) {
		while (readUpdFromServer(&upd) > 0) { // recebe todas mensagens
			switch (upd.type) { // tipo de update
				case 0: // update no mapa
					// atualizando os vetores players e monsters
					readUpd(upd);

					/*
					for (i = 0; i < qnt_clients; i++) { // debug
						printf("players[%d].sprite: %c\n", i, players[i].sprite);
						printf("players[%d].color: %scolor%s\n", i, color(i), KNRM);
						printf("players[%d].x: %d\n", i, players[i].x);
						printf("players[%d].y: %d\n", i, players[i].y);
					}
					*/

					/*
					for (i = 0; i < map.qnt_monsters; i++) { // debug
						printf("monsters[%d].sprite: %c\n", i, monsters[i].sprite);
						printf("monsters[%d].color: %scolor%s\n", i, KWTH, KNRM);
						printf("monsters[%d].x: %d\n", i, monsters[i].x);
						printf("monsters[%d].y: %d\n", i, monsters[i].y);
					}
					*/
					
					system("clear");
					if (!players[id].fight) {
						drawall();

						printf("%splayer %d\n", color(id), id);
						printf("HP:\t%d\n", players[id].hp);
						printf("ATK:\t%d\n",players[id].atk);
						printf("DEF:\t%d%s\n", players[id].def, KNRM);
					}
					else { // batalha
						for (i = 0; i < 26; i++)
							printf("%s\n", battle[i]);

						printf("%splayer %d\n", color(id), id);
						printf("HP:\t%d\n", players[id].hp);
						printf("ATK:\t%d\n", players[id].atk);
						printf("DEF:\t%d%s\n\n", players[id].def, KNRM);
						
						if (players[id].fight == 1) {
							printf("%smonster\n", KWHT);
							printf("HP:\t%d\n", monsters[players[id].whofight].hp);
							printf("ATK:\t%d\n", monsters[players[id].whofight].atk);
							printf("DEF:\t%d%s\n\n", monsters[players[id].whofight].def, KNRM);
						}
						else {
							printf("%splayer %d\n", color(players[id].whofight), players[id].whofight);
							printf("HP:\t%d\n", players[players[id].whofight].hp);
							printf("ATK:\t%d\n", players[players[id].whofight].atk);
							printf("DEF:\t%d%s\n\n", players[players[id].whofight].def, KNRM);
						}
					}
					break;

				case 1: // em batalha

					break;

				case 4: // lose
					system("clear");
					for (i = 0; i < 28; i++)
						printf("%s\n", lose[i]);

					do
						choice = getch();
					while (choice != 'h' && choice != 'q');

					if (choice == 'h') {
						playing = 0;
						break;
					}
					else if (choice == 'q')
						exit(1);

					break;

				case 5: // win
					system("clear");
					for (i = 0; i < 28; i++)
						printf("%s\n", win[i]);

					do
						choice = getch();
					while (choice != 'h' && choice != 'q');

					if (choice == 'h') {
						playing = 0;
						break;
					}
					else if (choice == 'q')
						exit(1);

					break;

				case 6: // primeira informação lida (só vai entrar 1 vez, ao começar o jogo)
					// lê o arquivo do mapa escolhido e o salva na matriz
					sprintf(map_name, "data/mapa%d.txt", upd.id);

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
					
					break;

				case 7:
					readUpd(upd);

					break;
			}
		}

		if (id == firstAlive()) {
			if (newtime - oldtime >= 1) {
				mov.upd_monsters = 1;
				sendMovToServer(mov);
				oldtime = time(NULL);
				mov.upd_monsters = 0;
			}
			else
				newtime = time(NULL);
		}

		mov.msg = getch();
		if (mov.msg != -1 && ((!players[id].fight && islegal(players[id].x, players[id].y, players[id].sprite, mov.msg)) || (players[id].fight != 0 && (mov.msg == 'a' || mov.msg == 'r')))) // retorna -1 se demorou muito e nada foi digitado.
			sendMovToServer(mov);
	}
}
