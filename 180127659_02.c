/*####################################################################
######################################################################
##Universidade de Brasilia                                          ##
##Instituto de Ciencias Exatas                                      ##
##Departamento de Ciencia da Computacao                             ##
##Algoritmos e Programação de Computadores - 2/2018                 ##
##Aluno: Matheus Arruda Aguiar                                      ##
##Matricula: 18/0127659                                             ##
##Turma: A                                                          ##
##Versão do compilador: GCC (GNU Compiler Collection), padrao ANSI  ##
##Descricao: Jogo em que pecas vao caindo de cima para baixo, quando##
##forma um combo com 4 pecas iguais, elas desaparecem e voce ganha  ##
##pontos, o jogo termina quando uma coluna é totalmente preenchida. ##
######################################################################
####################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/* Mudanca de cor dos caracteres */

#define GREEN     "\x1b[32m"
#define YELLOW    "\x1b[33m"
#define BLUE      "\x1b[34m"
#define MAGENTA   "\x1b[35m"
#define CYAN      "\x1b[36m"
#define RESET     "\x1b[00m"
#define RED       "\x1b[31m"
#define PURPLE    "\x1b[35m"
#define RED2      "\x1b[91m"
#define GREEN2    "\x1b[92m"
#define YELLOW2   "\x1b[93m"
#define BLUE2     "\x1b[94m"

#ifdef _WIN32
    # define CLEAR "cls"
#else
    # define CLEAR "clear"
#endif

#ifndef _WIN32
    #include <termios.h>
    int kbhit(){
        struct termios oldt, newt;
        int ch, oldf;

        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if(ch != EOF){
            ungetc(ch,stdin);
            return 1;
        }
        return 0;
    }
    int getch(void) {
        int ch;
        struct termios oldt;
        struct termios newt;

        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#else
    #include <conio.h>
#endif


/*GLOBAIS*/
int LIN = 13;
int COL = 8;
int inputVelocidade = 720000;
int variedadePeca = 5;/*recebe o input para quantidade de tipo de pecas no jogo*/
int qtPontuacao = 4;/*recebe o input para alterar a quantidade de pecas para combo*/
int nroPecas; /*vetor de struct*/
char Tabuleiro[20][20];
char copia[20][20]; /*matriz para verificar se houve combo*/
int isDescerRapido; /*boolena*/
int inputRankeado = 0;
char nomeJogador[11];
int carregarJogo = 0; /*boolena*/
char replayNome[100]; /*nome do replay lido/escrito*/
char replayStr[3001]; /*string com as pecas do replay (maximo de 2000)*/
int replayCont = 0; /*contador para identificacao das pecas a serem lidas na string do replay*/
int replayPecasSalvas = 0;
int gravarJogo = 0; /*booleana*/

typedef struct{
    int linha;
    int coluna;
    char tipo; /*A,B,C,D,E*/
} peca;

peca peca_1;
peca peca_2;

typedef struct{
    char nick [11];
    int pontuacao;
}jogador;

jogador jog[10];

void printaPeca(char c){
    switch(c){
        case 'A':
            printf(RED "A" RESET);
            break;
        case 'B':
            printf(GREEN "B" RESET);
            break;
        case 'C':
            printf(YELLOW "C" RESET);
            break;
        case 'D':
            printf(BLUE "D" RESET);
            break;
        case 'E':
            printf(MAGENTA "E" RESET);
            break;
        case 'F':
            printf(PURPLE "F" RESET);
            break;
        case 'G':
            printf(RED2 "G" RESET);
            break;
        case 'H':
            printf(GREEN2 "H" RESET);
            break;
        case 'I':
            printf(YELLOW2 "I" RESET);
            break;
        case 'J':
            printf(BLUE2 "J" RESET);
            break;
        case '*':
            printf(CYAN "*" RESET);
            break;
        default:
            printf("%c", c);
    }
}

void criaPecas(){
    int pos_cair;
    if (carregarJogo == 1 && replayCont < replayPecasSalvas){
        peca_1.tipo = replayStr[replayCont];
        replayCont++;
        peca_2.tipo = replayStr[replayCont];
        replayCont++;
        pos_cair = 1 + rand() % (COL-3); /*POSICAO EM QUE A PECA CAI DO TOP*/
        peca_1.coluna = pos_cair;
        peca_1.linha = 0;
        peca_2.coluna = pos_cair + 1;
        peca_2.linha = 0;
        Tabuleiro[peca_1.linha][peca_1.coluna] = peca_1.tipo;
        Tabuleiro[peca_2.linha][peca_2.coluna] = peca_2.tipo;
    }else{
        peca_1.tipo = 'A' + rand() % variedadePeca;
        peca_2.tipo = 'A' + rand() % variedadePeca;
        pos_cair = 1 + rand() % (COL-3); /*POSICAO EM QUE A PECA CAI DO TOP*/
        peca_1.coluna = pos_cair;
        peca_1.linha = 0;
        peca_2.coluna = pos_cair + 1;
        peca_2.linha = 0;
        Tabuleiro[peca_1.linha][peca_1.coluna] = peca_1.tipo;
        Tabuleiro[peca_2.linha][peca_2.coluna] = peca_2.tipo;
        if (gravarJogo == 1){
            FILE* fd;
            fd = fopen(replayNome, "a");
            fprintf(fd,"%c%c",peca_1.tipo,peca_2.tipo);
            fclose(fd);
        }
    }
}

void inicializaTabuleiro(){
    int i, j;

    for(i = 0; i < LIN; i++){
        for(j = 0; j < COL; j++){
            if(j == 0 || j == (COL-1) || i == (LIN-1)){
                Tabuleiro[i][j] = '#';
            }else
                Tabuleiro[i][j] = ' ';
        }
    }
}

void printaTabuleiro(int pontos){
    int i, j;

    system(CLEAR);
    printf("Pontos : %d\n", pontos);
    printf("\n");
    for(i = 0; i < LIN; i++){
        for(j = 0; j < COL; j++){
            printaPeca(Tabuleiro[i][j]);
            if(j == (COL-1)){
                printf("\n");
            }
        }
    }
    if(inputRankeado == 1){
        printf("MODO RANKEADO\n");
    }
}

int movimentoQueda(){/*verifica se a peca pode continuar o movimento de descida*/
    if(peca_1.linha == peca_2.linha){ /*verifica se as pecas estao na horizontal*/
        if (Tabuleiro[peca_1.linha + 1][peca_1.coluna] == ' '){
            Tabuleiro[peca_1.linha + 1][peca_1.coluna] = peca_1.tipo;
            Tabuleiro[peca_1.linha][peca_1.coluna] = ' ';
        }
        if (Tabuleiro[peca_2.linha + 1][peca_2.coluna] == ' '){
            Tabuleiro[peca_2.linha + 1][peca_2.coluna] = peca_2.tipo;
            Tabuleiro[peca_2.linha][peca_2.coluna] = ' ';
        }
    }else {                           /*pecas na vertical*/
        if (Tabuleiro[peca_2.linha + 1][peca_2.coluna] == ' '){
            Tabuleiro[peca_2.linha + 1][peca_2.coluna] = peca_2.tipo;
            Tabuleiro[peca_2.linha][peca_2.coluna] = peca_1.tipo;
            Tabuleiro[peca_1.linha][peca_1.coluna] = ' ';
        }
    }
    if (Tabuleiro[peca_1.linha + 1][peca_1.coluna] != ' ' && Tabuleiro[peca_2.linha + 1][peca_2.coluna] != ' '){
        return 1;
    }
    return 0;
}

void movimentoEsquerda(){
    if(peca_1.linha == peca_2.linha){        /*verifica se as pecas estao na horizontal*/
        if (Tabuleiro[peca_1.linha][peca_1.coluna - 1] == ' ' &&
            Tabuleiro[peca_1.linha + 1][peca_1.coluna] == ' ' &&
            Tabuleiro[peca_2.linha + 1][peca_2.coluna] == ' '){

            peca_1.coluna = peca_1.coluna - 1;
            peca_2.coluna = peca_2.coluna - 1;
            Tabuleiro[peca_1.linha][peca_1.coluna] = peca_1.tipo;
            Tabuleiro[peca_2.linha][peca_2.coluna] = peca_2.tipo;
            Tabuleiro[peca_2.linha][peca_2.coluna + 1] = ' ';
        }
    }else if(peca_1.coluna == peca_2.coluna){ /*pecas na vertical*/
        if (Tabuleiro[peca_1.linha][peca_1.coluna - 1] == ' ' &&
            Tabuleiro[peca_2.linha][peca_2.coluna - 1] == ' '){

            Tabuleiro[peca_1.linha][peca_1.coluna] = ' ';
            Tabuleiro[peca_2.linha][peca_2.coluna] = ' ';
            peca_1.coluna = peca_1.coluna - 1;
            peca_2.coluna = peca_2.coluna - 1;
            Tabuleiro[peca_1.linha][peca_1.coluna] = peca_1.tipo;
            Tabuleiro[peca_2.linha][peca_2.coluna] = peca_2.tipo;
        }
    }
}

void movimentoDireita(){
    if(peca_1.linha == peca_2.linha){ /*verifica se as pecas estao na horizontal*/
        if (Tabuleiro[peca_2.linha][peca_2.coluna + 1] == ' ' &&
            Tabuleiro[peca_1.linha + 1][peca_1.coluna] == ' ' &&
            Tabuleiro[peca_2.linha + 1][peca_2.coluna] == ' '){

            peca_1.coluna = peca_1.coluna + 1;
            peca_2.coluna = peca_2.coluna + 1;
            Tabuleiro[peca_2.linha][peca_2.coluna] = peca_2.tipo;
            Tabuleiro[peca_1.linha][peca_1.coluna] = peca_1.tipo;
            Tabuleiro[peca_1.linha][peca_1.coluna - 1] = ' ';
        }
    }else {                           /*pecas na vertical*/
        if (Tabuleiro[peca_1.linha][peca_1.coluna + 1] == ' ' &&
            Tabuleiro[peca_2.linha][peca_2.coluna + 1] == ' '){

            Tabuleiro[peca_1.linha][peca_1.coluna] = ' ';
            Tabuleiro[peca_2.linha][peca_2.coluna] = ' ';
            peca_1.coluna = peca_1.coluna + 1;
            peca_2.coluna = peca_2.coluna + 1;
            Tabuleiro[peca_1.linha][peca_1.coluna] = peca_1.tipo;
            Tabuleiro[peca_2.linha][peca_2.coluna] = peca_2.tipo;
        }
    }
}

void inverterPosicao(){
    char aux;

    aux = peca_1.tipo;
    peca_1.tipo = peca_2.tipo;
    peca_2.tipo = aux;
}

void inverterOrientacao(){
    if(peca_1.linha == peca_2.linha && peca_1.linha > 0){
        Tabuleiro[peca_1.linha][peca_1.coluna] = ' ';
        peca_1.coluna = peca_1.coluna + 1;
        peca_1.linha = peca_1.linha - 1;
        Tabuleiro[peca_1.linha][peca_1.coluna] = peca_1.tipo;
    }else if(peca_1.coluna == peca_2.coluna){
        if(Tabuleiro[peca_2.linha][peca_2.coluna - 1] == ' '){
            Tabuleiro[peca_1.linha][peca_1.coluna] = ' ';
            peca_1.coluna = peca_1.coluna - 1;
            peca_1.linha = peca_1.linha + 1;
            Tabuleiro[peca_1.linha][peca_1.coluna] = peca_1.tipo;
        }
    }
}

int isGameOver(){
	int i;
    for(i = 1; i < COL - 1; i++){
        if(Tabuleiro[0][i] != ' '){
            return 1;
        }
    }
    return 0;
}

void apertarTecla(int pontos){
    if( kbhit() != 0 ){

        switch( getch() ){
            case 'a':
                if(peca_1.coluna > 1 && !isDescerRapido){
                    movimentoEsquerda();
                    printaTabuleiro(pontos);
                }
                break;
            case 'd':
                if(peca_2.coluna < COL - 2 && !isDescerRapido){
                    movimentoDireita();
                    printaTabuleiro(pontos);
                }
                break;
            case 's':
                isDescerRapido = 1;
                printaTabuleiro(pontos);
                break;
            case 'k':
                if(!isDescerRapido){
                    inverterPosicao();
                    printaTabuleiro(pontos);
                }
                break;
            case 'l':
                if(!isDescerRapido){
                    inverterOrientacao();
                    printaTabuleiro(pontos);
                }
                break;
        }
    }
    while( kbhit() ){
        getch();
    }
}

  /*################################################################################
  #essa funcao verifica se ha alguma peca igual nas posicoes adjantes, caso haja,  #
  #acontecera recurcividade ate que nao encontre mais pecas nas posicoes adjacentes#
  ################################################################################*/
void buscar(int i, int j, char alvo, peca *vetorPecas, int indice){

    copia[i][j] = '*';
    peca peca;
    int k;
    int isPecaNova = 1;
    int passou = 0;

    for(k = 0; k < nroPecas; k++){
    	if(vetorPecas[k].linha == i && vetorPecas[k].coluna == j){
    		isPecaNova = 0;
        }
    }
    if(isPecaNova){
		peca.linha = i;
		peca.coluna = j;
		peca.tipo = alvo;
		vetorPecas[indice] = peca;
		indice++;
    }
    if(copia[i][j+1] == alvo && copia[i][j+1] != '*'){/*verifica posicao a direita*/
        passou++;
        buscar(i, j+1, alvo, vetorPecas, indice);
    }
    if(copia[i][j-1] == alvo && copia[i][j-1] != '*'){/*verifica posicao a esquerda*/
        if(passou){
        	indice++;
		}
        passou++;
        buscar(i, j-1, alvo, vetorPecas, indice);
    }
    if(copia[i+1][j] == alvo && copia[i+1][j] != '*'){/*verifica posica acima*/
        if(passou){
        	indice++;
		}
		passou++;
        buscar(i+1, j, alvo, vetorPecas, indice);
    }
    if(copia[i-1][j] == alvo && copia[i-1][j] != '*'){ /*verifica posica abaixo*/
        if(passou){
        	indice++;
		}
		passou++;
        buscar(i-1, j, alvo, vetorPecas, indice);
    }
}

void copiarMatriz(){
    int i, j;

    for(i = 0; i < LIN; i++){
        for(j = 0; j < COL; j++){
            copia[i][j] = Tabuleiro[i][j];
        }
    }
}

void escreverAsteriscos(int pontos){
    int i, j, nroAsteriscos;

    for(i = 0; i < LIN; i++){
        for(j = 0; j < COL; j++){
            Tabuleiro[i][j] = copia[i][j];
        }
    }
    printaTabuleiro(pontos);

    for(j = 1; j < COL-1; j++){
    	nroAsteriscos = 0;
    	for(i = LIN-2; i >= 0; i--){
    		if(Tabuleiro[i][j] == '*'){
				nroAsteriscos++;
			}else {
				if(i+nroAsteriscos <= LIN-2){
					Tabuleiro[i+nroAsteriscos][j] = Tabuleiro[i][j];
				}
			}
        }
    }
}

int calcularPontuacao(int qtdQuedas){
    int pontos = 0;
    char alvo;
    int i, j, k;
    int nroPecas = (LIN-1)*(COL-2);
    peca vetorPecas[nroPecas];
    peca peca;
    int indice;
    int qtdQuedasUltimoCombo = 0;
    int combo = 0;

    copiarMatriz();

    for(i = LIN - 2; i >= 0; i--){
        for(j = 1; j < COL - 1; j++){
            for(k = 0; k < nroPecas; k++){
            	peca.linha = 0;
				peca.coluna = 0;
				peca.tipo = ' ';
				vetorPecas[k] = peca;
			}
			indice = 0;
			alvo = Tabuleiro[i][j]; /*alvo eh a peca de verificacao para o combo*/

			if(alvo != ' ' && alvo != '*'){
                buscar(i, j, alvo, vetorPecas, indice);
            }
            for(k = 0; k < nroPecas; k++){
            	if(vetorPecas[k].linha == 0){
            		break;
                }
			}
			if(k < qtPontuacao){ /*verifica se nao houve combo*/
            	copiarMatriz();
			} else {   /*houve combo*/
				if(qtdQuedas != qtdQuedasUltimoCombo){
					combo++;
					pontos += k;
					qtdQuedasUltimoCombo = qtdQuedas;
				} else {
					combo++;
					pontos += combo * k;
				}
				escreverAsteriscos(pontos);
			}
        }
    }
    return pontos;
}

void jogo(){
    int esp;
    int qtdQuedas = 0;
    int i;
    int velocidade = inputVelocidade;
    int pontos = 0;
    int gameOver = 0;

    srand(time(0));
    system(CLEAR);

    while( !gameOver ){
        isDescerRapido = 0;
        velocidade = inputVelocidade;
        esp = LIN-1;
        criaPecas();
        qtdQuedas++;
        usleep(velocidade);

        while(esp != 0){ /*Esse loop eh para cada descida de linha das pecas*/
            printaTabuleiro(pontos);
            for(i = 0; i < 40; i++){
                apertarTecla(pontos);
                usleep(velocidade / 40);
            }
            if(isDescerRapido){
                velocidade = (velocidade / 10);
                usleep(velocidade);
            }
            movimentoQueda();
            esp--;
            peca_1.linha++;
            peca_2.linha++;
        }
        /*Quando esse loop acaba, quer dizer que a peca acabou o movimento de descida, com isso, comeca a verificacao de combo*/
        pontos += calcularPontuacao(qtdQuedas);

        if(isGameOver()){
            int rankPosi = 0;
            if(inputRankeado == 1){
                FILE* fd;
                char file[] = "ranking.bin";
                if((fd = fopen("ranking.bin", "rb")) != NULL){
                    fd = fopen(file, "rb");
                    fread(jog, sizeof(jogador), 10, fd);
                    fclose(fd);
                }
                while(rankPosi < 10){
                    if(jog[rankPosi].pontuacao > pontos){
                        rankPosi++;
                    }else{
                        fd = fopen("ranking.bin", "wb");
                        for(i = 9; i >= rankPosi; i--){
                            jog[i].pontuacao = jog[i-1].pontuacao;
                            strncpy(jog[i].nick, jog[i-1].nick, 11);
                        }
                        jog[rankPosi].pontuacao = pontos;
                        strncpy(jog[rankPosi].nick, nomeJogador, 11);
                        fwrite(&jog, sizeof(jogador), 10, fd);
                        fclose(fd);
                        break;
                    }
                }
            }
            gameOver = 1;
            system(CLEAR);
            printf("            GAME OVER ! \n\n");
            printf("Aperte qualquer tecla para voltar ao MENU.\n");
            getch();
            getch();
            system(CLEAR);
        }
    }
}

void ranking(){

    system(CLEAR);
    printf("\n ######- Ranking de Jogadores -######\n");
    printf("  \n");
    printf(" NOME - PONTOS \n");
    printf("  \n");

    FILE* fd;
    char file[] = "ranking.bin";
    fd = fopen(file, "rb");
    if(fopen(file, "rb") == NULL){
        (fd = fopen(file, "wb"));
    }
    
    fread(&jog, sizeof(jogador), 10, fd);

    int i;
    for(i = 0; i < 10; i++){
        if((strlen(jog[i].nick) != 0) && (jog[i].nick[0] != ' '))
        printf("Nome: %s - Pontuacao: %d\n", jog[i].nick, jog[i].pontuacao);
    }
    fclose(fd);
    printf("\n\n\nAperte qualquer tecla para voltar ao MENU\n");
    getch();
    getch();
    system(CLEAR);
}

void replayGravarJogo(){
    system(CLEAR);

    printf("\n Informe o nome do arquivo no qual o jogo sera salvo com .txt: ");
    scanf(" %s", replayNome);
    printf("\n Serao gravados, no arquivo, as pecas geradas neste jogo. Aguarde\n");
    usleep(900000);
    usleep(900000);
    usleep(900000);
    gravarJogo = 1;
}

void replayCarregarJogo(){
    system(CLEAR);
    FILE* fd;

    do{
        system(CLEAR);
        printf("\n Obs: A variedade de pecas do seguinte jogo sera a mesma do jogo de quando o arquivo foi gravado.\n");
        printf("\n Informe o nome do arquivo com .txt: ");
        scanf(" %s", replayNome);
        fd = fopen(replayNome, "r+");
        if(fd == NULL){
            printf("\n O arquivo informado nao existe.\n");
            usleep(900000);
            usleep(900000);
        }else{
            carregarJogo = 1;
            fscanf(fd, " %s", replayStr);
            replayPecasSalvas = 0;
            while(replayStr[replayPecasSalvas] != '\0'){
                replayPecasSalvas++;
            }
            fclose(fd);
            break;
        }
    }while(fd == NULL);
}

void configuracao(){
    int inputConf;
    if(inputRankeado == 0){
        printf("\t CONFIGURACOES: \n\n 1 - Tabuleiro \n 2 - Pecas \n 3 - Ativar Modo Rankeado \n 4 - Voltar \n ");
    }else
        printf("\t CONFIGURACOES: \n\n 1 - Tabuleiro \n 2 - Pecas \n 3 - Desativar Modo Rankeado \n 4 - Voltar \n ");
    scanf(" %d", &inputConf);
    system(CLEAR);
    FILE *fd;
    fd = fopen("ConfJogo.txt", "w");
    switch(inputConf){
        case 1: /*editar dimensoes tabuleiro*/
            if(inputRankeado == 0){
                system(CLEAR);
                do{
                    system(CLEAR);
                    printf("\n Valores limites para Altura e Largura estao entre 4 e 20.\n\n Digite as dimensoes altura e largura do tabuleiro!\n\n");
                    printf(" Altura: ");
                    scanf(" %d", &LIN);
                    printf("\n Largura: ");
                    scanf(" %d", &COL);
                    if((LIN > 20) || (LIN < 4) || (COL > 20)  || (COL < 4)){
                        system(CLEAR);
                        printf("\n\n ERRO! Valores fora dos limites!\n\n Insira valores permitidos!\n");
                        usleep(900000);
                        usleep(900000);
                    }
                    fprintf(fd, "%d %d %d %d %d", LIN, COL, variedadePeca, qtPontuacao, inputVelocidade);
                }while((LIN > 20) || (LIN < 4) || (COL > 20)  || (COL < 4));

                system(CLEAR);
                usleep(100000);
            }else if(inputRankeado == 1){
                printf("Modo Rankeado ativado, nao eh possivel alterar configuracoes!\n");
                usleep(900000);
                usleep(900000);
                system(CLEAR);
            }
            break;
        case 2: /*editar pecas*/
            if(inputRankeado == 0){
                printf("\t CONFIGURACOES DAS PECAS: \n\n 1 - Editar variedade de pecas \n 2 - Editar quantidade de pecas conectadas para pontuar \n 3 - Editar velocidade de descida \n 4 - Voltar \n ");
                    int inputConfPecas;
                    scanf("%d", &inputConfPecas);
                    switch(inputConfPecas){
                        case 1: /*Editar variedade de pecas*/
                            do{
                                system(CLEAR);
                                printf("\n Digite um numero, entre 2 e 10, para editar a variedade de pecas que aparecerao no jogo\n\n Variedade de pecas: ");
                                scanf(" %d", &variedadePeca);
                                system(CLEAR);
                                if((variedadePeca < 2) || (variedadePeca > 10)){
                                    system(CLEAR);
                                    printf("\n\n ERRO! Valor fora do limite!\n\n Insira valor permitido!\n");
                                    usleep(900000);
                                    usleep(900000);
                                }
                                fprintf(fd, "%d %d %d %d %d", LIN, COL, variedadePeca, qtPontuacao, inputVelocidade);
                            }while((variedadePeca < 2) || (variedadePeca > 10));
                            break;
                        case 2: /*Editar quantidade de pecas conectadas para pontuar*/
                            do{
                                system(CLEAR);
                                printf("\n Digite a quantidade de pecas conectadas para se pontuar!\n\n O limite esta entre 3 e 10 pecas.\n\n Quantidade de pecas para se pontuar: ");
                                scanf("%d", &qtPontuacao);
                                if((qtPontuacao < 3) || (qtPontuacao > 10)){
                                    system(CLEAR);
                                    printf("\n\n ERRO! Valores fora dos limites!\n\n Insira valores permitidos!\n");
                                    usleep(900000);
                                    usleep(900000);
                                }
                                fprintf(fd, "%d %d %d %d %d", LIN, COL, variedadePeca, qtPontuacao, inputVelocidade);
                            }while((qtPontuacao < 3) || (qtPontuacao > 10));
                            system(CLEAR);
                            break;
                        case 3:
                            do{
                                system(CLEAR);
                                printf("\n Digite a velocidade de descida da peca no tabuleiro.\n\nO limite esta entre 500000 e 900000 .\n\nVelocidade de descida: ");
                                scanf("%d", &inputVelocidade);
                                if((inputVelocidade < 500000) || (inputVelocidade > 900000)){
                                    system(CLEAR);
                                    printf("\n\n ERRO! Valores fora dos limites!\n\n Insira valores permitidos!\n");
                                    usleep(900000);
                                    usleep(900000);
                                }
                                fprintf(fd, "%d %d %d %d %d", LIN, COL, variedadePeca, qtPontuacao, inputVelocidade);
                            }while((inputVelocidade < 500000) || (inputVelocidade > 900000));

                            system(CLEAR);
                            usleep(100000);
                            system(CLEAR);
                            break;
                        default:
                            system(CLEAR);
                            break;
                    }
            }else if(inputRankeado == 1){
                printf("Modo Rankeado ativado, nao eh possivel alterar configuracoes!\n");
                usleep(900000);
                usleep(900000);
                system(CLEAR);
                }

            break;
        case 3: /*ativar modo rankeado*/
            if(inputRankeado == 0){
                inputRankeado = 1;
            }else{
                inputRankeado = 0;
            }
            if(inputRankeado == 0){
                printf("\n\tModo Rankeado Desativado!\n");
                usleep(900000);
                system(CLEAR);
                break;
            }else if(inputRankeado == 1){
                LIN = 15;
                COL = 9;
                variedadePeca = 7;
                qtPontuacao = 5;
                inputVelocidade = 720000;
                fprintf(fd, "%d %d %d %d %d", LIN, COL, variedadePeca, qtPontuacao, inputVelocidade);

                system(CLEAR);
                printf("\n\tModo Rankeado ativo!\n");
                usleep(900000);
                system(CLEAR);
                break;
            }
            break;
        case 4: /*voltar ao menu*/
            system(CLEAR);
            break;
        default:
            break;
    }
}

void Menu(){
    int inputMenu;
    int confReplay;
    while(inputMenu != 5){
        printf("             JOGO PUYO PUYO\n\n ");
        printf(" 1 - Jogar\n  2 - Instrucoes\n  3 - Configuracoes\n  4 - Ranking\n  5 - Sair\n\n  Digite o numero correspondente a opcao desejada: \n  ");
        scanf(" %d", &inputMenu);
        system(CLEAR);
        switch(inputMenu){
            case 1: /*jogar*/
                do{
                    system(CLEAR);
                    if(inputRankeado){
                        printf("\n 1 - Jogo normal\n 2 - Replay: Gravar proximo jogo\n 3 - Voltar\n");
                        scanf(" %d", &confReplay);
                    }else{
                        printf("\n 1 - Jogo normal\n 2 - Replay: Gravar proximo jogo\n 3 - Replay: Carregar jogo salvo em arquivo\n 4 - Voltar\n");
                        scanf(" %d", &confReplay);
                    }
                    if((confReplay != 1) && (confReplay != 2) && (confReplay != 3) && (confReplay != 4)){
                        system(CLEAR);
                        printf("\n\n ERRO! Opcao invalida!\n\n Insira a opcao desejada!\n");
                        usleep(900000);
                    }
                }while ((confReplay != 1) && (confReplay != 2) && (confReplay != 3) && (confReplay != 4));
                if(inputRankeado && confReplay == 3){
                    system(CLEAR);
                    break;
                }
                if(!inputRankeado && confReplay == 4){
                    system(CLEAR);
                    break;
                }
                if(confReplay == 2){
                    system(CLEAR);
                    replayGravarJogo();
                }
                if(confReplay == 3){
                    system(CLEAR);
                    replayCarregarJogo();
                }
                system(CLEAR);
                if(inputRankeado == 1){
                    int x;
                    do{
                        printf("O nickname tem que conter no maximo 10 caracteres!\nInforme seu nickname: ");
                        scanf(" %s", nomeJogador);
                        x = strlen(nomeJogador);
                        system(CLEAR);
                        if(x > 10){
                            printf("ERRO! Nickname fora do limite.\nDigite novamente.");
                        }
                        usleep(900000);
                        system(CLEAR);
                    }while(x > 10);
                }
                inicializaTabuleiro();
                jogo();
                break;
            case 2: /*instrucoes*/
                printf("Objetivo:\n");
                printf("Cairao duas pecas por vez do topo do tabuleiro, voce pode move-las para esquerda, direita, inverter\n");
                printf("a posicao e mudar a orientacao de modo que, no minimo, 4 pecas iguais fiquem juntas para poder formar\n");
                printf("combos e desaparecerem do tabuleiro, quanto maior o numero de pecas no combo, maior a pontuacao.\n");
                printf("O jogo finaliza quando uma fileira na vertical fica completamente preenchida.\n\n");
                printf("Instrucoes:\n");
                printf("a - move o conjunto de pecas horizontalmente para esquerda;\n");
                printf("s - acelera a descida do conjunto de pecas e o movimento eh irreversivel ate que o conjunto pare;\n");
                printf("d - move o conjunto de pecas para direita;\n\n");
                printf("k - inverte a posicao das pecas;\n");
                printf("l - altera a orientacao das pecas entre vertical e horizontal.\n\n");
                printf("Aperte qualquer tecla para voltar ao MENU\n");
                getch();
                getch();
                system(CLEAR);
                break;
            case 3:
                configuracao();
                break;
            case 4:
                ranking();
                system(CLEAR);
                break;
            default:
                break;
        }
    }
    system(CLEAR);
}

int main(){
    FILE *fd;
    fd = fopen("confJogo.txt","r");
    if(fd == NULL){
        fd = fopen("confJogo.txt", "w");
        fprintf(fd,"%d %d %d %d %d", LIN, COL, variedadePeca, qtPontuacao, inputVelocidade);
    }
    fd = fopen("confJogo.txt","r");
    fscanf(fd, "%d %d %d %d %d", &LIN, &COL, &variedadePeca, &qtPontuacao, &inputVelocidade);
    system(CLEAR);

    srand(time(0));

    Menu();
    fclose(fd);
    if (carregarJogo == 1 || gravarJogo == 1){
        FILE *fd;
        fd = fopen(replayNome,"r");
        fclose(fd);
    }

    return 0;
}
