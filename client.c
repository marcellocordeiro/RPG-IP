#include "lib/client.h"

int main () {
	clientInfo info;
	char msg[BUFFER_SIZE];
	int falha = 0, comecar = 0, jogando = 1;
	upd_msg updt;
	mov_msg mov;

	menu(&info);

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
			/*
			switch (updt.tipo) {
				case 0:
					// update no mapa
					break;
				case 1:
					// ...
					break;
			}
			*/

			printf("%c ", updt.new);
		}

		mov.msg = getch();
		if (mov.msg != -1) // retorna -1 se demorou muito e nada foi digitado.
			sendMovToServer(mov);
	}
	
	return 0;
}