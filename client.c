#include "lib/client.h"

int main () {
	clientInfo info;
	char msg[BUFFER_SIZE];
	int falha = 0, comecar = 0, jogando = 1, i, j, x = 0, y = 0;
	upd_msg updt;
	mov_msg mov;

	menu(&info);
	//memset(matriz, ' ', sizeof (matriz));
	for (i = 0; i < 20; i++)
		for (j = 0; j < 20; j++)
			matriz[i][j] = ' ';

	while (comecar == 0) { // depois só mensagem de texto até o jogo começar
		if (readTxtFromServer(msg) > 0) {
			if (msg[0] == '1')
				comecar = 1;
			else if (msg[0] != 'o')
				falha = 1;
			
			printf("%s\n", msg);
		}

		if (falha)
			exit(1);

		mov.msg = getch();
		if (mov.msg != -1)
			sendMovToServer(mov);	
	}

	//receber informações iniciais do jogo(mapa, status inicial, etc...)
	//depois desse ponto, todas as mensagens recebidas serão de update, e as enviadas são de movimento.
	while (jogando) {
		while (readUpdFromServer(&updt) > 0) {// recebe todas mensagens
			// qual o tipo do update, ex:
			switch (updt.tipo) {
				case 0:
					// update no mapa
					system("clear");

					matriz[updt.x][updt.y] = ' ';

					if (updt.new == up) {
						matriz[updt.x - 1][updt.y] = updt.id + '0';
						x = updt.x - 1;
					}
					else if (updt.new == down){
						matriz[updt.x + 1][updt.y] = updt.id + '0';
						x = updt.x + 1;
					}
					else if (updt.new == left) {
						matriz[updt.x][updt.y - 1] = updt.id + '0';
						y = updt.y - 1;
					}
					else if (updt.new == right) {
						matriz[updt.x][updt.y + 1] = updt.id + '0';
						y = updt.y + 1;
					}
					else {
						matriz[updt.x][updt.y] = updt.id + '0';
						x = updt.x;
						y = updt.y;
					}

					for (i = 0; i < 20; i++) {
						for (j = 0; j < 20; j++)
							printf("[%c]", matriz[i][j]);

						printf("\n");
					}
					
					break;
				case 10:
					
					break;
			}

			//printf("%c ", updt.new);
		}

		mov.msg = getch();
		if (mov.msg != -1 && islegal(x, y, mov.msg)) // retorna -1 se demorou muito e nada foi digitado.
			sendMovToServer(mov);
	}
	
	return 0;
}