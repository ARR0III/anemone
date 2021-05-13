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
  uint8_t  table[KEY_LENGTH];
} ZWES_CTX;

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

void anemone_init(ZWES_CTX * ctx, uint8_t * key, int key_len, int operation) {
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
}

void pos_de(ZWES_CTX * ctx) {
  ctx->position -= (BLOCK_SIZE / 2);
  
  if (ctx->position < 0)
    ctx->position = KEY_LENGTH - (BLOCK_SIZE / 2);
}

void pos_en(ZWES_CTX * ctx) {
  ctx->position += (BLOCK_SIZE / 2);
    
  if (ctx->position >= KEY_LENGTH)
    ctx->position = 0;
}

/* reverse buffer 01234567 -> 76543210 */
void reverse(uint8_t * temp) {
 uint8_t t = temp[0];
 temp[0] = temp[7];
 temp[7] = t;

 t = temp[1];
 temp[1] = temp[6];
 temp[6] = t;
 
 t =  temp[2];
 temp[2] = temp[5];
 temp[5] = t;
 
 t = temp[3];
 temp[3] = temp[4];
 temp[4] = t;
}

/* Substitution-Permutation network */
void sp_en(ZWES_CTX * ctx, uint8_t * temp) {
  uint32_t t;
    
  uint32_t * L0 = (uint32_t *)temp + 0; // 1
  uint32_t * R0 = (uint32_t *)temp + 1; // 2
  
  uint32_t * L1 = (uint32_t *)temp + 2; // 3
  uint32_t * R1 = (uint32_t *)temp + 3; // 4
  
  uint32_t KEY;
  
  /*---------------------------------------------------*/
  KEY  = *((uint32_t *)(ctx->table + ctx->position) + 0) ^ *R0;
  *L0 += KEY;
  /*---------------------------------------------------*/
  
    t = *L0;
  *L0 = *R0;
  *R0 = t;
  
  /*---------------------------------------------------*/
  KEY  = *((uint32_t *)(ctx->table + ctx->position) + 1) ^ *R1;
  *L1 += KEY;
  /*---------------------------------------------------*/
  
    t = *L1;
  *L1 = *R1;
  *R1 = t;
  
  pos_en(ctx);
}

/* Substitution-Permutation network */
void sp_de(ZWES_CTX * ctx, uint8_t * temp) {
  uint32_t t;
    
  uint32_t * L0 = (uint32_t *)temp + 0; // 1
  uint32_t * R0 = (uint32_t *)temp + 1; // 2
  
  uint32_t * L1 = (uint32_t *)temp + 2; // 3
  uint32_t * R1 = (uint32_t *)temp + 3; // 4
  
  uint32_t KEY;
  
  /*---------------------------------------------------*/
  KEY  = *((uint32_t *)(ctx->table + ctx->position) + 0) ^ *L0;
  *R0 -= KEY;
  /*---------------------------------------------------*/
  
    t = *L0;
  *L0 = *R0;
  *R0 = t;
  
  /*---------------------------------------------------*/
  KEY  = *((uint32_t *)(ctx->table + ctx->position) + 1) ^ *L1;
  *R1 -= KEY;
  /*---------------------------------------------------*/
  
    t = *L1;
  *L1 = *R1;
  *R1 = t;
  
  pos_de(ctx);
}

/* 1   2  3  4     5  6  7  8 
   5   6  7  8     1  2  3  4 -^
   9  10 11 12    13 14 15 16
   13 14 15 16 ->  9 10 11 12 -^ */
void tornado(uint32_t * temp) {
  uint32_t t = *(temp + 0);
  *(temp + 0) = *(temp + 1);
  *(temp + 1) = t;
  
  t = *(temp + 3);
  *(temp + 3) = *(temp + 2);
  *(temp + 2) = t;
}

/* 1   2  3  4     1  3  2  4
   5   6  7  8     9  6  7 12
   9  10 11 12     5 10 11  8
   13 14 15 16 -> 13 15 14 16 */
void loss(uint8_t * temp) {
  uint8_t t = temp[1];
  temp[1] = temp[2];
  temp[2] = t;
  
  t = temp[13];
  temp[13] = temp[14];
  temp[14] = t;
  
  t = temp[4];
  temp[4] = temp[8];
  temp[8] = t;
  
  t = temp[7];
  temp[7] = temp[11];
  temp[11] = t;
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

/*  1  2  3  4     2  3  4  1 <- 1
    5  6  7  8     8  5  6  7 -> 1
    9 10 11 12    10 11 12  9 <- 1
   13 14 15 16 -> 16 13 14 15 -> 1 */
void table_right(uint8_t * temp) {
    uint8_t t = temp[0];
    temp[0] = temp[1];
    temp[1] = temp[2];
    temp[2] = temp[3];
    temp[3] = t;
  
    t = temp[7];
    temp[7] = temp[6];
    temp[6] = temp[5];
    temp[5] = temp[4];
    temp[4] = t;
  
    t = temp[8];
    temp[8]  = temp[9];
    temp[9]  = temp[10];
    temp[10] = temp[11];
    temp[11] = t;
  
    t = temp[15];
    temp[15] = temp[14];
    temp[14] = temp[13];
    temp[13] = temp[12];
    temp[12] = t;
}

void box_left(uint8_t * temp) {
  uint8_t t = temp[0];
    temp[0]  = temp[10];
    temp[10] = t;
    
    t = temp[1];
    temp[1]  = temp[4];
    temp[4]  = temp[13];
    temp[13] = temp[11];
    temp[11] = temp[14];
    temp[14] = temp[7];
    temp[7]  = t;

    t = temp[2];
    temp[2] = temp[8];
    temp[8] = t;
    
    t = temp[3];
    temp[3]  = temp[12];
    temp[12] = temp[15];
    temp[15] = t;
    
    t = temp[5];
    temp[5] = temp[9];
    temp[9] = temp[6];
    temp[6] = t;
}

void box_right(uint8_t * temp) {
    uint8_t t = temp[1];
    temp[1]  = temp[7];
    temp[7]  = temp[14];
    temp[14] = temp[11];
    temp[11] = temp[13];
    temp[13] = temp[4];
    temp[4]  = t;
    
    t = temp[2];
    temp[2] = temp[8];
    temp[8] = t;
    
    t = temp[3];
    temp[3]  = temp[15];
    temp[15] = temp[12];
    temp[12] = t;
    
    t = temp[5];
    temp[5] = temp[6];
    temp[6] = temp[9];
    temp[9] = t;
    
    t = temp[0];
    temp[0]  = temp[10];
    temp[10] = t;
}

void table_left(uint8_t * temp) {
    uint8_t t = temp[3];
    temp[3] = temp[2];
    temp[2] = temp[1];
    temp[1] = temp[0];
    temp[0] = t;
  
    t = temp[4];
    temp[4] = temp[5];
    temp[5] = temp[6];
    temp[6] = temp[7];
    temp[7] = t;
  
    t = temp[11];
    temp[11] = temp[10];
    temp[10] = temp[9];
    temp[9] = temp[8];
    temp[8] = t;
  
    t = temp[12];
    temp[12] = temp[13];
    temp[13] = temp[14];
    temp[14] = temp[15];
    temp[15] = t;
}

void add(uint8_t * temp) {
  for (int i = 0; i < BLOCK_SIZE; ++i) {
    temp[i] += zbox[i] ^ (i + 1);
  }
}

void sub(uint8_t * temp) {
  for (int i = 0; i < BLOCK_SIZE; ++i)
    temp[i] -= zbox[i] ^ (i + 1);
}

void anemone_encrypt(ZWES_CTX * ctx, uint8_t * in, uint8_t * out) {
  uint8_t temp[BLOCK_SIZE];

  memcpy(temp, in, BLOCK_SIZE);
  
  for (int i = 0; i < Rounds; ++i) {
    add(temp);
    sp_en(ctx, temp);

    //loss(temp);
    sonne(temp);
    
    tornado((uint32_t*)temp);
    
    box_left(temp);
    table_left(temp);  
    
    reverse(temp);
    reverse(temp + 8);
  }
  
  memcpy(out, temp, BLOCK_SIZE);
}

void anemone_decrypt(ZWES_CTX * ctx, uint8_t * in, uint8_t * out) {
  uint8_t temp[BLOCK_SIZE];
  
  memcpy(temp, in, BLOCK_SIZE);
  
  for (int i = 0; i < Rounds; ++i) {     
    reverse(temp + 8);
    reverse(temp);
    
    table_right(temp);
    box_right(temp);
    
    tornado((uint32_t*)temp); 
    
    sonne(temp);
    //loss(temp);
    
    sp_de(ctx, temp);
    sub(temp);
  }
  
  memcpy(out, temp, BLOCK_SIZE);
}
