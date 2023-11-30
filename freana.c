#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef int32_t DATA;

void quicksort(DATA * data, DATA min, DATA max) {
  DATA i, j, t, m;

  if (min >= max) return;
  
  i = min;
  j = max;
  m = data[(min + max) >> 1];

  do {
    while (data[i] < m) i++;  
    while (data[j] > m) j--;
        
    if (i <= j) {
      t = data[i];
      data[i] = data[j];
      data[j] = t;

      i++;
      j--;
    }

  } while (i <= j);

  quicksort(data, min, j);
  quicksort(data, i, max);
}

int charcontrol(int c) {
  if (c < 33 || c > 126) {
    return 0;
  }

  return c;
}

int main (int argc, char * argv[]) {

  if (argc != 2) {
    printf("[!] Error: incorrect arguments!\n");
    printf("[!] Enter: %s [filename]\n", argv[0]);
    return 0;
  }
  
  FILE * f = fopen(argv[1], "rb");
 
  if (f == NULL) {
    printf("[!] File \"%s\" not openned!\n", argv[1]);
    return 0;
  }

  fseek(f, 0, SEEK_END);
  long int fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  
  if (fsize <= 0) {
    printf("[!] Size of file \"%s\" not more 0 byte!", argv[1]);
    fclose(f);
    return 0;
  }
  
  double percent;
  double div = (double)fsize / 100.0;
  
  DATA data[256] = {0};
  DATA sort[256] = {0};

  int realread = 0;
  uint32_t byte_counter = 0;  
  uint32_t symbol_counter = 0;

  uint8_t * buffer = (uint8_t*)malloc(1024);

  if (NULL == buffer) {
    return -1;
  }
  
  while (1) {
    realread = (int)fread(buffer, 1, 1024, f);

    for (int i = 0; i < realread; i++) {
      int index = buffer[i];

      if (0 == data[index]) {
        byte_counter++;
      }

      data[index]++;
    }
    
    if (realread < 1024) /* if read from file < 1024 byte then break cycle */
      break;
  }

  free(buffer);
  fclose(f);

  memcpy(sort, data, 256 * sizeof(DATA));
  quicksort(sort, 0, 255);

  printf("------------------------------------------\n");
  printf("| HEX | DEC | CHR |  FREQUENCY | PERCENT |\n");
  printf("------------------------------------------\n");

  for (int i = 255, j = 0; i >= 0 && j >= 0 && sort[i] > 0; i--) {
    j = 255;

    while (j >= 0 && sort[i] != data[j]) {
      j--;
    }

    if (j < 0 || -1 == data[j]) {
      continue;
    }

    if (charcontrol(j)) {
      symbol_counter++;
    }

    percent = (double)sort[i] / div;
    printf("|  %2.2X | %3d |   %c | %10d | %5.2lf%% |\n",
          (uint8_t)j, j, (charcontrol(j) ? (uint8_t)j : ' '), sort[i], percent);
    
    fflush(stdout);

    sort[i] = data[j] = -1;
  }

  printf("------------------------------------------\n");
  printf("| bytes:\t%3d\n", byte_counter);
  printf("| symbols:\t%3d\n", symbol_counter);
  printf("------------------------------------------\n");

  return 0;
}

