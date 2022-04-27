#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef int32_t DATA;

void quicksort(DATA * data, DATA min, DATA max) {
  if (min < max) {
    DATA left = min, right = max, middle = data[(left + right) / 2];
      do {
        while (data[left] < middle) {
          left++;
        }
        
        while (data[right] > middle) {
          right--;
        }
        
        if (left <= right) {
          DATA tmp = data[left];
          data[left] = data[right];
          data[right] = tmp;
          left++;
          right--;
        }
      } while (left <= right);

    quicksort(data, min, right);
    quicksort(data, left, max);
  }
}

int charcontrol(int c) {
  if (c < 33 || c > 126) {
    return 0;
  }
  else {
    return c;
  }
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
  
  float div = (float)fsize / 100.0;
  
  DATA data[256] = {0};
  DATA sort[256] = {0};
  
  float percent;
  int realread;
  uint8_t buffer[1024] = {0x00};
  
  while ((realread = fread(buffer, 1, 1024, f)) == 1024) {
    for (int i = 0; i < realread; i++) {
      data[buffer[i]] += 1;
    }
  }

  memcpy(sort, data, 256 * sizeof(DATA));
  quicksort(sort, 0, 255);

  fclose(f);

  printf("------------------------------------------\n");
  printf("| HEX | DEC | CHR |  FREQUENCY | PERCENT |\n");
  printf("------------------------------------------\n");

  for (int i = 255, j; (i >= 0) && (j >= 0) && (sort[i] > 0); i--) {

    j = 255;

    while ((j >= 0) && (sort[i] != data[j])) {
      j--;
    }

    if ((j < 0) || (data[j] == -1)) {
      continue;
    }

    percent = (float)sort[i] / div;
    printf("|  %2.2X | %3d |   %c | %10d | %5.2f%% |\n",
          (uint8_t)j, j, (charcontrol(j) ? (uint8_t)j : ' '), sort[i], percent);
    
    fflush(stdout);

    sort[i] = data[j] = -1;
  }

  printf("------------------------------------------\n");

  return 0;
}

