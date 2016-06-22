#include "functions.h"
#include "constants.h"

void clean(char* text){

  int i, j, size = strlen(text);
  char auxText[TAM_MAX] = "\0";

  for(i = 0, j = 0; i < size; i++){
    if((text[i] >= A) && (text[i] <= Z)){
      auxText[j] = text[i];
      j++;
    }else if((text[i] >= A_) && (text[i] <= Z_)){
      auxText[j] = toupper(text[i]);
      j++;
    }
  }

  strcpy(text, auxText);
}

void aCharCrypt(char cIn, char* cOut, mpz_t a, mpz_t b){
  mpz_t ch, res, norm, m;

  mpz_init_set_ui(norm, A);
  mpz_init_set_ui(ch, cIn - A);
  mpz_init_set_ui(m, ALPHSIZE);
  mpz_init(res);

  mpz_mul(res, ch, a);
  mpz_add(res, res, b);
  mpz_mod(res, res, m);
  mpz_add(res, res, norm);

  cOut[0] = mpz_get_ui(res);

  mpz_clear(ch);
  mpz_clear(res);
  mpz_clear(norm);
  mpz_clear(m);
}

void aCrypt(char* textIn, char* textOut){
  srand(time(NULL));
  unsigned int aux;

  mpz_t a, b;

  mpz_init(a);
  mpz_init(b);

  textOut[strlen(textIn)] = '\0';
  for(int i = 0; i < strlen(textIn); i++){
    aux = rand() % ASIZE;
    mpz_set_ui(a, POSSIBA[aux]);

    aux = rand() % BSIZE;
    mpz_set_ui(b, POSSIBB[aux]);

    aCharCrypt(textIn[i], &textOut[i], a, b);
  }

  mpz_clear(a);
  mpz_clear(b);
}

void aCryptNoRnd(char* textIn, char* textOut){
  srand(time(NULL));
  unsigned int aux;

  mpz_t a, b;

  mpz_init(a);
  mpz_init(b);

  textOut[strlen(textIn)] = '\0';
  for(int i = 0; i < strlen(textIn); i++){
    if(i % 2 == 0){
      aux = 10 + rand() % 10;
      mpz_set_ui(a, POSSIBA[aux]);

      aux = rand() % BSIZE/2;
      mpz_set_ui(b, POSSIBB[aux]);
    }else{
      aux = rand() % ASIZE;
      mpz_set_ui(a, POSSIBA[aux]);

      aux = rand() % BSIZE;
      mpz_set_ui(b, POSSIBB[aux]);
    }

    aCharCrypt(textIn[i], &textOut[i], a, b);
  }

  mpz_clear(a);
  mpz_clear(b);
}

void printProbs(char* textIn, FILE* printer){
  int letters = ALPHSIZE, size = strlen(textIn);
  double prob[letters];
  for(int i = 0; i < size; i++){
    prob[textIn[i] - A]++;
  }
  for(int i = 0; i < letters; i++){
    if(printer != NULL)
      fprintf(printer, "P(%c)=%.2f\n", i+A, prob[i]/size);
    else
      printf("P(%c)=%.2f\n", i+A, prob[i]/size);
  }
}

void printCondProbs(char* textIn, char* textOut, FILE* printer){
  int letters = ALPHSIZE, size = strlen(textOut);

  double prob[letters][letters], probS[letters];

  for(int i = 0; i < letters; i++){
    for(int j = 0; j < letters; j++){
      prob[i][j] = 0;
      probS[i] = 0;
    }
  }

  for(int i = 0; i < size; i++){
    prob[textIn[i] - A][textOut[i] - A]++;
  }

  for(int i = 0; i < size; i++){
    probS[textOut[i] - A]++;
  }

  for(int i = 0; i < letters; i++){
    for(int j = 0; j < letters; j++){
      prob[i][j]/=probS[j];
    }
  }

  double auxSum, dt, dtSum, med;
  for(int i = 0; i < letters; i++){
    auxSum = 0;
    dtSum = 0;
    for(int j = 0; j < letters; j++){
      auxSum += prob[i][j];
      if(printer != NULL)
        fprintf(printer, "P(%c/%c)=%.2f ", i+A, j+A, prob[i][j]);
      else
        printf("P(%c/%c)=%.2f ", i+A, j+A, prob[i][j]);
    }
    med = auxSum/letters;
    for(int j = 0; j < letters; j++){
      dtSum += (prob[i][j]-med)*(prob[i][j]-med);
    }
    dt = dtSum/letters;
    dt = sqrt(dt);
    if(printer != NULL)
      fprintf(printer, " ====> ~P(%c)=%.2f DT(%c)=%.2f\n", i+A, med, i+A, dt);
    else
      printf(" ====> ~P(%c)=%.2f DT(%c)=%.2f\n", i+A, med, i+A, dt);
  }
}
