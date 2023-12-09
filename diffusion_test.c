#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 16

#include "src/xtalw.h"
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

    printf("[---TEST_VECTOR:%3d---]\n", 1);

    printf("HEX_KEY_TEXT:\n");
    phex(HEX_STRING, password, 16, stdout);

    anemone_init(ctx, password, 16, ANEMONE_ENCRYPT);

    printf("HEX_PLAIN_TEXT:\n");
    phex(HEX_STRING, in, 16, stdout);

    anemone_encrypt(ctx, in, out);

    arraytobits(out, 16, stdout);

    printf("HEX_CIPHER_TEXT:\n");
    phex(HEX_STRING, out, 16, stdout);
    printf("\n");

    in[15]++;

    printf("[---TEST_VECTOR:%3d---]\n", 2);

    printf("HEX_KEY_TEXT:\n");
    phex(HEX_STRING, password, 16, stdout);

    anemone_init(ctx, password, 16, ANEMONE_ENCRYPT);

    printf("HEX_PLAIN_TEXT:\n");
    phex(HEX_STRING, in, 16, stdout);

    anemone_encrypt(ctx, in, out);

    arraytobits(out, 16, stdout);

    printf("HEX_CIPHER_TEXT:\n");
    phex(HEX_STRING, out, 16, stdout);
    printf("\n");
/*
    in[15]--;
    password[15]++;

    printf("[---TEST_VECTOR:%3d---]\n", 3);

    printf("HEX_KEY_TEXT:\n");
    phex(HEX_STRING, password, 16, stdout);

    anemone_init(ctx, password, 16, ANEMONE_ENCRYPT);

    printf("HEX_PLAIN_TEXT:\n");
    phex(HEX_STRING, in, 16, stdout);

    anemone_encrypt(ctx, in, out);

    printf("HEX_CIPHER_TEXT:\n");
    phex(HEX_STRING, out, 16, stdout);
    printf("\n");
*/
  free(ctx);
  return 0;
}
