#include "desECB.h"

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

void bitsToChars(short* bits, unsigned char* bytes){
  int aux;

  for(int i = 0, j = 0; i < TAM_MAX; i++){ // TAM_MAX
    aux = 7;
    bytes[i] = 0;
    while (aux >= 0) {
      bytes[i] |= bits[j] << aux;
      j++;
      aux--;
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

int checkParity(short* key){
  int sum;
  for(int i = 0; i < BITS_IN_IP; i++){
    if((i+1)%8 == 0){
      sum = 0;
      for(int j = (i - 1); j > (i - 8); j--){
        if(key[j])
          sum++;
      }
      if(sum%2 == key[i]){
        return 0;
      }
    }
  }
  return 1;
}

void reverseKeys(short keys[ROUNDS][BITS_IN_PC2]){
  short aux[BITS_IN_PC2];
  for (int i = 0; i < ROUNDS/2; i++){
    for (int j = 0; j < BITS_IN_PC2; j++){
      aux[j] = keys[i][j];
      keys[i][j] = keys[ROUNDS-i-1][j];
      keys[ROUNDS-i-1][j] = aux[j];
    }
  }
}

void genKey(short* key){
  int sum;
  for (int i = 0; i < BITS_IN_IP; i++){
    if((i+1)%8 == 0){
      sum = 0;
      for(int j = (i - 1); j > (i - 8); j--){
        if(key[j])
          sum++;
      }
      key[i] = !(sum%2);
    }else{
      key[i] = rand()%2;
    }
  }
}
