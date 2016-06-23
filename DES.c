#include "desFuncs.c"

int main (int argc, char *argv[]) {
  srand(time(NULL));
  char option;
  FILE *bufferOut = NULL;
  int nIterations;

  if((argc<2)||(argc>6)){
		printf("Error en los parametros de entrada.\n");
		printf("Uso: DES {-S | -B} { -N n} [-o fileOut]\n");
		return ERR;
	}

  // Se comprueban los comandos
  if(strcmp(argv[1], "-S")==0){
    option='S';
  }else if(strcmp(argv[1], "-B") == 0){
    option='B';
  }else return ERR;

  if(strcmp(argv[2], "-N")==0){
    nIterations = atoi(argv[3]);
  }else return ERR;

  // Se comprueba el OUT
  if(argc>=5){
    if(strcmp(argv[4], "-o")==0){
      bufferOut = fopen(argv[5],"w+");
      if(bufferOut == NULL){
        printf("Error abriendo el archivo de salida.\n");
        return ERR;
      }
    }
  }

  if(bufferOut == NULL){
    bufferOut = stdout;
  }

  short generatedBits[SBOX_IN];
  double result2[8][SBOX_OUT];
  double result[8][SBOX_OUT];

  genBits(generatedBits);

  fprintf(bufferOut, "NITERATIONS: %i\n", nIterations);

  for(int i = 0; i < nIterations; i++){
    getSAC(generatedBits, result2);
    fprintf(bufferOut, "SBOXES:\n");
    for(int j = 0; j < 8; j++){
      for(int k = 0; k < SBOX_OUT; k++){
        fprintf(bufferOut, "P(%i)%f", j, result2[j][k]);
        fprintf(bufferOut, "\n");
        result[j][k] += result2[j][k];
      }
    }
  }

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < SBOX_OUT; j++){
      result[i][j] /= nIterations;
    }
  }

  // fprintf(bufferOut, "BITS GENERADOS: ");
  // for(int i = 0; i < SBOX_IN; i++)
  //   fprintf(bufferOut, "%i", generatedBits[i]);
  // fprintf(bufferOut, "\n");

  // calcSBox(generatedBits, 0, result);
  // fprintf(bufferOut, "SBOXES:\n");
  // for(int i = 0; i < 8; i++){
  //   for(int j = 0; j < SBOX_OUT; j++){
  //     fprintf(bufferOut, "P(%i)%f", j, result[i][j]);
  //     fprintf(bufferOut, "\n");
  //   }
  // }


  if(bufferOut != NULL && bufferOut != stdout) fclose(bufferOut);

  return(0);
}

void genBits(short *bits){
  for(short i = 0; i < SBOX_IN; i++){
    bits[i] = rand() % 2;
  }
}

void getSAC(short *bits, double res[8][SBOX_OUT]){
  short initialRes[8][SBOX_OUT];

  short resP[8][SBOX_OUT];

  double sum[8][SBOX_OUT];

  short bAux[SBOX_IN];

  for(short i = 0; i < 8; i++){
    calcSBox(bits, i, initialRes[i]);
  }

  for(short i = 0; i < SBOX_IN; i++){ // para cada bit de entrada (cambiado)
    restoreBits(bits, bAux); // meto bits de entrada en bAux (restaurados)
    bAux[i] = !bAux[i]; // invierto el bit i
    for(short j = 0; j < 8; j++){ // calculo las sBoxes con bits cambiados
      calcSBox(bAux, j, resP[j]);
    }
    for(short j = 0; j < 8; j++){ // para cada sbox
      for(short k = 0; k < SBOX_OUT; k++){ // para cada bit de salida
        if(resP[j][k] != initialRes[j][k]){
          sum[j][k]++;
        }
      }
    }
  }

  for(short j = 0; j < 8; j++){ // para cada sbox
    for(short k = 0; k < SBOX_OUT; k++){ // para cada bit de salida
      res[j][k] = sum[j][k] / SBOX_IN; // saco las prob de cambio
    }
  }
}

void calcSBox(short *bits, short boxIndex, short* res){
  short iB = (bits[0] << 1) + bits[5];
  short jB = (bits[1] << 3) + (bits[2] << 2) + (bits[3] << 1) + (bits[4]);
  short decValue = S_BOXES[boxIndex][iB][jB];
  for(int k = 0; k < SBOX_OUT; k++){
    res[k] = ((decValue & (1 << (SBOX_OUT - k - 1))) > 0);
  }
}
void restoreBits(short* bits, short* restored){
  for(short i = 0; i < SBOX_IN; i++){
    restored[i] = bits[i];
  }
}
