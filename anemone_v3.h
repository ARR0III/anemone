#ifndef ANEMONE_H
#define ANEMONE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_LENGTH 256

typedef struct {
  int32_t  operation;
  int32_t  position;
  uint32_t white[2][4];
  uint8_t  table[KEY_LENGTH];
} ANEMONE_CTX;

void anemone_init(ANEMONE_CTX * ctx, uint8_t * key, int key_len, int operation);

void anemone_encrypt(ANEMONE_CTX * ctx, uint8_t * in, uint8_t * out);
void anemone_decrypt(ANEMONE_CTX * ctx, uint8_t * in, uint8_t * out);

#ifdef __cplusplus
}
#endif

#endif
