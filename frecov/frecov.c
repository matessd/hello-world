#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<sys/mman.h>
#include <stdint.h>

#define MB (1024*1024)
//扇区sector大小为512字节
#define SECSZ 512
#define FATNUM 2
char *start=NULL, *tmp_start;
int32_t FAT_SEC/*FAT扇区数*/, RES_SEC/*保留扇区数*/, SEC_PER_CLU/*每簇扇区数，簇：cluster*/, ST_CLU/*起始簇号*/, data_off/*数据区偏移*/, RES/*数据区大小*/;

void init(){
  FAT_SEC = *(int16_t*)(start+0x16);
  if(FAT_SEC==0) FAT_SEC = *(int32_t*)(start+0x24);//0x1f8
  RES_SEC = *(int16_t*)(start+0xe);//0x20
  SEC_PER_CLU = *(int8_t*)(start+0xd);//0x1
  ST_CLU = *(int32_t*)(start+0x2c);//0x2
  data_off = (RES_SEC + FAT_SEC*FATNUM + (ST_CLU-2)*SEC_PER_CLU)*SECSZ;//0x82000
  //printf("%x\n",64*MB);
  start = start+data_off;
  RES = 64*MB - data_off;
}

int main(int argc, char *argv[]) {
  int fd = open(argv[1], O_RDWR);
  assert(fd!=-1);
  //64*MB = 0x4,000,000
  start = mmap(NULL, 64*MB, PROT_READ, MAP_PRIVATE, fd, 0);
  assert((intptr_t)start!=-1);
  tmp_start = start;
  
  init();
  //printf("%x**%x\n",(int)(intptr_t)start,(int)(intptr_t)tmp_start);
  //int j = 0x82000>>5;
  //printf("%x\n",(int)(intptr_t)(start-j*32));
  for(int i=0; i<=RES/32; i++){
    unsigned char s = *(unsigned char*)(start+i*32);
    if(((uint8_t)s)==0xe5) printf("1\n");
    if(i==1020325) printf("%d\n",RES/32);
  }
  munmap(tmp_start, 64*MB);
  close(fd);
  return 0;
}
