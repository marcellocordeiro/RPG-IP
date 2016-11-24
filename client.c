#include "lib/client.h"

int main () {
	// ler as telas das telas do jogo (menu principal, menu de opções, batalha e game over)
	loadAll();
	
	while (1)
		game();

	return 0;
}

void game () {
	// variáveis do servidor
	char msg[BUFFER_SIZE];
	upd_msg upd;
	mov_msg mov;

	// variáveis do cliente
	int id = -1;
	int i, start = 0, playing = 1;
	long int newtime, oldtime;
	char choice;

	// menu principal
	menu(MAIN);

	// nome do mapa
	char map_name[16];

	// receber o id do client
	while (id == -1) {
		if (readTxtFromServer(msg) > 0) {
			if (msg[0] >= '0' && msg[0] <= '9') // se receber um número, é o id
				id = msg[0] - '0';
			else { // caso contrário, é uma mensagem de erro
				printf("%s\n", msg);
				exit(1);
			}
		}
	}

	// espera o servidor começar o jogo
	while (!start) {
		mov.msg = getch(); // espera uma confirmação do player 0
		if (mov.msg != -1)
			sendMovToServer(mov);

		if (readTxtFromServer(msg) > 0) { // espera o servidor começar o jogo
			qnt_clients = msg[0] - '0';
			start = 1;
		}
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

	newtime = time(NULL);
	oldtime = time(NULL);

	// receber informações iniciais do jogo (mapa, status inicial, etc...) e fazer as atualizações de movimento
	// depois desse ponto, todas as mensagens recebidas serão de update, e as enviadas são de movimento.
	while (playing) {
		while (readUpdFromServer(&upd) > 0) { // recebe todas mensagens
			switch (upd.type) { // tipo de update
				case 0: // update no mapa
					// atualizando os vetores players e monsters
					readUpd(upd);

					system("clear");
					if (!players[id].fight) {
						printMap();

						printStats(id, 0);
					}
					else { // imprime a tela de batalha
						if (players[id].fight == 1) { // contra monstros
							printBattle(battlem, battlem_height, id, players[id].whofight, 1);

							printStats(id, 0);
							printStats(players[id].whofight, 1);
						}
						else { // contra players
							printBattle(battlep, battlep_height, id, players[id].whofight, 2);

							printStats(id, 0);
							printStats(players[id].whofight, 0);
						}
					}

					break;

				case 1: // lose
					// imprime a tela de game over
					menu(LOSE);
					playing = 0;

					break;

				case 2: // win
					// imprime a tela de game over
					menu(WIN);
					playing = 0;

					break;

				case 3: // primeira informação lida (só vai entrar 1 vez, ao começar o jogo)
					// lê o arquivo do mapa escolhido e o salva na matriz
					sprintf(map_name, "data/map%d.txt", upd.id);
					loadMap(map_name);
					
					break;

				case 4: // inicialização dos vetores players e monsters
					readUpd(upd);

					break;
			}
		}

		// se for o jogador vivo de menor id, controla o movimento dos monstros
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

		// se o jogador se mover, mandar a informação para o servidor
		mov.msg = getch();
		if (mov.msg != -1 && ((!players[id].fight && islegal(players[id].x, players[id].y, players[id].sprite, mov.msg)) || (players[id].fight != 0 && (mov.msg == 'a' || mov.msg == 'r')))) // retorna -1 se demorou muito e nada foi digitado.
			sendMovToServer(mov);
	}
}
