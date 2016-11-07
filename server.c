#include "lib/server.h"

void MyClientConnected(int id, clientInfo startInfo); // função que é chamada quando um cliente é connectado
void MyClientMoved(int id, mov_msg mov);// função que é chamada quando cleinte manda mensagem de movimento
void startGame(void);// função que é chamada quando cliente 0 confirma o inicio do jogo
void MyBroadcast(void);// um exemplo de como mandar uma mensagem para todos os usuários

int main () {
	srand(time(NULL)); // seed pra usar o rand durante o jogo
	mov_msg movv;
	int sd, id;

	clientMoved = MyClientMoved;
	clientConnected = MyClientConnected;
	clientConfirmed = startGame;

	init();

	while (1) {
		// espera alguma mensagem
		sleepServer();

		// verifando se foi uma conexão..
		checkConn();

		// se foi alguém se comunicando...
		wasClient();

		//broadcasting das modificações do mapa
		if(game_status == 0) {
			for (id = 0; id < MAX_CLIENTS; id++) {
				sd = clients[id].sockid;
				if(sd > 0){
					readMovFromClient(sd, &movv);
					printf("--%c--\n", movv.msg);
					if ((movv.msg == 'c') && (id == 0)) 
						game_status = 1;
				}
			}
		} else if(game_status == 2) {
			broadcast();
		}
	}
}

void MyClientConnected(int id, clientInfo startInfo){
	printf("Client %s connected, id = %d, map = %d\n", startInfo.nome, id, startInfo.mapa);
	//sendTxtToClient(clients[id].sockid, "ok");
}

void MyClientMoved(int id, mov_msg mov){
	usleep(100); // verificado experimentalmente que melhora a dinâmica do jogo
	printf("Client %d moved: %c\n", id, mov.msg);
}

void startGame(){
	printf("Client 0 confirmed, the game will start now...\n");
	broadcastTxt("1 - O jogo vai comecar", -1);
	game_status = 2;
	// avisar para os clientes que o jogo vai começar
	// enviar algumas informações, como mapa, status inicial do cliente, etc..
}

void MyBroadcast(){
	int id, sd;
	
	for(id = 0; id < MAX_CLIENTS; ++id){
		sd = clients[id].sockid;
		if(sd > 0){
			//enviar mensagem, ex: sendUpdToClient(sd, updt);
		}
	}
}