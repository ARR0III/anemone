/*
  Block crypt algoritm Anemone-1;
  SP-cipher;
  
  block =  16 byte;
  key   = 256 byte;
*/
#ifndef _C_STDIO_H_
#define _C_STDIO_H_
  #include <stdio.h>
#endif

#ifndef _C_STDINT_H_
#define _C_STDINT_H_
  #include <stdint.h>
#endif

#ifndef _C_STDLIB_H_
#define _C_STDLIB_H_
  #include <stdlib.h>
#endif

#include "xtalw.h"

#define Rounds      32
#define BLOCK_SIZE  16
#define KEY_LENGTH 256

typedef struct {
  int32_t  operation;
  int32_t  position;
  uint32_t white[4];
  uint8_t  table[KEY_LENGTH];
} ANEMONE_CTX;

uint8_t zbox[] = {
 0x0F, 0x4B, 0x87, 0xC3,
 0x1E, 0x5A, 0x96, 0xD2,
 0x2D, 0x69, 0xA5, 0xE1,
 0x3C, 0x78, 0xB4, 0xF0
};

void swap (uint8_t * a, uint8_t * b) {
  uint8_t t = *a;

  *a = *b;
  *b = t;
}

void anemone_init(ANEMONE_CTX * ctx, uint8_t * key, int key_len, int operation) {
  uint32_t i, k = 0;

  ctx->operation = operation;
  
  if (operation == 0xDE) {
    ctx->position = KEY_LENGTH - (BLOCK_SIZE / 2);
  }
  else {
    ctx->position = 0;
  }
  
  for (i = 0; i < KEY_LENGTH; ++i) {
    ctx->table[i] = (uint8_t)i;
  }
  
  for (i = 0; i < KEY_LENGTH; ++i) {
    k = key[i % key_len] + ctx->table[i % KEY_LENGTH] + key_len + zbox[k % BLOCK_SIZE] + k;
    
    swap((uint8_t *)&ctx->table[i], (uint8_t *)&ctx->table[k % KEY_LENGTH]);
  }
  
  for (i = 0; i < 4; i++) {
    ctx->white[i] = 0x00000000;
    
    for (int j = 0; j < (KEY_LENGTH / 4); j += 4) {
      ctx->white[i] ^= *(((uint32_t *)&ctx->table[0]) + i + j);
    }
  }
}

void pos_de(ANEMONE_CTX * ctx) {
  ctx->position -= (BLOCK_SIZE / 2);
  
  if (ctx->position < 0) {
    ctx->position = KEY_LENGTH - (BLOCK_SIZE / 2);
  }
}

void pos_en(ANEMONE_CTX * ctx) {
  ctx->position += (BLOCK_SIZE / 2);
    
  if (ctx->position >= KEY_LENGTH) {
    ctx->position = 0;
  }
}

/*
  Функция FX(x) создает 32-битную гамму, зависящую от
  открытого текста/шифротекста, 8-битного значения таблицы
  "размытия" и байта ключа шифрования, для последующего
  объединения битовым xor с 32-битным значением раундового
  ключа.
*/
uint32_t FX(ANEMONE_CTX * ctx, uint32_t X) {
/*
  if x == 0 then {
    t =  table[0]; ???
    t = (0 + ???) ^ zbox[???];
    
    return (0 * ???); == 0;
  }
*/
  uint8_t t = ctx->table[(X >> 24) ^ (X & 0x000000FF)];
          t = (X + (uint32_t)t) ^ (uint32_t)(zbox[t % BLOCK_SIZE]);
          
  return (uint32_t)(X * t);
}

/* Substitution-Permutation network */
void sp_en(ANEMONE_CTX * ctx, uint8_t * temp) {
  uint32_t t;
    
  uint32_t * L0 = (uint32_t *)temp + 0; // 1
  uint32_t * R0 = (uint32_t *)temp + 1; // 2
  
  uint32_t * L1 = (uint32_t *)temp + 2; // 3
  uint32_t * R1 = (uint32_t *)temp + 3; // 4
  
  uint32_t KEY;
  
  /*---------------------------------------------------*/
  KEY  = *((uint32_t *)(ctx->table + ctx->position) + 0) ^ FX(ctx, *R0); /* ^ *R0; */
  *L0 += KEY;
  /*---------------------------------------------------*/
  
    t = *L0;
  *L0 = *R0;
  *R0 = t;
  
  /*---------------------------------------------------*/
  KEY  = *((uint32_t *)(ctx->table + ctx->position) + 1) ^ FX(ctx, *R1); /* ^ *R1; */
  *L1 += KEY;
  /*---------------------------------------------------*/
  
    t = *L1;
  *L1 = *R1;
  *R1 = t;
  
  pos_en(ctx);
}

/* Substitution-Permutation network */
void sp_de(ANEMONE_CTX * ctx, uint8_t * temp) {
  uint32_t t;
    
  uint32_t * L0 = (uint32_t *)temp + 0; // 1
  uint32_t * R0 = (uint32_t *)temp + 1; // 2
  
  uint32_t * L1 = (uint32_t *)temp + 2; // 3
  uint32_t * R1 = (uint32_t *)temp + 3; // 4
  
  uint32_t KEY;
  
  /*---------------------------------------------------*/
  KEY  = *((uint32_t *)(ctx->table + ctx->position) + 0) ^ FX(ctx, *L0); /* ^ *L0; */
  *R0 -= KEY;
  /*---------------------------------------------------*/
  
    t = *L0;
  *L0 = *R0;
  *R0 = t;
  
  /*---------------------------------------------------*/
  KEY  = *((uint32_t *)(ctx->table + ctx->position) + 1) ^ FX(ctx, *L1); /* ^ *L1; */
  *R1 -= KEY;
  /*---------------------------------------------------*/
  
    t = *L1;
  *L1 = *R1;
  *R1 = t;
  
  pos_de(ctx);
}

/*  1  2  3  4   16  2  3 13
    5  6  7  8    5 11 10  8
    9 10 11 12    9  7  6 12
   13 14 15 16 -> 4 14 15  1 */
void sonne(uint8_t * temp) {
  uint8_t t = temp[3];
  temp[3] = temp[12];
  temp[12] = t;
  
  t = temp[0];
  temp[0] = temp[15];
  temp[15] = t;
  
  t = temp[5];
  temp[5] = temp[10];
  temp[10] = t;
  
  t = temp[6];
  temp[6] = temp[9];
  temp[9] = t;
}
/*
  Процедура отбеливания входных данных,
  объединением с white-таблицей, сгенерированной
  из 256-байтного ключа шифрования.
*/
void whitening(ANEMONE_CTX * ctx, uint8_t * temp) {
  uint32_t * ptemp = (uint32_t *)temp;
  
  for (int i = 0; i < 4; i++) {
    *(ptemp + i) ^= ctx->white[i];
  }
}

void anemone_encrypt(ANEMONE_CTX * ctx, uint8_t * in, uint8_t * out) {
  uint8_t temp[BLOCK_SIZE];

  memcpy(temp, in, BLOCK_SIZE);
  
  whitening(ctx, temp);
  
  for (int i = 0; i < Rounds; ++i) {
    sp_en(ctx, temp);
    sonne(temp);
    
    /* printhex(HEX_STRING, temp, BLOCK_SIZE); */
  }
  
  memcpy(out, temp, BLOCK_SIZE);
}

void anemone_decrypt(ANEMONE_CTX * ctx, uint8_t * in, uint8_t * out) {
  uint8_t temp[BLOCK_SIZE];
  
  memcpy(temp, in, BLOCK_SIZE);
  
  for (int i = 0; i < Rounds; ++i) {
    sonne(temp);
    sp_de(ctx, temp);
    
    /* printhex(HEX_STRING, temp, BLOCK_SIZE); */
  }

  whitening(ctx, temp);
  
  memcpy(out, temp, BLOCK_SIZE);
}

