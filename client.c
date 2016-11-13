#include "lib/client.h"

int main () {
	// VARIÁVEIS
	clientInfo info;
	char msg[BUFFER_SIZE];
	upd_msg upd;
	mov_msg mov;

	int i, error = 0, start = 0, playing = 1;
	int id = -1;

	long int newtime, oldtime;

	// mapa
	FILE *fpmap; // ponteiro para o arquivo do mapa
	char map_name[16];

	menu(&info);

	while (id == -1) // receber o id do client
		if (readTxtFromServer(msg) > 0)
			id = msg[0] - '0';

	while (!start) { // depois só mensagem de texto até o jogo começar
		if (readTxtFromServer(msg) > 0) {
			if (msg[0] == 'O')
				start = 1;
			else if (msg[0] != 'o')
				error = 1;
		}

		if (error)
			exit(1);

		mov.msg = getch();
		if (mov.msg != -1)
			sendMovToServer(mov);
	}

	// formatação e mensagem de start game
	system("clear");

	for (i = 0; i < 13; i++) 
		printf("\n");
	for (i = 0; i < 4; i++)
		printf("\t");

	printf(" %s\n", msg);
	
	for (i = 0; i < 3; i++)
		printf("\t");
	for (i = 0; i < 13; i++) 
		printf("\n");

	//delay(2);

	newtime = time(NULL);
	oldtime = time(NULL);

	//receber informações iniciais do jogo(mapa, status inicial, etc...)
	//depois desse ponto, todas as mensagens recebidas serão de update, e as enviadas são de movimento.
	while (playing) {
		while (readUpdFromServer(&upd) > 0) {// recebe todas mensagens
			switch (upd.type) { // tipo de update
				case 0: // update no mapa
					// atualizando as posições de cada player
					if (!upd.ismonster) {
						players[upd.id].x = upd.x;
						players[upd.id].y = upd.y;
						players[upd.id].hp = upd.hp;
						players[upd.id].fight = upd.fight;
						players[upd.id].whofight = upd.whofight;
						players[upd.id].sprite = upd.sprite;
					} else { // atualizando as posições de cada monstro
						monsters[upd.id].x = upd.x;
						monsters[upd.id].y = upd.y;
						monsters[upd.id].hp = upd.hp;
						monsters[upd.id].fight = upd.fight;
						monsters[upd.id].sprite = upd.sprite;
					}

					/*
					for (i = 0; i < qnt_clients; i++) { // debug
						printf("players[%d].sprite: %c\n", i, players[i].sprite);
						printf("players[%d].color: %scolor%s\n", i, players[i].color, KNRM);
						printf("players[%d].x: %d\n", i, players[i].x);
						printf("players[%d].y: %d\n", i, players[i].y);
					}
					*/

					/*
					for (i = 0; i < map.qnt_monsters; i++) { // debug
						printf("monsters[%d].sprite: %c\n", i, monsters[i].sprite);
						printf("monsters[%d].color: %scolor%s\n", i, monsters[i].color, KNRM);
						printf("monsters[%d].x: %d\n", i, monsters[i].x);
						printf("monsters[%d].y: %d\n", i, monsters[i].y);
					}
					*/
					
					system("clear");
					if (!players[id].fight) {
						drawall();
						printf("%splayer %d%s\n", players[id].color, id, KNRM);
						printf("%sHP: %d%s\n", players[id].color, players[id].hp, KNRM);
						//printf("%sSCORE: %d%s\n", players[id].color, , KNRM);
						printf("%sATK: %d%s\n", players[id].color, players[id].atk, KNRM);
						printf("%sDEF: %d%s\n", players[id].color, players[id].def, KNRM);
					}
					else { // coloquei a batalha aqui só para testes, mas podemos passar para o case 1, se preferir
						printf("Battle\n");
						if (players[id].fight == 1)
							printf("%splayer %d%s; hp player1: %d; hp monster: %d\n", players[id].color, id, KNRM, players[id].hp, monsters[players[id].whofight].hp);
						else
							printf("%splayer %d%s; hp player1: %d; hp player2: %d\n", players[id].color, id, KNRM, players[id].hp, players[players[id].whofight].hp);
					}
					break;

				case 1: // em batalha

					break;

				case 4:
					printf("Dead\n");

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

					for (i = 0; i < map.qnt_monsters; i++)
						monsters[i].color = KWHT;

					break;

				case 7:
					if (upd.dir == -1 && upd.id == 0) { // informações iniciais
						qnt_clients = upd.whofight;

						for (i = 0; i < qnt_clients; i++)
							players[i].color = color(i);
					}

					if (!upd.ismonster) {
						players[upd.id].x = upd.x;
						players[upd.id].y = upd.y;
						players[upd.id].hp = upd.hp;
						players[upd.id].sprite = upd.sprite;
					}
					else {
						monsters[upd.id].x = upd.x;
						monsters[upd.id].y = upd.y;
						monsters[upd.id].hp = upd.hp;
						monsters[upd.id].sprite = upd.sprite;
					}
					break;
			}
		}

		if (id == 0) {
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
		if (((mov.msg != -1) && islegal(players[id].x, players[id].y, players[id].sprite, mov.msg)) && !players[id].fight || (players[id].fight != 0 && (mov.msg == 'a' || mov.msg == 'r'))) // retorna -1 se demorou muito e nada foi digitado.
			sendMovToServer(mov);
	}

	return 0;
}
