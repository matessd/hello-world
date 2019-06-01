#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<sys/mman.h>
#include<stdint.h>

#define MB (1024*1024)
//扇区sector大小为512字节
#define SECSZ 512
#define FATNUM 2
unsigned char *start=NULL, *tmp_start;
int32_t fd, FILE_SZ/*文件大小*/, FAT_SEC/*FAT扇区数*/, RES_SEC/*保留扇区数*/, SEC_PER_CLU/*每簇扇区数，簇：cluster*/, ST_CLU/*起始簇号*/, data_off/*数据区偏移*/, RES/*数据区大小*/;

int file_size2(char* filename){  
    struct stat statbuf;  
    stat(filename,&statbuf);  
    int size=statbuf.st_size;    
    return size;  
}

void init(char *filename){
  FILE_SZ = file_size2(filename);
  fd = open(filename, O_RDWR);
  assert(fd!=-1);
  //64*MB = 0x4,000,000
  start = mmap(NULL, FILE_SZ, PROT_READ, MAP_PRIVATE, fd, 0);
  assert((intptr_t)start!=-1);
  tmp_start = start;

  FAT_SEC = *(int16_t*)(start+0x16);
  if(FAT_SEC==0) FAT_SEC = *(int32_t*)(start+0x24);//0x1f8
  RES_SEC = *(int16_t*)(start+0xe);//0x20
  SEC_PER_CLU = *(int8_t*)(start+0xd);//0x1
  ST_CLU = *(int32_t*)(start+0x2c);//0x2
  data_off = (RES_SEC + FAT_SEC*FATNUM + (ST_CLU-2)*SEC_PER_CLU)*SECSZ;//0x82000
  //printf("%x\n",64*MB);
  start = start+data_off;
  RES = FILE_SZ - data_off;
}

int main(int argc, char *argv[]) { 
  init(argv[1]);
  //printf("%x**%x\n",(int)(intptr_t)start,(int)(intptr_t)tmp_start);
  //printf("%x\n",FILE_SZ);
  unsigned char *cur = NULL;
  //unsigned char s = *start;
  for(int i=0; i<RES; i++){
    cur = start+i;
    //s = *cur;
    //if(i==1031936) printf("1\n");
    if(cur[0xc]==0 &&cur[0xb]==0x20) printf("%x*%x*%x\n",cur[0x8],cur[0x9],cur[0xa]);
    //if(cur[0xc]==0 &&cur[0xb]==0x20) printf("%s\n",cur);
  }
  munmap(tmp_start, FILE_SZ);
  close(fd);
  return 0;
}
