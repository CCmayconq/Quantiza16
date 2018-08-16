#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <process.h>

//Variaveis Globais
unsigned char Fl = 0; //Floyd Variable
FILE *IN1; //Arquivo de entrada
FILE *OT1; //Arquivo de saida
unsigned int L[13]; //Cabecalho do BMP
unsigned int Al, La; //Altura e Largura da imagem
unsigned char **IMG;
unsigned char **NIL;
unsigned int T; //Temporario
unsigned char P[16][3]; //Paleta de cores para acesso direto (sem calculos)

//Escreve mensagem de ajuda
void EscreveAjuda(){
  printf("Quantiza imagens bitmap true color (24bpp) para 16 cores system palette\n\n");
  printf("Quantiza [/F] arquivo\n");
  printf("/F           Utiliza Floyd-Steinberg\n");
  printf("arquivo      Arquivo a ser quantizado (sem extensao)\n");
}

//Gera a paleta de cores que sera usada (Windows system palette)
void gerapaleta(){
  P[0][0] = 0;
  P[0][1] = 0;
  P[0][2] = 0;
  P[1][0] = 128;
  P[1][1] = 0;
  P[1][2] = 0;
  P[2][0] = 0;
  P[2][1] = 128;
  P[2][2] = 0;
  P[3][0] = 128;
  P[3][1] = 128;
  P[3][2] = 0;
  P[4][0] = 0;
  P[4][1] = 0;
  P[4][2] = 128;
  P[5][0] = 128;
  P[5][1] = 0;
  P[5][2] = 128;
  P[6][0] = 0;
  P[6][1] = 128;
  P[6][2] = 128;
  P[7][0] = 192;
  P[7][1] = 192;
  P[7][2] = 192;
  P[8][0] = 128;
  P[8][1] = 128;
  P[8][2] = 128;
  P[9][0] = 255;
  P[9][1] = 0;
  P[9][2] = 0;
  P[10][0] = 0;
  P[10][1] = 255;
  P[10][2] = 0;
  P[11][0] = 255;
  P[11][1] = 255;
  P[11][2] = 0;
  P[12][0] = 0;
  P[12][1] = 0;
  P[12][2] = 255;
  P[13][0] = 255;
  P[13][1] = 0;
  P[13][2] = 255;
  P[14][0] = 0;
  P[14][1] = 255;
  P[14][2] = 255;
  P[15][0] = 255;
  P[15][1] = 255;
  P[15][2] = 255;
}

//Metodo ineficiente mas funciona para achar a cor mais proxima
unsigned char procuraproxima(unsigned char R, unsigned char G, unsigned char B){
  T = 0;
  int i, T2;
  unsigned char pos = 0;
  T = abs(R-P[0][0])+abs(G-P[0][1])+abs(B-P[0][2]);
  for (i = 1; i < 16; i++){
    T2 = abs(R-P[i][0])+abs(G-P[i][1])+abs(B-P[i][2]);
    if (T2 < T){
      T = T2;
      pos = i;
    }
  }
  return pos;
}

int main(int argc, char const *argv[]) {
  int i, j;
  if (argc != 2 && argc != 3){
    printf("Erro: Quantidade invalida de argumentos\n\n");
    EscreveAjuda();
    return 1;
  } else if (argc == 2){
    printf("argc = 1\n");
  } else if (argc == 3){ //Provavelmente tem Floyd
    if (strcmp(argv[1], "/f") == 0 || strcmp(argv[1], "/F") == 0 || strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "-F") == 0){
      Fl = Fl | 1; //Usa pra nada
    } else { //Argumento Invalido
      printf("Erro: Primeiro argumento nao e solicitacao de Floyd (Pois e, tem que ser o primeiro)\n\n");
      EscreveAjuda();
      return 1;
    }
    char cam[strlen(argv[2])];
    char camN[strlen(argv[2])];
    strcpy(cam, argv[2]);
    strcpy(camN, argv[2]);
    IN1 = fopen(strcat(cam, ".bmp"),"rb"); //Leitura Binaria
    if (IN1 == NULL){
      printf("Erro: Nao foi possivel abrir o arquivo de entrada\n\n");
      EscreveAjuda();
      return 1;
    }
    OT1 = fopen(strcat(camN, "16.bmp"),"wb"); //Arquivo de saida
    if (OT1 == NULL){
      printf("Erro: Nao foi possivel abrir arquivo para saida\n\n");
      EscreveAjuda();
      return 1;
    }

    //Leitura do cabecalho
    fseek(IN1, 2, SEEK_SET); //Pula BM
    fread(&L, sizeof(L), 1, IN1); //Le o cabecalho inteiro
    if (L[2] != 54){ //Se tem paleta
      printf("Erro: O arquivo apresenta paleta de cores; esta operacao pode ser realizada apenas em imagens true color\n\n");
      EscreveAjuda();
      return 1;
    }
    La = L[4]; //Atribui largura
    Al = L[5]; //Atribui altura
    T = La*3;
    IMG=(unsigned char **) malloc(T * sizeof(unsigned char));
    for(i = 0; i < T; i++){
      IMG[i]=(unsigned char *) malloc(Al * sizeof(unsigned char));
    }
    if (!IMG){ //Teste para alocacao?
      printf("\nErro: Nao foi possivel alocar memoria para leitura da imagem\n");
      return 1;
    }
    T = ceil(La/2);
    NIL=(unsigned char **) malloc(T * sizeof(unsigned char));
    for(i = 0; i < T; i++){
      NIL[i]=(unsigned char *) malloc(Al * sizeof(unsigned char));
    }
    if (!NIL){ //Teste para alocacao?
      printf("\nErro: Nao foi possivel alocar memoria para imagem de saida\n");
      return 1;
    }
    for (i = 0; i < Al; i++){ //Leitura da imagem (Fazer: Tratar leituras)
      fread(&IMG[i], 1, La * 3, IN1);
    }

    gerapaleta();
    //A partir daqui a quantizacao acontece
    T = (La * 3) - 3;
    for (i = 0; i < La; i++){
      for (j = 0; j < T; j+=3){
        //Primeira Coluna
        Fl = procuraproxima(IMG[i][j], IMG[i][j+1], IMG[i][j+2]);

      }
    }
  }
  return 0;
}
