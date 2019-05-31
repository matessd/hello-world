#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<sys/mman.h>

#define MB (1024*1024)
//扇区sector大小为512字节
#define SECSZ 512
//FAT每个表项32字节
#define FATSZ 32
#define FATNUM 2
char *start=NULL;


int main(int argc, char *argv[]) {
  int fd = open(argv[1], O_RDWR);
  assert(fd!=-1);
  start = mmap(NULL, 64*MB, PROT_READ, MAP_PRIVATE, fd, 0);
  assert((intptr_t)start!=-1);
  printf("%x\n",*(short *)(start+19));
  munmap(start, 64*MB);
  close(fd);
  //printf("%x\n",(int)(intptr_t)start);
  return 0;
}
