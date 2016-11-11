#include "lib/client.h"

int main () {
	clientInfo info;
	char msg[BUFFER_SIZE];
	int i, error = 0, start = 0, playing = 1;
	upd_msg upd;
	mov_msg mov;

	//variaveis
	int id = -1;

	long int newtime = time(NULL);
	long int oldtime = time(NULL);

	int x = 0, y = 0;
	///mapa
	FILE *fpmap; //ponteiro para o arquivo do mapa
	char nome_mapa[16];

	strcpy(nome_mapa, "data/mapa");

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

	//receber informações iniciais do jogo(mapa, status inicial, etc...)
	//depois desse ponto, todas as mensagens recebidas serão de update, e as enviadas são de movimento.
	while (playing) {
		while (readUpdFromServer(&upd) > 0) {// recebe todas mensagens
			switch (upd.tipo) { // tipo de update
				case 0: // update no mapa
					if (upd.dir == -1 && upd.id == 0) { // informações iniciais
						qnt_clients = upd.vida;

						for (i = 0; i < qnt_clients; i++)
							players[i].color = color(i);

						break;
					}

					if (upd.dir == -1) {
						players[upd.id].sprite = upd.sprite;
						players[upd.id].x = upd.x;
						players[upd.id].y = upd.y;

						break;
					}
					else if (upd.dir == -2) {
						monsters[upd.id].sprite = upd.sprite;
						monsters[upd.id].x = upd.x;
						monsters[upd.id].y = upd.y;

						break;
					}

					// atualizando as posições de cada player
					for (i = 0; i < qnt_clients && !upd.ismonster; i++)
						if (upd.id == i) {
							players[i].sprite = upd.sprite;
							players[i].x = upd.x;
							players[i].y = upd.y;
						}

					/*
					for (i = 0; i < qnt_clients; i++) { // debug
						printf("players[%d].sprite: %c\n", i, players[i].sprite);
						printf("players[%d].color: %scolor%s\n", i, players[i].color, KNRM);
						printf("players[%d].x: %d\n", i, players[i].x);
						printf("players[%d].y: %d\n", i, players[i].y);
					}
					*/

					// atualizando as posições de cada monstro
					for (i = 0; i < map.qnt_monsters && upd.ismonster; i++)
						if (upd.id == i) {
							monsters[i].sprite = upd.sprite;
							monsters[i].x = upd.x;
							monsters[i].y = upd.y;
						}

					/*
					for (i = 0; i < map.qnt_monsters; i++) { // debug
						printf("monsters[%d].sprite: %c\n", i, monsters[i].sprite);
						printf("monsters[%d].color: %scolor%s\n", i, monsters[i].color, KNRM);
						printf("monsters[%d].x: %d\n", i, monsters[i].x);
						printf("monsters[%d].y: %d\n", i, monsters[i].y);
					}
					*/

					system("clear");
					drawall();
					printf("%splayer %d%s\n", players[id].color, id, KNRM);
					
					break;

				case 1: // em batalha
					system("clear");
					printf("Battle\n");

					break;
					
				case 6: // primeira informação lida (só vai entrar 1 vez, ao começar o jogo)
					// lê o arquivo do mapa escolhido e o salva na matriz
					sprintf(nome_mapa, "%s%d.txt", nome_mapa, upd.id);

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

					for (i = 0; i < map.qnt_monsters; i++)
						monsters[i].color = KWHT;

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
		if ((mov.msg != -1) && islegal(players[id].x, players[id].y, mov.msg)) // retorna -1 se demorou muito e nada foi digitado.
			sendMovToServer(mov);
	}

	return 0;
}