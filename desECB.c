#include "desECB.h"

int main (int argc, char *argv[]) {
  srand(time(NULL));
  char option;
  short key[BITS_IN_IP];
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
    for(int i = 0; i < BITS_IN_IP; i++)
    	key[i]=argv[3][i] - '0';
  }else return ERR;

  // Se comprueba el I/O
  if(option=='C'){
    if(argc>=4){
      if(strcmp(argv[2], "-i")==0){
        bufferIn = fopen(argv[3],"rb");
        if(bufferIn == NULL){
          printf("Error abriendo el archivo de entrada.\n");
          return ERR;
        }
        if(argc>=6){
          if(strcmp(argv[4], "-o")==0){
            bufferOut = fopen(argv[5],"w+b");
            if(bufferOut == NULL){
              printf("Error abriendo el archivo de salida.\n");
              return ERR;
            }
          }
        }
      }else if(strcmp(argv[2], "-o")==0){
        bufferOut = fopen(argv[3],"w+b");
        if(bufferOut == NULL){
          printf("Error abriendo el archivo de salida.\n");
          return ERR;
        }
      }
    }
  }else{
    if(argc>=6){
      if(strcmp(argv[4], "-i")==0){
        bufferIn = fopen(argv[5],"rb");
        if(bufferIn == NULL){
          printf("Error abriendo el archivo de entrada.\n");
          return ERR;
        }
        if(argc>=6){
          if(strcmp(argv[6], "-o")==0){
            bufferOut = fopen(argv[8],"w+b");
            if(bufferOut == NULL){
              printf("Error abriendo el archivo de salida.\n");
              return ERR;
            }
          }
        }
      }else if(strcmp(argv[4], "-o")==0){
        bufferOut = fopen(argv[5],"w+b");
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
    freopen(NULL, "rb", bufferIn);
  }

    // size_t len = fread(textIn, 1, TAM_MAX, bufferIn);
    // textIn[len] = '\0';
  short bits[BITS_IN_IP];
  short keys[ROUNDS][BITS_IN_PC2];
  short encoded[BITS_IN_IP];
  if(stopOnEOL){ //0000000100100011010001010110011110001001101010111100110111101111
    while(1){
      unsigned char input[TAM_MAX*TAM_MAX+1];
      memset(&input[0], -1, sizeof(input));
      fgets(input, TAM_MAX*TAM_MAX+1, bufferIn);
      unsigned char* point = strstr(input, "\n");
      for(int i = 0; i < TAM_MAX*TAM_MAX; i++){
		  if(input[i] == '0')
			  bits[i] = 0;
		  else if(input[i] == '1')
			  bits[i] = 1;
		  else
			  bits[i] = rand()%2;
      }
      keysRound(key, keys);
      DESBlock(bits, keys, encoded);

      for(int  i = 0; i < BITS_IN_IP; i++)
    	  fprintf(bufferOut, "%i", encoded[i]);
      //ENCRIPTAR PASANDO el bufferOut
      if(point)
        break;
    }
  }else{ //TODO: revisar
	  while(1){
			size_t point = fread(textIn, 1, TAM_MAX+1, bufferIn);
			if(point < TAM_MAX){
			  fillChars(textIn, TAM_MAX - point);
			}
			keysRound(key, keys);
			charsToBits(textIn, bits);
			//encoded = DESBlock(bits, keys);

			for(int  i = 0; i < BITS_IN_IP; i++)
			  fprintf(bufferOut, "%i", bits[i]);
			fprintf(bufferOut, "%i", 1234);
			//ENCRIPTAR PASANDO el bufferOut
			if(point != TAM_MAX)
			  break;
	  }
  }

  if(bufferIn != NULL && bufferIn != stdin) fclose(bufferIn);

  if(bufferOut != NULL && bufferOut != stdout) fclose(bufferOut);

  return(0);
}

void fillChars(unsigned char* text, int pos){
  for(int i = pos; i < TAM_MAX; i++)
    text[i] = rand() % 128;
}

void charsToBits(unsigned char* bytes, short* bits){
  int mask; /* 10000000 */

  for(int i = 0, j = 0; i < TAM_MAX; i++){
    mask = 0x80;
    while (mask>0) {
      bits[j] = (bytes[i] & mask) > 0;
      mask >>= 1; /* move the bit down */
      j++;
    }
  }
}

void DESBlock(short* bits, short keys[ROUNDS][BITS_IN_PC2], short* perm){
  static short perm1[BITS_IN_IP], lastL[BITS_IN_P], lastR[BITS_IN_P], nextL[BITS_IN_P], nextR[BITS_IN_P],
				cFunc[BITS_IN_P];

  for(int i = 0; i < BITS_IN_IP; i++){
    perm1[i] = bits[IP[i]-1];
  }

  for(int i = 0; i < BITS_IN_P; i++){
    lastL[i] = perm1[i];
    lastR[i] = perm1[i+BITS_IN_P];
  }

  for(int i = 0; i < ROUNDS; i++){
	for(int j = 0; j < BITS_IN_P; j++){
		nextL[j] = lastR[j];
	}
    cipherFunc(lastR, keys[i], cFunc);
    sumMod2(lastL, cFunc, nextR, BITS_IN_P);
    for(int j = 0; j < BITS_IN_P; j++){
    	lastR[j] = nextR[j];
    	lastL[j] = nextL[j];
    }
  }

  for(int i = 0; i < BITS_IN_IP; i++){
	  if(i < BITS_IN_IP/2)
		  perm1[i]=nextR[i];
	  else
		  perm1[i]=nextL[i-BITS_IN_IP/2];
  }

  for(int i = 0; i < BITS_IN_IP; i++){
  	  perm[i] = perm1[IP_INV[i]-1];
    }
}

void keysRound(short* key, short keysR[ROUNDS][BITS_IN_PC2]){
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
      if((PC2[j] - 1) < BITS_IN_PC1/2)
        keysR[i][j] = c[PC2[j]-1];
      else
        keysR[i][j] = d[PC2[j]-(BITS_IN_PC1/2)-1];
    }
  }
}

void leftShift(short* bits, int size){
  short aux = bits[0];
  for(int i = 0; i < size-1; i++){
	  bits[i] = bits[i+1];
  }
  bits[size-1] = aux;
}

void sumMod2(short* bits1, short* bits2, short* sum, int size){
  for(int i = 0; i < size; i++){
    if(bits1[i] == bits2[i])
      sum[i] = 0;
    else
      sum[i] = 1;
  }
}

void cipherFunc(short* bitsR, short* bitsK, short* result){ //48bit, 48bit, 32bit
  short expE[BITS_IN_E], sum[BITS_IN_E], resultP[BITS_IN_P];
  short auxI, auxJ, auxSOut;
  short SiN = 6, SOut = 4;

  for(int i = 0; i < BITS_IN_E; i++){
	  expE[i] = bitsR[E[i]-1];
  }

  sumMod2(expE, bitsK, sum, BITS_IN_E);

  for(int i = 0; i < NUM_S_BOXES; i++){
	  auxI = (sum[i*SiN] << 1) + sum[i*SiN + 5];
	  auxJ = (sum[i*SiN+1] << 3) + (sum[i*SiN+2] << 2) + (sum[i*SiN+3] << 1) + (sum[i*SiN+4]);
	  auxSOut = S_BOXES[i][auxI][auxJ];
	  for(int j = 0; j < SOut; j++){
		  resultP[i*4+j] = (auxSOut & (1 << (SOut - j - 1))) >> (SOut - j - 1);
	  }
  }

  for(int i = 0; i < BITS_IN_P; i++){
	  result[i] = resultP[P[i]-1];
  }

}
