#include "lib/client.h"

int main () {
	clientInfo info;
	char msg[BUFFER_SIZE];
	int falha = 0, comecar = 0, jogando = 1, i, j, x = 0, y = 0;
	upd_msg updt;
	mov_msg mov;

	//variaveis
	char c;
	///mapa
	FILE *fpmap, *fpbat; //ponteiros para os arquivos do mapa e da tela de batalha
	MAP battleframe;
	int inbattle = 0;
	int flag = 0;
	char nome_mapa[16];

	strcpy(nome_mapa, "data/mapa");

	menu(&info);

	while (comecar == 0) { // depois só mensagem de texto até o jogo começar
		if (readTxtFromServer(msg) > 0) {
			if (msg[0] == 'O')
				comecar = 1;
			else if (msg[0] != 'o')
				falha = 1;
			
			system("clear");
			printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t %s\n\n\n\n\n\n\n\n\n\n\n\n\n", msg);

			do {
				c = getch();
			} while(c == -1);
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
 
 					field.mapa[updt.x][updt.y] = ' ';
 
 					if (updt.new == up) {
 						field.mapa[updt.x - 1][updt.y] = updt.id + '0';
 						x = updt.x - 1;
 					}
 					else if (updt.new == down){
 						field.mapa[updt.x + 1][updt.y] = updt.id + '0';
 						x = updt.x + 1;
 					}
 					else if (updt.new == left) {
 						field.mapa[updt.x][updt.y - 1] = updt.id + '0';
 						y = updt.y - 1;
 					}
 					else if (updt.new == right) {
 						field.mapa[updt.x][updt.y + 1] = updt.id + '0';
 						y = updt.y + 1;
 					}
 					else {
 						field.mapa[updt.x][updt.y] = updt.id + '0';
 						x = updt.x;
 						y = updt.y;
 					}
 
 					for (i = 0; i < field.linha; i++) {
 						for (j = 0; j < field.coluna; j++)
 							printf("%c", field.mapa[i][j]);
 
 						printf("\n");
 					}
 					
 					break;
				case 6: //primeira informação lida (só vai entrar 1 vez, ao começar o jogo)
					info.mapa = updt.id; //le o mapa escolhido pelo cliente de id = 0

					//lê o arquivo do mapa escolhido e o salva na matriz
					sprintf(nome_mapa, "%s%d.txt", nome_mapa, info.mapa);

					fpmap = fopen(nome_mapa, "rt");
					if (fpmap == NULL) {
						system("clear");
						printf("ERRO: MAPA NÃO ENCONTRADO\n");
						exit(1);
					}
					fscanf(fpmap, "%d %d %d", &field.linha, &field.coluna, &field.monstros);

					field.mapa = (char**) malloc(field.linha*sizeof(char*));
					if (field.mapa == NULL) {
						system("clear");
						printf("ERRO: MEMÓRIA NÃO ALOCADA\n");
						exit(1);
					}
					for(i = 0; i < field.linha; i++){
						field.mapa[i] = (char*) malloc((field.coluna + 1)*sizeof(char));
						if (field.mapa[i] == NULL) {
							system("clear");
							printf("ERRO: MEMÓRIA NÃO ALOCADA\n");
							exit(1);
						}
					}

					for(i = 0; i < field.linha; i++)
						fscanf(fpmap, " %[^\n]", field.mapa[i]);

					fclose(fpmap);

					break;
			}

			//printf("%c ", updt.new);
		}

		mov.msg = getch();
		if ((mov.msg != -1) && islegal(x, y, mov.msg)) // retorna -1 se demorou muito e nada foi digitado.
			sendMovToServer(mov);
	}
	
	return 0;
}