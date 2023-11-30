#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 16

#include "src/anemone.h"

int main (void) {

  size_t ctx_len = sizeof(ANEMONE_CTX);
  ANEMONE_CTX * ctx = (ANEMONE_CTX *)calloc(1, ctx_len);

  if (NULL == ctx) {
    return -1;
  }

  uint8_t password[16] = {0x00};

  uint8_t out[16] = {0x00};
  uint8_t in [16] = {0x00};

  in[15]++;

  anemone_init(ctx, password, 16, 0x01);
  anemone_encrypt(ctx, in, out);

  free(ctx);
  return 0;
}
