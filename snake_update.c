/*=================bibliotecas==================*/
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>

HANDLE hConsole;
void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
/* ================= UTIL ================= */

void setColor(int color){
    SetConsoleTextAttribute(hConsole,color);
}

void gotoxy(int x,int y){
    COORD coord={x,y};
    SetConsoleCursorPosition(hConsole,coord);
}
/*===================defs=======================*/
#define SYM_H "═"
#define SYM_V "║"
#define SYM_TL "╔"
#define SYM_TR "╗"
#define SYM_BL "╚"
#define SYM_BR "╝"
#define SYM_SEP_L "╠"
#define SYM_SEP_R "╣"
#define WIDTH 60
#define HEIGHT 30
#define OFFSET_X 30
#define TOP_OFFSET 4
#define SPEED 50
#define FRUIT_COUNT 5
#define COR_CABECA   10
#define COR_CORPO    2
#define COR_FRUTA    12
#define COR_SCORE    14
#define COR_BORDA    9
#define COR_TEXTO    7
#define COR_IMUNIDADE 11
#define IMMUNITY_DURATION 50
#define LARGURA_TELA 120
#define MENU_OPCOES 3
#define MAX_RANK 10





typedef struct Node{
    int x,y;
    struct Node *next;
}Node;

typedef struct{
    int x,y;
    int type;
}Fruit;

enum{
    APPLE,
    GRAPE,
    BANANA,
    PINEAPPLE
};

Node *head=NULL;
Node *tail=NULL;
Fruit fruits[FRUIT_COUNT];


/*==============================ints==============================*/
int cauda_ant_x = 0;
int cauda_ant_y = 0;
int dirX=0, dirY=0;
int score=0;
int gameOver=0;
int immunity=0;
int immunityTimer=0;
int pineappleSpawned=0;
int paused=0;
int blinkState=0;
int opcao_menu = 0;
int speedBoost =0;
int speedBoostTimer=0;

int get_recorde();


/*=============tela de gameover==============================*/
int tela_game_over(){

    int opcao = 0;
    int max_opcoes = 3;

    int recorde = get_recorde();

    while(1){

        system("cls");

        int col = 40;
        int lin = 5;

  //TELA DE VITORIA
  if(gameOver == 2){
    setColor(10);
    gotoxy(col+2, lin+1);
    printf("   PARABÉNS! VOCÊ VENCEU!   ");
}


        // TÍTULO
        setColor(12);
        gotoxy(col, lin); printf("%s", SYM_TL);
        for(int i=0;i<22;i++) printf("%s", SYM_H);
        printf("%s", SYM_TR);

        gotoxy(col, lin+1);
        printf("%s     GAME  OVER       %s", SYM_V, SYM_V);

        gotoxy(col, lin+2);
        printf("%s", SYM_BL);
        for(int i=0;i<22;i++) printf("%s", SYM_H);
        printf("%s", SYM_BR);

        // SCORE
        setColor(COR_SCORE);
        gotoxy(col+2, lin+4);
        printf("Score:   %05d", score);

        // RECORDE
        setColor(11);
        gotoxy(col+2, lin+5);
        printf("Recorde: %05d", recorde);

        // OPÇÕES
        char *opcoes[3] = {
            "Jogar Novamente",
            "Menu",
            "Sair"
        };

        for(int i=0;i<max_opcoes;i++){

            gotoxy(col+2, lin+7 + i*2);

            if(i == opcao){
                setColor(14);
                printf("> %s", opcoes[i]);
            } else {
                setColor(7);
                printf("  %s", opcoes[i]);
            }
        }

        // INPUT
        int key = _getch();

        if(key == 224){
            key = _getch();

            if(key == 72){ // ↑
                opcao--;
                if(opcao < 0) opcao = max_opcoes-1;
            }

            if(key == 80){ // ↓
                opcao++;
                if(opcao >= max_opcoes) opcao = 0;
            }
        }

        if(key == 'w' || key == 'W'){
            opcao--;
            if(opcao < 0) opcao = max_opcoes-1;
        }

        if(key == 's' || key == 'S'){
            opcao++;
            if(opcao >= max_opcoes) opcao = 0;
        }

        if(key == 13){ // ENTER
            return opcao;
        }
    }
}
/*======================estrutura do rank===================*/
typedef struct{
    char nome[20];
    int score;
}Rank;

Rank ranking[MAX_RANK];
int totalRanking = 0;

/*============funçao do recorde=============================*/
int get_recorde(){
    if(totalRanking > 0)
        return ranking[0].score;
    return 0;
}


/*=========carregando o ranking para um arquivo=============*/

void carregar_ranking(){

    FILE *f = fopen("ranking.txt", "r");

    if(!f) return;

    totalRanking = 0;

    while(fscanf(f, "%s %d", ranking[totalRanking].nome, &ranking[totalRanking].score) != EOF){
        totalRanking++;
        if(totalRanking >= MAX_RANK) break;
    }

    fclose(f);
}




/*============salvando o ranking no arquivo=====================*/
void salvar_ranking(){

    FILE *f = fopen("ranking.txt", "w");

    for(int i=0;i<totalRanking;i++){
        fprintf(f, "%s %d\n", ranking[i].nome, ranking[i].score);
    }

    fclose(f);
}

/*===================add o novo score================================*/
void adicionar_ranking(char nome[], int score){

    // adiciona no final
    strcpy(ranking[totalRanking].nome, nome);
    ranking[totalRanking].score = score;
    totalRanking++;

    // ordena (maior score primeiro)
    for(int i=0;i<totalRanking-1;i++){
        for(int j=i+1;j<totalRanking;j++){
            if(ranking[j].score > ranking[i].score){

                Rank temp = ranking[i];
                ranking[i] = ranking[j];
                ranking[j] = temp;
            }
        }
    }

    // limita a 10
    if(totalRanking > MAX_RANK)
        totalRanking = MAX_RANK;

    salvar_ranking();
}


/*=================Menu inicial===================*/
void desenhar_menu(){

    system("cls");

    int col = 40;
    int lin = 5;

    // TÍTULO
    setColor(COR_BORDA);
    gotoxy(col, lin);     printf("%s", SYM_TL);
    for(int i=0;i<22;i++) printf("%s", SYM_H);
    printf("%s", SYM_TR);

    gotoxy(col, lin+1);
    printf("%s      S N A K E       %s", SYM_V, SYM_V);

    gotoxy(col, lin+2);
    printf("%s", SYM_BL);
    for(int i=0;i<22;i++) printf("%s", SYM_H);
    printf("%s", SYM_BR);

    // OPÇÕES
    char *opcoes[MENU_OPCOES] = {
        "Jogar",
        "Ranking",
        "Sair"
    };

    for(int i=0;i<MENU_OPCOES;i++){

        gotoxy(col+4, lin+5 + i*2);

        if(i == opcao_menu){
            setColor(14);
            printf("> %s", opcoes[i]);
        } else {
            setColor(7);
            printf("  %s", opcoes[i]);
        }
    }

    // instrução
    setColor(8);
    gotoxy(col, lin+12);
    printf("Use W/S ou SETAS e ENTER");
}

/*==========input do menu===================================*/
int menu_input(){

    int key = _getch();

    if(key == 224){ // setas
        key = _getch();

        if(key == 72){ // ↑
            opcao_menu--;
            if(opcao_menu < 0) opcao_menu = MENU_OPCOES-1;
        }

        if(key == 80){ // ↓
            opcao_menu++;
            if(opcao_menu >= MENU_OPCOES) opcao_menu = 0;
        }
    }

    if(key == 'w' || key == 'W'){
        opcao_menu--;
        if(opcao_menu < 0) opcao_menu = MENU_OPCOES-1;
    }

    if(key == 's' || key == 'S'){
        opcao_menu++;
        if(opcao_menu >= MENU_OPCOES) opcao_menu = 0;
    }

    if(key == 13){ // ENTER
        return opcao_menu;
    }

    return -1;
}
/*=============================tela de ranking======================*/
void tela_ranking(){

    system("cls");

    carregar_ranking();

    int col = 35;
    int lin = 5;

    setColor(COR_BORDA);

    gotoxy(col, lin); printf("%s", SYM_TL);
    for(int i=0;i<30;i++) printf("%s", SYM_H);
    printf("%s", SYM_TR);

    gotoxy(col, lin+1);
    printf("%s         RANKING         %s", SYM_V, SYM_V);

    gotoxy(col, lin+2);
    printf("%s", SYM_BL);
    for(int i=0;i<30;i++) printf("%s", SYM_H);
    printf("%s", SYM_BR);

    for(int i=0;i<totalRanking;i++){

        if(i==0) setColor(14);
        else if(i==1) setColor(7);
        else if(i==2) setColor(6);
        else setColor(8);

        gotoxy(col+2, lin+4+i);
        printf("%d. %-10s %05d", i+1,
               ranking[i].nome,
               ranking[i].score);
    }

    setColor(8);
    gotoxy(col, lin+15);
    printf("ENTER para voltar");

    while(_getch() != 13);
}
/*====================loop do menu===========================*/
void loop_menu(){

    while(1){

        desenhar_menu();

        int escolha = menu_input();

        if(escolha == -1) continue;

        if(escolha == 0){
            return; // iniciar jogo
        }

        if(escolha == 1){
            tela_ranking();
        }

        if(escolha == 2){
            exit(0);
        }
    }
}

int snake_on_position(int x,int y);

/*==============Fundo do tabuleiro==============================*/
void draw_background(){

    setColor(8); // cinza escuro (não polui visual)

    for(int y = TOP_OFFSET + 1; y < HEIGHT + TOP_OFFSET; y++){
        for(int x = 1; x < WIDTH; x++){

            // evita desenhar onde já tem cobra
            if(!snake_on_position(x, y)){
                gotoxy(x + OFFSET_X, y);
                printf(".");
            }
        }
    }

    setColor(COR_TEXTO);
}

/* ================= SNAKE ================= */

void insert_head(int x,int y){
    Node *newNode=malloc(sizeof(Node));
    newNode->x=x;
    newNode->y=y;

    if(!head){
        head=tail=newNode;
        newNode->next=newNode;
    } else {
        newNode->next=head;
        tail->next=newNode;
        head=newNode;
    }
}
/*============removedor da cauda============*/
void remove_tail(){

    if(head==tail) return;

    Node *temp=head;

    while(temp->next!=tail)
        temp=temp->next;

    
    cauda_ant_x = tail->x;
    cauda_ant_y = tail->y;

    temp->next=head;
    free(tail);
    tail=temp;
}

/*==============anti flicker==============*/
void render_delta();
void render_delta(){

    // restaura fundo ao invés de apagar
    gotoxy(cauda_ant_x + OFFSET_X, cauda_ant_y);
    setColor(8);
    printf(".");

    // cabeça
    int headColor = immunity ? (blinkState ? 11 : 3) : COR_CABECA;

    setColor(headColor);
    gotoxy(head->x + OFFSET_X, head->y);
    printf("O");

    // corpo
    if(head->next != head){
        int bodyColor = immunity ? (blinkState ? 11 : 3) : COR_CORPO;

        setColor(bodyColor);
        gotoxy(head->next->x + OFFSET_X, head->next->y);
        printf("o");
    }

    setColor(COR_TEXTO);
}




/* ================= HUD ================= */

void draw_border(){

    setColor(COR_BORDA);

    // canto superior esquerdo
    gotoxy(OFFSET_X, TOP_OFFSET);
    printf("%s", SYM_TL);

    // linha superior
    for(int i=1;i<WIDTH;i++){
        printf("%s", SYM_H);
    }

    // canto superior direito
    printf("%s", SYM_TR);

    // laterais
    for(int i=TOP_OFFSET+1;i<HEIGHT+TOP_OFFSET;i++){

        gotoxy(OFFSET_X, i);
        printf("%s", SYM_V);

        gotoxy(WIDTH + OFFSET_X, i);
        printf("%s", SYM_V);
    }

    // canto inferior esquerdo
    gotoxy(OFFSET_X, HEIGHT + TOP_OFFSET);
    printf("%s", SYM_BL);

    // linha inferior
    for(int i=1;i<WIDTH;i++){
        printf("%s", SYM_H);
    }

    // canto inferior direito
    printf("%s", SYM_BR);

    setColor(COR_TEXTO);
}

void draw_score(){
    gotoxy(0,0);
    for(int i=0;i<80;i++) printf(" ");

    setColor(COR_SCORE);
    gotoxy(2,0);
    printf("SNAKE GAME");
}








/* ===== Painel   ===== */

void draw_side_panel(){

    int x = WIDTH + OFFSET_X + 3;
    int w = 22;
    int y = TOP_OFFSET;

    setColor(COR_BORDA);

    // TOPO
    gotoxy(x,y);
    printf("%s", SYM_TL);
    for(int i=0;i<w;i++) printf("%s", SYM_H);
    printf("%s", SYM_TR);
    y++;

    // SCORE
    setColor(COR_SCORE);
    gotoxy(x,y);
    printf("%s Score: %-10d    %s", SYM_V, score, SYM_V);
    y++;

    // STATUS
gotoxy(x,y);

if(paused){
    setColor(14);
    printf("%s PAUSADO              %s", SYM_V, SYM_V);
}
else if(immunity){
    setColor(COR_IMUNIDADE);
    printf("%s IMUNE                %s", SYM_V, SYM_V);
}
else{
    setColor(10);
    printf("%s RODANDO              %s", SYM_V, SYM_V);
}
y++;

    // SEPARADOR
    setColor(COR_BORDA);
    gotoxy(x,y);
    printf("%s", SYM_SEP_L);
    for(int i=0;i<w;i++) printf("%s", SYM_H);
    printf("%s", SYM_SEP_R);
    y++;

    // BARRA DE IMUNIDADE
    setColor(COR_TEXTO);
    gotoxy(x,y);
    printf("%s [", SYM_V);

    int filled = (immunityTimer * 10) / IMMUNITY_DURATION;

    for(int i=0;i<10;i++)
        printf(i<filled?"#":" ");

    printf("]         %s", SYM_V);
    y++;

    // SEPARADOR
    setColor(COR_BORDA);
    gotoxy(x,y);
    printf("%s", SYM_SEP_L);
    for(int i=0;i<w;i++) printf("%s", SYM_H);
    printf("%s", SYM_SEP_R);
    y++;

    

// FRUTAS
    setColor(12);
    gotoxy(x,y); printf("%s @ Maçã  +2           %s", SYM_V, SYM_V); y++;
 
    setColor(13);
    gotoxy(x,y); printf("%s o Uva   +5           %s", SYM_V, SYM_V); y++;

    setColor(14);
    gotoxy(x,y); printf("%s ) Banana  +10        %s", SYM_V, SYM_V); y++;

    setColor(11);
    gotoxy(x,y); printf("%s & Bônus    +100      %s", SYM_V, SYM_V); y++;

    // BASE
    setColor(COR_BORDA);
    gotoxy(x,y);
    printf("%s", SYM_BL);
    for(int i=0;i<w;i++) printf("%s", SYM_H);
    printf("%s", SYM_BR);

    setColor(COR_TEXTO);
}

    

/* ================= COLLISION ================= */

int snake_on_position(int x,int y){
    Node *temp=head;
    do{
        if(temp->x==x && temp->y==y)
            return 1;
        temp=temp->next;
    }while(temp!=head);
    return 0;
}

int collision(int x,int y){
    if (x <= 0 || x >= WIDTH ||
        y <= TOP_OFFSET || y >= HEIGHT + TOP_OFFSET)
        return 1;

    Node *temp=head->next;
    while(temp!=head){
        if(temp->x==x && temp->y==y){
            if(immunity){
                score -= 50;
                if(score<0) score=0;
                return 0;
            }
            return 1;
        }
        temp=temp->next;
    }
    return 0;
}

/* ================= FRUITS ================= */

int chooseFruitType(){
    int r=rand()%100;
    if(r<60) return APPLE;
    if(r<85) return GRAPE;
    return BANANA;
}

void drawFruit(Fruit f){

    gotoxy(f.x + OFFSET_X,f.y);

    if(f.type==APPLE)
        setColor(blinkState ? 12 : 4);

    if(f.type==GRAPE)
        setColor(blinkState ? 13 : 5);

    if(f.type==BANANA)
        setColor(blinkState ? 14 : 6);

    if(f.type==PINEAPPLE)
        setColor(blinkState ? 11 : 3);

    if(f.type==APPLE) printf("@");
    if(f.type==GRAPE) printf("o");
    if(f.type==BANANA) printf(")");
    if(f.type==PINEAPPLE) printf("&");

    setColor(COR_TEXTO);
}

void generate_fruit(int i){
    do{
        fruits[i].x = rand()%(WIDTH-2)+1;
        fruits[i].y = rand()%(HEIGHT-1)+1+TOP_OFFSET;
    }while(snake_on_position(fruits[i].x,fruits[i].y));

    fruits[i].type = (score>=150 && rand()%100<3) ? PINEAPPLE : chooseFruitType();

    drawFruit(fruits[i]);
}

void init_fruits(){
    for(int i=0;i<FRUIT_COUNT;i++)
        generate_fruit(i);
}

/* ================= INPUT ================= */

void input(){

    if(_kbhit()){

        int key=_getch();

        if(key=='p' || key=='P'){
            paused=!paused;
            return;
        }

        if(paused) return;

        if(key==224){
            key=_getch();
            if(key==72 && dirY!=1){dirX=0;dirY=-1;}
            if(key==80 && dirY!=-1){dirX=0;dirY=1;}
            if(key==75 && dirX!=1){dirX=-1;dirY=0;}
            if(key==77 && dirX!=-1){dirX=1;dirY=0;}
        }
    }
}
    /*==========overlay de pause================*/



/* ================= UPDATE ================= */

void update(){

    if(paused){
    
    draw_side_panel();
    return;
}

    blinkState=!blinkState;

    if(dirX==0 && dirY==0) return;



    /*===========TIMER IMUNIDADE=================*/ 
    if(immunity){
        immunityTimer--;
        if(immunityTimer<=0){
            immunity=0;
        }
    }

    int newX=head->x+dirX;
    int newY=head->y+dirY;

    // verifica se ocupou todo o tabuleiro
int occupied = 0;
Node* temp = head;
do {
    occupied++;
    temp = temp->next;
} while(temp != head);

int totalSpaces = (WIDTH-1) * (HEIGHT-1); // sem contar bordas
if(occupied >= totalSpaces){
    gameOver = 2; // 2 = vitória
}
    if(collision(newX,newY)){
        gameOver=1;
        return;
    }

    // COR DA COBRA
    int bodyColor = immunity ? (blinkState ? 11 : 3) : COR_CORPO;
    int headColor = immunity ? (blinkState ? 11 : 3) : COR_CABECA;
    insert_head(newX,newY);
    int ateFruit=0;

    for(int i=0;i<FRUIT_COUNT;i++){
        if(newX==fruits[i].x && newY==fruits[i].y){

            ateFruit=1;

            if(fruits[i].type==APPLE) score+=2;
            if(fruits[i].type==GRAPE) score+=5;
            /*Se comer a banana ganha mais velocidade por 3s*/
            if(fruits[i].type==BANANA){
                score+=10;
                 speedBoost = 1;
                 speedBoostTimer = 30;
            };
            /*se comer o abacaxi (bonus) ganha imunidade à colisão contra o corpo da cobra*/
            if(fruits[i].type==PINEAPPLE){
                score+=100;
                immunity=1;
                immunityTimer=IMMUNITY_DURATION;
            }

            generate_fruit(i);
        }
    }


    int currentSpeed = SPEED;
    if(speedBoost){
    currentSpeed /= 2; // 2x mais rápido
    speedBoostTimer--;
    if(speedBoostTimer <= 0){
        speedBoost = 0;
        currentSpeed = SPEED;
        }
    }

    Sleep(currentSpeed);



    if(!ateFruit){
        remove_tail();
    }
        render_delta();
    for(int i=0;i<FRUIT_COUNT;i++)
        drawFruit(fruits[i]);

    draw_score();
    draw_side_panel();
}

/* ================= RESET ================= */

void reset_game(){

    Node *temp;

    if(head){
        Node *current = head->next;
        while(current != head){
            temp=current;
            current=current->next;
            free(temp);
        }
        free(head);
    }

    head=NULL;
    tail=NULL;

    score=0;
    dirX=0;
    dirY=0;
    gameOver=0;
    immunity=0;
    immunityTimer=0;
    paused=0;

    system("cls");

    insert_head(WIDTH/2, HEIGHT/2 + TOP_OFFSET);

    draw_border();
    draw_background();
    init_fruits();

    setColor(COR_CABECA);
    gotoxy(head->x + OFFSET_X, head->y);
    printf("O");

    draw_score();
    draw_side_panel();
// inicializa a  cauda antiga
cauda_ant_x = head->x;
cauda_ant_y = head->y;
}

/* ================= MAIN ================= */

int main(){
  
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
 system("chcp 65001");
    srand(time(NULL));
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    hideCursor();
    carregar_ranking();
    loop_menu();
    while(1){

        reset_game();

        while(!gameOver){
            input();
            update();
            Sleep(SPEED);
        }

        setColor(12);
        gotoxy(2, HEIGHT + TOP_OFFSET + 2);
        
        char nome[20];
    
gotoxy(2, HEIGHT + TOP_OFFSET + 2);
printf("Digite seu nome: ");
scanf("%s", nome);
hideCursor();
adicionar_ranking(nome, score);


int escolha = tela_game_over();

if(escolha == 0){
    continue; // joga de novo
}

if(escolha == 1){
    loop_menu();
}

if(escolha == 2){
    break;
}
   
    
}
}