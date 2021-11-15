/*
  Block crypt algoritm Anemone-1;
  SP-cipher;
  
  block =  16 byte;
  key   = 256 byte;
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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
  uint32_t i, j, k = 0;

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
    k = key[i % key_len] + ctx->table[i % KEY_LENGTH] + key_len + zbox[k & 15] + k;
    
    swap((uint8_t *)&ctx->table[i], (uint8_t *)&ctx->table[k & 255]);
  }
  
  for (i = 0; i < 4; i++) {
    ctx->white[i] = 0x00000000;
    
    for (j = 0; j < (KEY_LENGTH / 4); j += 4) {
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
 * При шифровании/расшифровке 1/4-ой открытого текста, функция FX сдвигает
 * 32-битную часть ключа на 24 бита, после получая индекс таблицы размытия
 * от 0 до 15, складывая его с шифротекстом/открытым текстом, давая t.
 * 
 * Далее ключ ксорится с сдивигаемым влево значением t, и объединяется с
 * результатом предыдущей операции.
 * 
 * Далее из 256 байтной таблицы берется некое число, индекс которого получен
 * ксором старшей части числа R1 с младшей частью числа R2
 *   
 *   где R1 = a ^ c;
 *       R2 = b ^ d;
 * 
 * Сумма чисел a, b, c, d и t складывается/вычитается при
 * шифрованни/расшифровке.
 */

uint32_t FX(ANEMONE_CTX * ctx, uint32_t X, int pos) {

  uint32_t a, b, c, d, t;

  uint32_t KEY = *((uint32_t *)(ctx->table + ctx->position) + pos);

  t = X + zbox[(KEY >> 24) & 15]; /* if X = 0 then t = zbox[0..15] */

  a =       KEY ^ (t <<  1);
  b = (a + (KEY ^ (t <<  8)));
  c = (b + (KEY ^ (t << 16)));
  d = (c + (KEY ^ (t << 24)));

  t = ctx->table[((a ^ c) >> 24) ^ ((b ^ d) & 0x000000FF)];

  return (a + b + c + d + t);
}

/* Feistel network */
void sp_en(ANEMONE_CTX * ctx, uint8_t * temp) {
  uint32_t t;
    
  uint32_t * L0 = (uint32_t *)temp + 0; // 1
  uint32_t * R0 = (uint32_t *)temp + 1; // 2
  
  uint32_t * L1 = (uint32_t *)temp + 2; // 3
  uint32_t * R1 = (uint32_t *)temp + 3; // 4
  
  /*---------------------------------------------------*/

  *L0 += FX(ctx, *R0, 0);
  *L1 += FX(ctx, *R1, 1);  

    t = *L0;
  *L0 = *R0;
  *R0 = t;
  
    t = *L1;
  *L1 = *R1;
  *R1 = t;

  /*---------------------------------------------------*/
    
  pos_en(ctx);
}

/* Feistel network */
void sp_de(ANEMONE_CTX * ctx, uint8_t * temp) {
  uint32_t t;
    
  uint32_t * L0 = (uint32_t *)temp + 0; // 1
  uint32_t * R0 = (uint32_t *)temp + 1; // 2
  
  uint32_t * L1 = (uint32_t *)temp + 2; // 3
  uint32_t * R1 = (uint32_t *)temp + 3; // 4

  /*---------------------------------------------------*/

  *R0 -= FX(ctx, *L0, 0);
  *R1 -= FX(ctx, *L1, 1);  

    t = *L0;
  *L0 = *R0;
  *R0 = t;
  
    t = *L1;
  *L1 = *R1;
  *R1 = t;

  /*---------------------------------------------------*/  
  
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
  int i;
  uint32_t * ptemp = (uint32_t *)temp;

  for (i = 0; i < 4; i++) {
    *(ptemp + i) ^= ctx->white[i];
  }
}

void anemone_encrypt(ANEMONE_CTX * ctx, uint8_t * in, uint8_t * out) {
  int i;
  uint8_t temp[BLOCK_SIZE];

  memcpy(temp, in, BLOCK_SIZE);

  whitening(ctx, temp);
  
  for (i = 0; i < Rounds; ++i) {
    sp_en(ctx, temp);
    sonne(temp);
    
    /* printhex(HEX_STRING, temp, BLOCK_SIZE); */
  }

  whitening(ctx, temp);

  memcpy(out, temp, BLOCK_SIZE);
}

void anemone_decrypt(ANEMONE_CTX * ctx, uint8_t * in, uint8_t * out) {
  int i;
  uint8_t temp[BLOCK_SIZE];

  memcpy(temp, in, BLOCK_SIZE);

  whitening(ctx, temp);

  for (i = 0; i < Rounds; ++i) {
    sonne(temp);
    sp_de(ctx, temp);

    /* printhex(HEX_STRING, temp, BLOCK_SIZE); */
  }

  whitening(ctx, temp);
  
  memcpy(out, temp, BLOCK_SIZE);
}

