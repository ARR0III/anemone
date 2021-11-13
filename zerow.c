#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
  zerow.c 
  make file random name and write for disk zerow data!

*/

#define FLEN         32
#define BLOCK_SIZE 4096

typedef struct {
  char newname[FLEN];
  char filepath[BLOCK_SIZE / 2]; /* 2048 byte*/

  unsigned char buffer[BLOCK_SIZE];
} DATA;

const short int    KB = 1024;
const int          MB = 1048576;
const unsigned int GB = 1073741824;

int randomrange(int min, int max) {
 return min + rand() % ((max + 1) - min);
}

void prints(long long int number) {
  if (number >= GB)
    printf("%4.1f Gb.\n", (float)number / (float)GB);
  else
  if ((number >= MB) && (number < GB))
    printf("%4.1f Mb.\n", (float)number / (float)MB);
  else
  if ((number >= KB) && (number < MB))
    printf("%4.1f Kb.\n", (float)number / (float)KB);
  else
  if ((number >= 0) && (number < KB))
    printf("%4d Bt.\n", (int)number);
}

int main (int argc, char * argv[]) {

  if (argc != 2) {
    printf("[!] Enter: %s [path of file]\n", argv[0]);
    return 0;
  }

  int  path_len, filename_len;
  int  struct_len = sizeof(DATA);
  
  long long int filesize = 0; /* 64-bits counter! */

  DATA * data = (DATA *)calloc(1, struct_len);

  if (data == NULL) {
    printf("[!] Cannot allocate memory!\n");
    return -1;
  }

  path_len = filename_len = 0;

  path_len = strlen(argv[1]);

  if ((path_len < 0) || (path_len > ((BLOCK_SIZE / 2) - FLEN - 1))) {
    printf("[!] Length argument 0 or more 2 KiB!\n");
    free(data);
    return 0;
  }

  memcpy(data->filepath, argv[1], path_len);

  srand(time(NULL));
  filename_len = randomrange(8, FLEN - 1);

  for (int i = 0; i < filename_len; i++) {
    data->newname[i] = (char)randomrange(65, 90);
  }

  memcpy(data->filepath + path_len, data->newname, filename_len);

  FILE * f = fopen(data->filepath, "w+b");

  if (f == NULL) {
    printf("[!] Error: file \"%s\" not created!\n", data->filepath);
    memset(data, 0x00, struct_len);
    free(data);
    return -1;
  }

  while(fwrite(data->buffer, 1, BLOCK_SIZE, f) == BLOCK_SIZE) {
    filesize += BLOCK_SIZE;
  }

  fclose(f);

  memset(data, 0x00, struct_len);
  free(data);
  data = NULL;

  if (remove(data->filepath) != 0) {
    printf("[!] File \"%s\" not deleted!\n", data->filepath); 
  }
  else {
    printf("[#] Kill of data:");
    prints(filesize);
  }

  return 0;
}
