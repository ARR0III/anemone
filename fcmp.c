#include <stdio.h>
#include <string.h>

#define SIZE 128

int main (int argc, char * argv[]) {
  FILE * f1 = fopen(argv[1], "rb");
  FILE * f2 = fopen(argv[2], "rb");
  
  char a[SIZE + 1] = {0};
  char b[SIZE + 1] = {0};
  
  long pos = 0;
  long seek = 0;
  long all = 0;
  
  long round = 0;

  while ((pos = fread(a, 1, SIZE, f1)) == SIZE) {
                fread(b, 1, SIZE, f2);

    seek += pos + 1;
    
    round++;
    printf("[ROUND %2d]\n", round);
    printf("%s\n", a);
    printf("%s\n", b);
    
    for (int i = 0; i < SIZE; i++) {
      if (a[i] != b[i]) {
        all++;
        putc('^', stdout);
      }
      else {
        putc(' ', stdout);
      }
    }
    
    if (all == SIZE) {
      printf(" <--- diffusion\n");
    }
    else {
      printf(" - %2ld/%d\n", all, SIZE);
    }

    all = 0;
    
    putc('\n', stdout);
    
    memset(a, 0x00, SIZE);
    memset(b, 0x00, SIZE);
    
    fflush(f1);
    fflush(f2);
    fflush(stdout);
    
    fseek(f1, seek, SEEK_SET);
    fseek(f2, seek, SEEK_SET);
  }
  
  fclose(f1);
  fclose(f2);
}
