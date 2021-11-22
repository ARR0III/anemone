#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "src/anemone.c"

#define BUFFER_SIZE (1024 * 1024)
#define BLOCK_SIZE            16

typedef struct {
  uint8_t input  [BUFFER_SIZE];
  uint8_t output [BUFFER_SIZE];
} DATA;

int main (int argc, char * argv[]) {

  printf("PlexusTCL block cipher Anemone-1.\n");

  if (5 != argc) {
    printf("command: %s [input] [output] [-e/d] [password]\n", argv[0]);
    return -1;
  }

  size_t ctx_len = sizeof(ANEMONE_CTX);
  ANEMONE_CTX * ctx = (ANEMONE_CTX *)calloc(1, ctx_len);
  
  if (NULL == ctx) {
    return -1;
  }
  
  DATA * data = (DATA *)calloc(1, sizeof(DATA));
  
  if (NULL == data) {
    return -1;
  }
  
  if (strcmp(argv[1], argv[2]) == 0) {
    printf("files names \"%s\" and \"%s\" equal!\n", (char *)argv[1], (char *)argv[2]);
    return -2;
  }
  
  FILE * fi = fopen(argv[1], "rb");
  
  if (NULL == fi) {
    printf("file \"%s\" not openned!\n", (char *)argv[1]);
    return -3;
  }
  
  FILE * fo = fopen(argv[2], "wb");
  
  if (NULL == fo) {
    printf("file \"%s\" not openned!\n", (char *)argv[2]);
    return -4;
  }
  
  int param;
  
  if (strcmp(argv[3], "-e") == 0 || strcmp(argv[3], "--encrypt") == 0) {
    param = 0x00;
  }
  else
  if (strcmp(argv[3], "-d") == 0 || strcmp(argv[3], "--decrypt") == 0) {
    param = 0xDE;
  }
  else { 
    printf("operation incorrect \"%s\"\n", argv[3]);
    return -5;
  }
  
  anemone_init(ctx, (uint8_t *)argv[4], strnlen(argv[4], 256), 0x00);
  
  uint8_t iv[BLOCK_SIZE];

  size_t read;
  
  if (0x00 == param) {
    srand(time(NULL));
    
    for (int i = 0; i < BLOCK_SIZE; i++) {
      *((uint32_t *)iv) ^= read;
      iv[i] ^= (uint8_t)genrand(0x00, 0xFF);
    }
    
    fwrite(iv, 1, BLOCK_SIZE, fo);
    fflush(fo);
    fseek(fo, BLOCK_SIZE, SEEK_SET);
  }
  else {
    fread(iv, 1, BLOCK_SIZE, fi);
    fseek(fi, BLOCK_SIZE, SEEK_SET);
  }
  
  //printhex(HEX_STRING, iv, BLOCK_SIZE);
  
  do {
    read = fread(data->input, 1, BUFFER_SIZE, fi);

    for (size_t i = 0; i < read; i += BLOCK_SIZE) {
      anemone_encrypt(ctx, iv, data->output + i);
      strxor(data->output + i, data->input + i, BLOCK_SIZE);
      memmove(iv, (param ? data->input : data->output) + i, BLOCK_SIZE);
    }
    
    fwrite(data->output, 1, read, fo);
    fflush(fo);

  } while (read == BUFFER_SIZE);
  
  fclose(fi);
  fclose(fo);
  
  free(data);
  free(ctx);
  
  return 0;
}

