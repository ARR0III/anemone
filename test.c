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

#ifndef _C_STRING_H_
#define _C_STRING_H_
  #include <string.h>
#endif

#include "src/anemone.c"

int main (void) {
  size_t ctx_len = sizeof(ZWES_CTX);
  ZWES_CTX * ctx = (ZWES_CTX *)calloc(1, ctx_len);
  
  if (NULL == ctx) {
    return -1;
  }
  
  uint8_t password[]      = "0123456789abcdee";
  uint8_t out[BLOCK_SIZE] = {0x00};
  uint8_t in [BLOCK_SIZE] = {0x00};
  uint8_t iv [BLOCK_SIZE] = "fedcba9876543210";
  
  anemone_init(ctx, password, 16, 0x00);
  
  printf("KEY:\n");
  printhex(HEX_TABLE, (char*)ctx + (sizeof(int32_t) * 2), ctx_len - (sizeof(int32_t) * 2));
  
  printf("PT:");
  printhex(HEX_STRING, in, BLOCK_SIZE);
  
  anemone_encrypt(ctx, in, out);
  printf("CT:");
  printhex(HEX_STRING, out, BLOCK_SIZE);
  
  ++in[0];
  
  anemone_init(ctx, password, 16, 0x00);
  anemone_encrypt(ctx, in, out);
  
  printf("CT:");
  printhex(HEX_STRING, out, BLOCK_SIZE);
  
  anemone_init(ctx, password, 16, 0xDE);
  anemone_decrypt(ctx, out, in);
  
  printf("\nDE:");
  printhex(HEX_STRING, in, BLOCK_SIZE);
  
  /*
  FILE * fi = fopen("temp", "rb");
  FILE * fo = fopen("temp.en", "wr");
  
  size_t read;
  
  while ((read = fread(in, 1, BLOCK_SIZE, fi)) == BLOCK_SIZE) {
    zwes_encrypt(ctx, iv, out);
    
    strxor(out, in, BLOCK_SIZE);
    
    fwrite(out, 1, read, fo);
    fflush(fo);
    
    memcpy(iv, out, BLOCK_SIZE);
  }
  
  fclose(fi);
  fclose(fo);
  
  free(ctx);
  */
  return 0;
}
