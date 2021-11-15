#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "src/anemone.c"

int main (void) {
  size_t ctx_len = sizeof(ANEMONE_CTX);
  ANEMONE_CTX * ctx = (ANEMONE_CTX *)calloc(1, ctx_len);
  
  if (NULL == ctx) {
    return -1;
  }
  
  uint8_t password[]      = "/x00/x00/x00/x00/x00/x00/x00/x00"
                            "/x00/x00/x00/x00/x00/x00/x00/x00";
  uint8_t out[BLOCK_SIZE] = {0x00};
  uint8_t in [BLOCK_SIZE] = {0x00};

  anemone_init(ctx, password, 16, 0x00);
  
  printf("KEY:\n");
  printhex(HEX_TABLE, ctx->table, 256);
  
  printf("PT:");
  printhex(HEX_STRING, in, BLOCK_SIZE);
  
  anemone_encrypt(ctx, in, out);
  printf("CT:");
  printhex(HEX_STRING, out, BLOCK_SIZE);
  
  
  //in[0]++;
  password[0]++;
  
  anemone_init(ctx, password, 16, 0x00);

  printf("KEY:\n");
  printhex(HEX_TABLE, ctx->table, 256);

  anemone_encrypt(ctx, in, out);
  
  printf("CT:");
  printhex(HEX_STRING, out, BLOCK_SIZE);
  
  anemone_init(ctx, password, 16, 0xDE);
  anemone_decrypt(ctx, out, in);
  
  printf("\nDE:");
  printhex(HEX_STRING, in, BLOCK_SIZE);
  
  free(ctx);
  return 0;
}

