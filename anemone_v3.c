/*
 * Author: ARR0III;
 * Modification: 01.12.2023; 00:01; +0700; Krasnoyarsk, Russia;
 *
 * Block cipher name: Anemone;
 * Version: 3;
 * Rounds: 8-32;
 * Block size: 128 bits;
 * Key size: 1-2048 bits;
 * Architecture: Feistel network;
 */

#include <stdio.h>
#include <stdint.h>

#include "anemone.h"

#define Rounds      32 /* This is max rounds! Change to 1 do 32 */
#define BLOCK_SIZE  16

#define ROR(x,n)  (((x) >> ((int)(n))) | ((x) << (32 - (int)(n))))
#define ROL(x,n)  (((x) << ((int)(n))) | ((x) >> (32 - (int)(n))))

static uint32_t zbox[2][4] = { /* sha256sum("Veritas vos liberabit") */
  {0x93B3A436, 0x97353B17, 0x8BCDAA75, 0xFE210D89},
  {0x5C908450, 0x60B9E2BF, 0xCF34D1A8, 0x5DBD8EA0}
};

void anemone_init(ANEMONE_CTX * ctx, uint8_t * key, int key_len, int operation) {
  uint32_t i, j, k = 0;
  uint8_t t;

  ctx->operation = operation;

  if (operation == 0xDE) {
    ctx->position = (8 * Rounds) - (BLOCK_SIZE / 2);
  }
  else {
    ctx->position = 0;
  }

  for (i = 0; i < KEY_LENGTH; ++i) {
    ctx->table[i] = (uint8_t)i;
  }

  for (i = 0; i < KEY_LENGTH; ++i) { /* begin k = 0 */
    k = (key[i % key_len] + ctx->table[i % KEY_LENGTH] + k) & 0xFF;

    t  = ctx->table[i];
    ctx->table[i] = ctx->table[k];
    ctx->table[k] = t;
  }

  for (i = 0; i < KEY_LENGTH; ++i) {
    ctx->table[i] ^= key[i % key_len];
  }

  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 4; ++j) {
      ctx->white[i][j] = zbox[i][j];

      for (k = 0; k < (KEY_LENGTH / 4); k += 4) {
        ctx->white[i][j] ^= *(((uint32_t *)&ctx->table[0]) + j + k);
      }
    }
  }
}

#undef KEY_LENGTH

static uint32_t FX(ANEMONE_CTX * ctx, uint32_t X, int pos) {

  uint32_t a, b, c, d, t;

  uint32_t KEY = *((uint32_t *)(ctx->table + ctx->position) + pos);

  t = X + zbox[X & 0x01][KEY & 0x03];

  a =     (KEY ^ ROR(t, 1));
  b = a + (KEY ^ ROR(t, 2));
  c = b + (KEY ^ ROR(t, 6));
  d = c + (KEY ^ ROR(t, 7));

  t = ctx->table[((uint32_t)(a ^ c) >> 24) ^ ((uint32_t)(b ^ d) & 0xFF)];

  return (a + b + c + d + t);
}

#undef ROR
#undef ROL

void anemone_encrypt(ANEMONE_CTX * ctx, uint8_t * in, uint8_t * out) {
  register int i;

  uint32_t L0 = *((uint32_t *)in + 0); // 1
  uint32_t R0 = *((uint32_t *)in + 1); // 2

  uint32_t L1 = *((uint32_t *)in + 2); // 3
  uint32_t R1 = *((uint32_t *)in + 3); // 4

  L0 ^= ctx->white[0][0];
  R0 ^= ctx->white[0][1];
  L1 ^= ctx->white[0][2];
  R1 ^= ctx->white[0][3];

  for (i = 0; i < Rounds; ++i) {
    uint32_t t;

    R0 += FX(ctx, L0, 0);
    R1 += FX(ctx, L1, 1);

     t = L0;
    L0 = R0;
    R0 = L1;
    L1 = R1;
    R1 = t;

    ctx->position += (BLOCK_SIZE / 2);
  }

  ctx->position = 0;

  L0 ^= ctx->white[1][0];
  R0 ^= ctx->white[1][1];
  L1 ^= ctx->white[1][2];
  R1 ^= ctx->white[1][3];

  *((uint32_t *)out + 0) = L0;
  *((uint32_t *)out + 1) = R0;
  *((uint32_t *)out + 2) = L1;
  *((uint32_t *)out + 3) = R1;
}

void anemone_decrypt(ANEMONE_CTX * ctx, uint8_t * in, uint8_t * out) {
  register int i;

  uint32_t L0 = *((uint32_t *)in + 0); // 1
  uint32_t R0 = *((uint32_t *)in + 1); // 2
  uint32_t L1 = *((uint32_t *)in + 2); // 3
  uint32_t R1 = *((uint32_t *)in + 3); // 4

  L0 ^= ctx->white[1][0];
  R0 ^= ctx->white[1][1];
  L1 ^= ctx->white[1][2];
  R1 ^= ctx->white[1][3];

  for (i = 0; i < Rounds; ++i) {
    uint32_t t = L0;
   
    L0 = R1;
    R1 = L1;
    L1 = R0;
    R0 = t;

    R0 -= FX(ctx, L0, 0);
    R1 -= FX(ctx, L1, 1);

    ctx->position -= (BLOCK_SIZE / 2);
  }

  ctx->position = (8 * Rounds) - (BLOCK_SIZE / 2);

  L0 ^= ctx->white[0][0];
  R0 ^= ctx->white[0][1];
  L1 ^= ctx->white[0][2];
  R1 ^= ctx->white[0][3];

  *((uint32_t *)out + 0) = L0;
  *((uint32_t *)out + 1) = R0;
  *((uint32_t *)out + 2) = L1;
  *((uint32_t *)out + 3) = R1;
}

#undef Rounds
#undef BLOCK_SIZE
