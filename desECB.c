#include "desFuncs.c"

int main (int argc, char *argv[]) {
  srand(time(NULL));
  char option;
  short key[BITS_IN_IP];
  FILE *bufferIn = NULL, *bufferOut = NULL;
  unsigned char textIn[TAM_MAX];
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

  if(option=='D'){
    if(!checkParity(key)){
      printf("No se cumplen los requisitos de paridad en la clave.\n");
      return ERR;
    }
  }else if(option=='C'){
    genKey(key);
    printf("La clave generada es: ");
    for(int i = 0; i < BITS_IN_IP; i++)
    	printf("%i", key[i]);
    printf("\n");
  }

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
        if(argc>=8){
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


  unsigned char textOut[TAM_MAX];
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

      if(option=='D'){
        reverseKeys(keys);
      }

      DESBlock(bits, keys, encoded);


      for(int  i = 0; i < BITS_IN_IP; i++)
    	  fprintf(bufferOut, "%i", encoded[i]);

      if(point)
        break;
    }
  }else{ //TODO: revisar
    unsigned char encodedC[TAM_MAX];
	  while(1){
			size_t point = fread(textIn, 1, TAM_MAX, bufferIn);
			if(point < TAM_MAX){
			  fillChars(textIn, point);
			}

			keysRound(key, keys);

      if(option=='D'){
        reverseKeys(keys);
      }

			charsToBits(textIn, bits);

			DESBlock(bits, keys, encoded);

      bitsToChars(encoded, encodedC);

      for(int  i = 0; i < TAM_MAX; i++)
			  fprintf(bufferOut, "%c", encodedC[i]);

			//ENCRIPTAR PASANDO el bufferOut
			if(point != TAM_MAX)
			  break;
	  }
  }

  if(bufferIn != NULL && bufferIn != stdin) fclose(bufferIn);

  if(bufferOut != NULL && bufferOut != stdout) fclose(bufferOut);

  return(0);
}
