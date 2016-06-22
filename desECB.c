#include "desECB.h"

int main (int argc, char *argv[]) {
  srand(time(NULL));
  char option, *keyChar;
  FILE *bufferIn = NULL, *bufferOut = NULL;
  unsigned char textIn[TAM_MAX], textOut[TAM_MAX];
  int stopOnEOL = 0;

  if((argc<2)||(argc>6)){
		printf("Error en los parametros de entrada.\n");
		printf("Uso: desECB {-C | -D -k key} [-i fileIn] [-o fileOut]\n");
		return ERR;
	}

  // Se comprueban los comandos
  if(strcmp(argv[1], "-C")==0){
    option='C';
  }else if(strcmp(argv[1], "-D") == 0 && strcmp(argv[2], "-k") == 0){
    option='D';
    keyChar=argv[3];
  }else return ERR;

  // Se comprueba el I/O
  if(option=='C'){
    if(argc>=4){
      if(strcmp(argv[2], "-i")==0){
        bufferIn = fopen(argv[3],"r");
        if(bufferIn == NULL){
          printf("Error abriendo el archivo de entrada.\n");
          return ERR;
        }
        if(argc>=6){
          if(strcmp(argv[4], "-o")==0){
            bufferOut = fopen(argv[5],"w+");
            if(bufferOut == NULL){
              printf("Error abriendo el archivo de salida.\n");
              return ERR;
            }
          }
        }
      }else if(strcmp(argv[2], "-o")==0){
        bufferOut = fopen(argv[3],"w+");
        if(bufferOut == NULL){
          printf("Error abriendo el archivo de salida.\n");
          return ERR;
        }
      }
    }
  }else{
    if(argc>=6){
      if(strcmp(argv[4], "-i")==0){
        bufferIn = fopen(argv[5],"r");
        if(bufferIn == NULL){
          printf("Error abriendo el archivo de entrada.\n");
          return ERR;
        }
        if(argc>=6){
          if(strcmp(argv[6], "-o")==0){
            bufferOut = fopen(argv[8],"w+");
            if(bufferOut == NULL){
              printf("Error abriendo el archivo de salida.\n");
              return ERR;
            }
          }
        }
      }else if(strcmp(argv[4], "-o")==0){
        bufferOut = fopen(argv[5],"w+");
        if(bufferOut == NULL){
          printf("Error abriendo el archivo de salida.\n");
          return ERR;
        }
      }
    }
  }

  if(bufferOut == NULL){
    bufferOut = stdout;
  }
  if(bufferIn == NULL){
    stopOnEOL = 1;
    if(option=='C')
        printf("\nEscriba el texto a encriptar:\n");
    else if(option=='D')
        printf("\nEscriba el texto a desencriptar:\n");
    bufferIn = stdin;
  }

    // size_t len = fread(textIn, 1, TAM_MAX, bufferIn);
    // textIn[len] = '\0';
  short* bits;
  short* encoded;
  short key[BITS_IN_IP] = {
  	1, 1, 0, 1, 0, 1, 1, 0,
  	0, 1, 0, 1, 1, 0, 1, 0,
  	0, 1, 0, 1, 0, 0, 1, 0,
  	0, 1, 0, 0, 0, 0, 1, 0,
  	1, 0, 0, 1, 1, 1, 1, 1,
  	1, 1, 0, 1, 1, 0, 1, 1,
  	0, 1, 0, 1, 0, 1, 1, 1,
  	0, 1, 0, 1, 1, 1, 1, 0
  };
  if(stopOnEOL){
    while(1){
      fgets(textIn, TAM_MAX, bufferIn);
      unsigned char* point = strstr(textIn, "\n");
      if(point){
        fillChars(textIn, point - textIn);
      }
      bits = charsToBits(textIn);
      encoded = DESBlock(bits, key);

      // for(int  i = 0; i < BITS_IN_IP; i++)
      //   fprintf(bufferOut, "%i", encoded[i]);
      //ENCRIPTAR PASANDO el bufferOut
      if(point)
        break;
    }
  }else{

  }

  if(bufferIn != NULL && bufferIn != stdin) fclose(bufferIn);

  if(bufferOut != NULL && bufferOut != stdout) fclose(bufferOut);

  return(0);
}

void fillChars(unsigned char* text, int pos){
  for(int i = pos; i < TAM_MAX; i++)
    text[i] = rand() % 128;
}

short* charsToBits(unsigned char* bytes){
  static short bits[BITS_IN_IP];
  int mask; /* 10000000 */

  for(int i = 0, j = 0; i < TAM_MAX; i++){
    mask = 0x80;
    while (mask>0) {
      bits[j] = (bytes[i] & mask) > 0;
      mask >>= 1; /* move the bit down */
      j++;
    }
  }

  return bits;
}

short* DESBlock(short* bits, short* key){
  static short perm1[BITS_IN_IP], lastL[BITS_IN_P], lastR[BITS_IN_P], nextL[BITS_IN_P], nextR[BITS_IN_P];

  for(int i = 0; i < BITS_IN_IP; i++){
    perm1[i] = bits[IP[i]-1];
  }

  for(int i = 0; i < BITS_IN_P; i++){
    lastL[i] = perm1[i];
    lastR[i] = perm1[i+BITS_IN_P];
  }

  for(int i = 0; i < ROUNDS; i++){
    //nextL = lastR;
  }

  return perm1;
}

void keysRound(short* key, short** keysR){
  short c[BITS_IN_PC1/2], d[BITS_IN_PC1/2];

  for(int j = 0; j < BITS_IN_PC1/2; j++){
    c[j] = key[PC1[j]-1];
    d[j] = key[PC1[j+BITS_IN_PC1/2]-1];
  }

  for(int i = 0; i < ROUNDS; i++){
    for(int j = 0; j < ROUND_SHIFTS[i]; j++){
      leftShift(c, BITS_IN_PC1/2);
      leftShift(d, BITS_IN_PC1/2);
    }
    for(int j = 0; j < BITS_IN_PC2; j++){
      if(PC2[j] < BITS_IN_PC1/2)
        keysR[i][j] = c[PC2[j]-1];
      else
        keysR[i][j] = d[PC2[j]-(BITS_IN_PC1/2)-1];
    }
  }
}

void leftShift(short* bits, int size){
  int aux = bits[size-1];
  bits[size-1] = bits[0];
  bits[0] = aux;
}

void sumMod2(short* bits1, short* bits2, short* sum, int size){
  for(int i = 0; i < size; i++){
    if(bits1[i] == bits2[i])
      sum[i] = 0;
    else
      sum[i] = 1;
  }
}

void cipherFunc(short* bitsR, short* bitsK, short* result){
  short expE[BITS_IN_E];

}
