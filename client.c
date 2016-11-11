#include "lib/client.h"

int main () {
	clientInfo info;
	char msg[BUFFER_SIZE];
	int falha = 0, comecar = 0, jogando = 1, i, j, x = 0, y = 0;
	upd_msg updt;
	mov_msg mov;

	//variaveis
	int id = -1;
	int indice;
	char c;
	///mapa
	FILE *fpmap; //ponteiro para o arquivo do mapa
	char nome_mapa[16];

	strcpy(nome_mapa, "data/mapa");

	menu(&info);

	while (id == -1) // receber o id do client
		if (readTxtFromServer(msg) > 0)
			id = msg[0] - '0';

	while (comecar == 0) { // depois só mensagem de texto até o jogo começar
		if (readTxtFromServer(msg) > 0) {
			if (msg[0] == 'O')
				comecar = 1;
			else if (msg[0] != 'o')
				falha = 1;
		}

		if (falha)
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

	long int newtime = time(NULL);
	long int oldtime = time(NULL);

	//delay(2);

	//receber informações iniciais do jogo(mapa, status inicial, etc...)
	//depois desse ponto, todas as mensagens recebidas serão de update, e as enviadas são de movimento.
	while (jogando) {
		while (readUpdFromServer(&updt) > 0) {// recebe todas mensagens
			// qual o tipo do update, ex:
			system("clear");
			switch (updt.tipo) {
				case 0:
					// update no mapa
					if (updt.new == -1 && updt.id == 0) { //informações iniciais
						qnt_clients = updt.vida;

						for (i = 0; i < qnt_clients; i++)
							players[i].color = color(i);

						break;
					}

					if (updt.new == -1) {
						players[updt.id].sprite = updt.sprite;
						players[updt.id].x = updt.x;
						players[updt.id].y = updt.y;

						break;
					}
					else if (updt.new == -2) {
						monsters[updt.id].sprite = updt.sprite;
						monsters[updt.id].x = updt.x;
						monsters[updt.id].y = updt.y;

						break;
					}


					x = updt.x;
					y = updt.y;



					//atualizando as posições de cada player
					for (i = 0; i < qnt_clients && !updt.ismonster; i++) {
						if (updt.id == i) {
							players[i].sprite = updt.sprite;
							players[i].x = x;
							players[i].y = y;
						}

						/*printf("players[%d].sprite: %c\n", i, players[i].sprite);
						printf("players[%d].color: %scolor%s\n", i, players[i].color, KNRM);
						printf("players[%d].x: %d\n", i, players[i].x);
						printf("players[%d].y: %d\n", i, players[i].y);*/
					}

					for (i = 0; i < qnt_clients; i++) {
						printf("players[%d].sprite: %c\n", i, players[i].sprite);
						printf("players[%d].color: %scolor%s\n", i, players[i].color, KNRM);
						printf("players[%d].x: %d\n", i, players[i].x);
						printf("players[%d].y: %d\n", i, players[i].y);
					}

					//atualizando as posições de cada player
					for (i = 0; i < field.qnt_monsters && updt.ismonster; i++) {
						if (updt.id == i) {
							monsters[i].sprite = updt.sprite;
							monsters[i].x = x;
							monsters[i].y = y;
						}

						//printf("players[%d].sprite: %c\n", i, players[i].sprite);
						//printf("players[%d].color: %scolor%s\n", i, players[i].color, KNRM);
						//printf("players[%d].x: %d\n", i, players[i].x);
						//printf("players[%d].y: %d\n", i, players[i].y);
					}

					
					drawall();
					printf("%splayer %d%s\n", players[id].color, id, KNRM);

					//for (i = 0; i < field.qnt_monsters && updt.ismonster; i++)
					//	printf("monsters[%d]:\nx:  %d y: %d  sprite: %c\n", i, monsters[i].x, monsters[i].y, monsters[i].sprite);
					
					break;
				case 1:
					system("clear");
					printf("Battle\n");

					break;
				case 6: //primeira informação lida (só vai entrar 1 vez, ao começar o jogo)
					//lê o arquivo do mapa escolhido e o salva na matriz
					sprintf(nome_mapa, "%s%d.txt", nome_mapa, updt.id);

					fpmap = fopen(nome_mapa, "rt");
					if (fpmap == NULL) {
						system("clear");
						printf("ERRO: MAPA NÃO ENCONTRADO\n");
						exit(1);
					}
					fscanf(fpmap, "%d %d %d", &field.linha, &field.coluna, &field.qnt_monsters);

					for(i = 0; i < field.linha; i++)
						fscanf(fpmap, " %[^\n]", field.mapa[i]);

					fclose(fpmap);

					for (i = 0; i < field.qnt_monsters; i++)
						monsters[i].color = KWHT;

					break;
			}
		}

		
		if (id == 0) {
			if (newtime - oldtime >= 3) {
				//mov.msg = -10;
				mov.updtmonsters = 1;
				sendMovToServer(mov);
				oldtime = time(NULL);
				mov.updtmonsters = 0;
				//printf("3 sec\n");
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