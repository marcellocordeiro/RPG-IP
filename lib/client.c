#include "client.h"

#define qtdmenu 3 // quantidade de opcoes no menu principal
#define opcoesmenu 20, 22, 24 // linhas de cada opcao do menu principal
#define qtdop 5 // quantidade de opcoes no menu de opcoes
#define opcoesoptions 20, 22, 24, 26, 28 // linhas de cada opcao no menu de opcoes
#define colseta 22 // coluna da seta >

enum {
	MAIN,
	OPTIONS
};

int mod (int x, int m) { // calcula o mod corretamente
    return (x%m + m)%m;
}

void menu (clientInfo *info) {
	FILE *mf = fopen("data/tela_inicial.txt", "r");
	FILE *opf = fopen("data/options.txt", "r");
	char ip[20];

	int i, seta = 0, qtdlm = 0, qtdlop = 0, tipomenu = MAIN; // seta: posicao da seta; qtdlm e qtdlop: quantidade de linhas de cada menu; tipomenu: menu principal ou de opcoes
	char mainmenu[100][100], options[100][100];
	char navm[] = {opcoesmenu}, navop[] = {opcoesoptions}; // armazenam as linhas de cada opcao
	char dir; // tecla pressionada

	strcpy(ip, "127.0.0.1"); // ip padrao. 127.0.0.1 = localhost
	strcpy((*info).nome, "default"); // nome padrao
	(*info).mapa = 1; // mapa padrao

	while (fscanf(mf, " %[^\n]", mainmenu[qtdlm]) > 0) // preenche a matriz com o menu principal
		qtdlm++;
	
	while (fscanf(opf, " %[^\n]", options[qtdlop]) > 0) // preenche a matriz com o menu de opcoes
		qtdlop++;
	
	fclose(mf);
	fclose(opf);

	while (1) {
		system("clear"); // limpa o terminal

		if (tipomenu == MAIN) { // menu principal
			for (i = 0; i < qtdlm; i++)
				printf("%s\n", mainmenu[i]); // mostra o menu

			do
				dir = getch();
			while (dir == -1); // espera uma tecla ser pressionada

			mainmenu[navm[seta]][colseta] = ' '; // limpa a antiga posicao da seta

			if (dir == 'w') // cima
				seta--;
			else if (dir == 's') // baixo
				seta++;
			else if (dir == 'd' && seta == 0) { // comecar o jogo e conectar ao servidor
				connectToServer(ip);
				sendInfoToServer(*info);

				return; // sai do menu
			}
			else if (dir == 'd' && seta == 1) { // acessa o menu de opcoes
				seta = 0;
				tipomenu = OPTIONS;				
			}
			else if (dir == 'd' && seta == 2) // sai do jogo
				exit(1);

			seta = mod(seta, qtdmenu); // calcula sempre um valor permitido pelo vetor
			mainmenu[navm[seta]][colseta] = '>'; // coloca a seta na nova posicao do menu
		}
		else if (tipomenu == OPTIONS) { // menu de opcoes
			for (i = 0; i < qtdlop; i++)
				printf("%s\n", options[i]);

			do
				dir = getch();
			while (dir == -1); // espera uma tecla ser pressionada

			options[navop[seta]][colseta] = ' '; // limpa...

			if (dir == 'w') // cima
				seta--;
			else if (dir == 's') // baixo
				seta++;
			else if (dir == 'd' && seta == 0) { // trocar o nome o player
				printf("Digite seu nome: ");
				scanf(" %[^\n]", (*info).nome);
			}
			else if (dir == 'd' && seta == 1) { // trocar o mapa do player
				printf("Digite o mapa que deseja utilizar: ");
				scanf("%d", &(*info).mapa);				
			}
			else if (dir == 'd' && seta == 2) { // trocar o ip do servidor
				printf("Digite o ip do servidor: ");
				scanf("%s", ip);
			}
			else if (dir == 'd' && seta == 3) { // criar um mapa aleatorio
				//createRandomMap();
			}
			else if (dir == 'd' && seta == 4) { // voltar para o menu principal
				seta = 0;
				tipomenu = MAIN;
			}

			seta = mod(seta, qtdop); // ...
			options[navop[seta]][colseta] = '>'; // ...
		}
	}
}

void connectToServer(const char *server_IP){
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);

	if(server_IP == NULL)
		server_address.sin_addr.s_addr = INADDR_ANY;
	else
		inet_aton(server_IP, &server_address.sin_addr); // lembra de tirar o .s_addr

	printf("Connecting...\n");
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(connection_status==0){
		printf("Connected\n");
	}else if(connection_status==-1){
		printf("ERROR! Connection was not successful\n");
		exit(1);
	}

	int flags = fcntl(network_socket, F_GETFL, 0); //<<< o read não vai parar o programa mais...
	fcntl(network_socket, F_SETFL, flags | O_NONBLOCK);
}

int sendMovToServer(mov_msg message){
	return write(network_socket, &message, sizeof(mov_msg));
}


int readTxtFromServer(char* message){
	usleep(1000); // dar um tempinho, já que n para e fica esperando
	return read(network_socket, message, BUFFER_SIZE);
}

int getch() {
	struct termios oldt, newt;
	int ch;
	struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	if ( poll(&mypoll, 1, 100)){
		ch = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	}
	else{
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		return -1;
	}
	return ch;
}

int sendInfoToServer(clientInfo info){
	return write(network_socket, &info, sizeof(clientInfo));
}

int readUpdFromServer(upd_msg* message){
	usleep(100); // dar um tempinho, já que n para e fica esperando
	return read(network_socket, message, sizeof(upd_msg));
}
