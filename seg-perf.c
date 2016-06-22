#include <stdio.h>
#include <stdlib.h>
#include "functions.c"

// gcc seg-perf.c -lgmp -L. -lm

int main (int argc, char *argv[]) {
  int flagFileIn = 0, flagFileOut = 0;
  char option;
  FILE *fileIn, *fileOut;
  char textIn[TAM_MAX], textOut[TAM_MAX];

  if((argc<2)||(argc>6)){
		printf("Error en los parametros de entrada.\n");
		printf("Uso: seg-perf {-P | -I} [-i fileIn] [-o fileOut]\n");
		return ERR;
	}

  // Se comprueban los comandos
  if(strcmp(argv[1], "-P")==0){
    option='P';
  }else if(strcmp(argv[1], "-I")==0){
    option='I';
  }else return ERR;

  // Se comprueba el I/O
	if(argc>=4){
		if(strcmp(argv[2], "-i")==0){
			fileIn = fopen(argv[3],"r");
			if(fileIn == NULL){
				printf("Error abriendo el archivo de entrada.\n");
				return ERR;
			}
			flagFileIn = 1;
			if(argc>=6){
				if(strcmp(argv[4], "-o")==0){
					fileOut = fopen(argv[5],"w+");
					if(fileOut == NULL){
						printf("Error abriendo el archivo de salida.\n");
						return ERR;
					 }
					flagFileOut = 1;
				}
			}
		}else if(strcmp(argv[2], "-o")==0){
				fileOut = fopen(argv[3],"w+");
				if(fileOut == NULL){
					printf("Error abriendo el archivo de salida.\n");
					return ERR;
				}
				flagFileOut = 1;
		}
	}

  if(!flagFileIn){
		if(option=='P')
			printf("\nEscriba el texto a encriptar:\n");
		else if(option=='I')
			printf("\nEscriba el texto a desencriptar:\n");

		fgets(textIn, TAM_MAX, stdin);
	}else{
    size_t len = fread(textIn, 1, TAM_MAX, fileIn);
    textIn[len] = '\0';
	}

  clean(textIn);

	if(option=='P'){
		aCrypt(textIn, textOut);
	}else{
		aCryptNoRnd(textIn, textOut);
	}

	if(!flagFileOut){
    printProbs(textIn, NULL);
    printCondProbs(textIn, textOut, NULL);
	}
	else{
    printProbs(textIn, fileOut);
    printCondProbs(textIn, textOut, fileOut);
	}

  if(flagFileIn) fclose(fileIn);

  if(flagFileOut) fclose(fileOut);

  return(0);
}
