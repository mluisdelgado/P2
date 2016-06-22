#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <time.h>
#include <math.h>

void clean(char* text);

void aCharCrypt(char cIn, char* cOut, mpz_t a, mpz_t b);

void aCrypt(char* textIn, char* textOut);

void printCondProbs(char* textIn, char* textOut, FILE* printer);

void printProbs(char* textIn, FILE* printer);
