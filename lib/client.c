#include "client.h"

#define asteristico 1// esses números são usados para proporção , exemplo asteristico 1 de 5 = 20%
#define espaco 4 // espaco 4 de 5 = 80%
#define maxrepetidos 5//numero max de vezes que um caractere pode ser repetido em sequncia

enum {
	MAIN,
	OPTIONS
};

void readUpd (upd_msg upd) {
	if (!upd.ismonster) {
		players[upd.id].x = upd.x;
		players[upd.id].y = upd.y;
		players[upd.id].hp = upd.hp;
		players[upd.id].atk = upd.atk;
		players[upd.id].def = upd.def;
		players[upd.id].fight = upd.fight;
		players[upd.id].whofight = upd.whofight;
		players[upd.id].sprite = upd.sprite;
	}
	else {
		monsters[upd.id].x = upd.x;
		monsters[upd.id].y = upd.y;
		monsters[upd.id].hp = upd.hp;
		monsters[upd.id].atk = upd.atk;
		monsters[upd.id].def = upd.def;
		monsters[upd.id].fight = upd.fight;
		monsters[upd.id].sprite = upd.sprite;
	}
}

void delay (unsigned int secs) {
    secs += time(0);
    while (time(0) < secs);
}

char* color (int id) {
	switch (id) {
		case 0:
			return KBLU;
			break;

		case 1:
			return KYEL;
			break;

		case 2:
			return KMAG;
			break;
	}
}

void printcchar (char *color, char c) {	
	printf("%s%c", color, c);
	printf("%s", KNRM);
}

void drawall () {
	int i, j, k, flag = 0;
	
	for(i = 0; i < map.height; i++) {
		for(j = 0; j < map.width; j++) {
			for (k = 0; k < qnt_clients && !flag; k++) {
				if(i == players[k].x && j == players[k].y && players[k].hp > 0) {
					printcchar(players[k].color, players[k].sprite);
					map.map[i][j] = ' ';
					flag = 1;
				}
			}

			for (k = 0; k < map.qnt_monsters && !flag; k++) {
				if((i == monsters[k].x && j == monsters[k].y) && monsters[k].hp > 0) {
					printcchar(monsters[k].color, monsters[k].sprite);
					flag = 1;
				}
			}

			if (flag == 0) {
				if ((i == 0 || j == 0) || (i == map.height - 1 || j == map.width - 1))
					printcchar(KRED, map.map[i][j]);
				else
					printcchar(KGRN, map.map[i][j]);
			}
			else
				flag = 0;
		}

		printf("\n");
	}
}

void drawmenus (char menu[100][100], int height, int width) {
	int i, j, flag = 0;
	
	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			if (menu[i][j] != '*' && menu[i][j] != ' ')
				printcchar(KWHT, menu[i][j]);
			else if ((i == 0 || j == 0) || (i == height - 1 || j == width - 1))
				printcchar(KRED, menu[i][j]);
			else if ((j + 1 < width && (menu[i][j] == '*' && menu[i][j + 1] == ' ')) || (i > 0 && (menu[i][j] == '*' && menu[i - 1][j] == ' ')) || (i + 1 < height && (menu[i][j] == '*' && menu[i + 1][j] == ' ')) || (j > 0 && (menu[i][j] == '*' && menu[i][j - 1] == ' ')))
				printcchar(KBLU, menu[i][j]);
			else
				printcchar(KGRN, menu[i][j]);
		}

		printf("\n");
	}
}

int islegal (int x, int y, char sprite, char c) {
	switch (c) {
		case up:
			if (x - 1 == 0 && sprite == '^')
				return 0;

			break;
		case down:
			if (x + 1 == map.height - 1 && sprite == 'v')
				return 0;

			break;
		case left:
			if (y - 1 == 0 && sprite == '<')
				return 0;

			break;
		case right:
			if (y + 1 == map.width - 1 && sprite == '>')
				return 0;

			break;
		default:
			return 0;

			break;
	}

	return 1;
}

int mod (int x, int m) { // calcula o mod corretamente
    return (x%m + m)%m;
}

void menu (clientInfo *info) {
	FILE *fpm = fopen("data/tela_inicial.txt", "r");
	FILE *fpop = fopen("data/options.txt", "r");
	char ip[20];

	int i, cursor = 0, main_height = 0, options_height = 0, draw = MAIN; // cursor: posicao da seta; main_height e options_height: quantidade de linhas de cada menu; draw: menu principal ou de opcoes
	char mainmenu[100][100], options[100][100];
	char navm[] = {menu_positions}, navop[] = {options_positions}; // armazenam as linhas de cada opcao
	char dir; // tecla pressionada

	strcpy(ip, "127.0.0.1"); // ip padrao. 127.0.0.1 = localhost
	strcpy((*info).nome, "default"); // nome padrao
	(*info).mapa = 1; // mapa padrao

	while (fscanf(fpm, " %[^\n]", mainmenu[main_height]) > 0) // preenche a matriz com o menu principal
		main_height++;
	
	while (fscanf(fpop, " %[^\n]", options[options_height]) > 0) // preenche a matriz com o menu de opcoes
		options_height++;
	
	fclose(fpm);
	fclose(fpop);

	while (1) {
		system("clear"); // limpa o terminal

		if (draw == MAIN) { // menu principal
			for (i = 0; i < main_height; i++)
				printf("%s\n", mainmenu[i]); // mostra o menu
			//drawmenus(mainmenu, main_height, strlen(mainmenu[0]));

			do
				dir = getch();
			while (dir == -1); // espera uma tecla ser pressionada

			mainmenu[navm[cursor]][cursor_pos] = ' '; // limpa a antiga posicao da seta

			if (dir == up) // cima
				cursor--;
			else if (dir == down) // baixo
				cursor++;
			else if (dir == right && cursor == 0) { // comecar o jogo e conectar ao servidor
				connectToServer(ip);
				sendInfoToServer(*info);

				memset(players, 0, sizeof players); // incialização dos vetores
				memset(monsters, 0, sizeof monsters);

				return; // sai do menu
			}
			else if (dir == right && cursor == 1) { // acessa o menu de opcoes
				cursor = 0;
				draw = OPTIONS;				
			}
			else if (dir == right && cursor == 2) // sai do jogo
				exit(1);

			cursor = mod(cursor, qnt_menu); // calcula sempre um valor permitido pelo vetor
			mainmenu[navm[cursor]][cursor_pos] = '>'; // coloca a seta na nova posicao do menu
		}
		else if (draw == OPTIONS) { // menu de opcoes
			for (i = 0; i < options_height; i++)
				printf("%s\n", options[i]);
			//drawmenus(options, options_height, strlen(options[0]));

			do
				dir = getch();
			while (dir == -1); // espera uma tecla ser pressionada

			options[navop[cursor]][cursor_pos] = ' '; // limpa...

			if (dir == up) // cima
				cursor--;
			else if (dir == down) // baixo
				cursor++;
			else if (dir == right && cursor == 0) { // trocar o nome o player
				printf("Digite seu nome: ");
				scanf(" %[^\n]", (*info).nome);
			}
			else if (dir == right && cursor == 1) { // trocar o mapa do player
				printf("Digite o mapa que deseja utilizar: ");
				scanf("%d", &(*info).mapa);				
			}
			else if (dir == right && cursor == 2) { // trocar o ip do servidor
				printf("Digite o ip do servidor: ");
				scanf("%s", ip);
			}
			else if (dir == right && cursor == 3) { // criar um mapa aleatorio
				(*info).mapa = createRandomMap();
			}
			else if (dir == right && cursor == 4) { // voltar para o menu principal
				cursor = 0;
				draw = MAIN;
			}

			cursor = mod(cursor, qnt_options); // ...
			options[navop[cursor]][cursor_pos] = '>'; // ...
		}
	}
}

// AS VEZES FUNCIONA, AS VEZES NÃO!!!!!
int createRandomMap () {
	FILE *fpmap;
	int i, j, proximo = 1, contador = 0;
	char caractere, anterior='*';
	int map_num;
	int n;
	char map_name[16];
	int height, width, qnt_monsters;

	printf("Digite a quantidade de linhas, colunas e monstros do mapa: ");
	scanf("%d %d %d", &height, &width, &qnt_monsters);

	for (i = 1; i == 1 || fpmap != NULL; i++) { // procura o primeiro numero de mapa disponivel
		if (fpmap != NULL) // se o anterior já existia, fecha o arquivo
			fclose(fpmap);

		sprintf(map_name, "data/mapa%d.txt", i); // coloca o i na string
		fpmap = fopen(map_name, "r"); // tenta abrir o mapa
	}
	map_num = i-1;
	fpmap = fopen(map_name, "w"); // abre o mapa com o número encontrado
	if (fpmap == NULL)
		printf("ERRO AO ABRIR O ARQUIVO\n");

	n = asteristico + espaco; // recebe a soma de asteristico + espaco ,como numa proporção 

	fprintf(fpmap, "%d %d %d\n", height, width, qnt_monsters);

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if(i == 0 || i == height - 1 || j == 0 || j == width - 1)
				fputc('*', fpmap);
			else {
				proximo = rand()%n;
				
				if (proximo < asteristico)
					caractere = '*';
				else
					caractere=' ';

				if (caractere == anterior)
					contador += 1;
				else
					contador = 0;

				if (contador >= maxrepetidos) {
					if (caractere == '*')
						caractere = ' ';
					else
						caractere = '*';

					contador=0;
				}
				fputc(caractere,fpmap);
		
				anterior= caractere;
			}
		}
		fputc('\n', fpmap);
		anterior = '*'; // "zera o anterior"
	}

	fclose(fpmap);

	return map_num;
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
