#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<sys/mman.h>
#include<stdint.h>
#include<string.h>
//#include<stdlib.h>
//#include <locale.h>

#define MB (1024*1024)
//扇区sector大小为512字节
#define SECSZ 512
#define FATNUM 2
unsigned char *start=NULL, *tmp_start;
int32_t fd, FILE_SZ/*文件大小*/, FAT_SEC/*FAT扇区数*/, RES_SEC/*保留扇区数*/, SEC_PER_CLU/*每簇扇区数，簇：cluster*/, ST_CLU/*起始簇号*/, data_off/*数据区偏移*/, RES/*数据区大小*/;
typedef struct{
  char name[15];
}SDE;
typedef struct{
  unsigned char name[15];
  unsigned char checksum;
  unsigned char idx; 
  int8_t vis, ife5;
}LDE;
#define MAX_NAME 50
typedef struct{
  unsigned char name[MAX_NAME];
  unsigned char checksum;
  int8_t ok, ife5;
}myDIR;
SDE sde[200];
LDE lde[300];
myDIR dir[200];
int scnt, lcnt, dircnt;

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

void find_sde(){
  unsigned char *cur = NULL;  scnt = 0;
  for(int i=0; i<RES/32; i++){
    cur = start+i*32;
    if(cur[0xc]==0 &&cur[0xb]==0x20) {
      if(*cur==0xe5) {
        sde[scnt].name[0] = '-';
        sprintf(sde[scnt].name,"%s",cur+1);
      }else{
        sprintf(sde[scnt].name,"%s",cur);
      }
      for(int i=0; i<8; i++){
        if(sde[scnt].name[i]==' '){
          sde[scnt].name[i] = '\0';
          strcat(sde[scnt].name, "BMP");
          break;
        }
      }
      sde[scnt++].name[11]='\0';
      //printf("%s:%d\n",sde[scnt].name,i);
      //if(i==996629) printf("%c**\n",cur[0]);
    }
  }
  //printf("scnt:%d\n",scnt);
}

int uniread(unsigned char *dst, unsigned char *src, int cnt){
  int ret = 0;
  for(int i=0; i<cnt; i++){
    dst[i] = src[i*2];
    ret++;
    if((src[i*2]==0x0&&src[i*2+1]==0x0) || (src[i*2]==0xff&&src[i*2+1]==0xff)){
      ret--;
      break;
    }
  }
  dst[ret] = '\0';
  return ret;
}

void find_lde(){
  unsigned char *cur = NULL, tmp[MAX_NAME];  
  lcnt = 0;  
  for(int i=0; i<RES/32; i++){
    cur = start+i*32;
    if(cur[0xc]==0 &&cur[0xb]==0xf&&cur[0x1a]==0) {
      //printf("0x%x &&",*cur);
      //printf("   %c*\n",cur[0x1]);
      int cnt = 0;
      cnt += uniread(&tmp[cnt], &cur[1], 5);
      cnt += uniread(&tmp[cnt], &cur[0xe], 6);
      cnt += uniread(&tmp[cnt], &cur[0x1c], 2); 
      strcpy((char*)lde[lcnt].name, (char*)tmp);
      if(*cur==0xe5) lde[lcnt].ife5 = 1;
      else lde[lcnt].ife5 = 0;
      lde[lcnt].checksum = cur[0xd];
      lde[lcnt].idx = *cur;
      lde[lcnt].vis = 0;
      //printf("%s*\n",tmp);
      lcnt++;
    }
  }
  //printf("lcnt:%d*\n",lcnt);  
}

void merge_lde(){
  unsigned char tmp[MAX_NAME]; dircnt=0;
  for(int i=lcnt-1; i>=0; i--){
    if(lde[i].vis==1) continue;
    lde[i].vis = 1;
    tmp[0] = '\0';
    strcat((char*)tmp, (char*)lde[i].name);
    if(tmp[0]=='p'&&tmp[1]=='\0'){
      lde[i].vis = 0;
      continue;
    }
    unsigned char checksum = lde[i].checksum;
    unsigned char idx = lde[i].idx;
    //if(idx==0x41) printf("%s\n",tmp);
    //printf("%s\n",tmp);
    if(idx==0xe5){
      for(int j=lcnt-1; j>=0; j--){
        if(lde[j].vis) continue;
        if((lde[j].idx!=0xe5)
            ||(lde[j].checksum!=checksum))
          continue;
        lde[j].vis=1;
        strcat((char*)tmp, (char*)lde[j].name);
      }
    }
    else if((idx&0x40)==0){
      for(int j=i-1; j>=0; j--){
        if(lde[j].vis) continue;
        if(((lde[j].idx&0x1f)!=(idx&0x1f)+1) 
           || lde[j].checksum!=checksum)
           continue;
        lde[j].vis=1;
        strcat((char*)tmp, (char*)lde[j].name);
        if(lde[j].idx&0x40) break;
      }
    }
    strcpy((char*)dir[dircnt].name, (char*)tmp);
    dir[dircnt].checksum = checksum;
    dir[dircnt].ok = 0;
    dir[dircnt].ife5 = lde[i].ife5;
    dircnt++;
    printf("%s*\n",tmp);
    if(tmp[0]=='p'&&tmp[1]=='\0'){
      dircnt--;
      continue;
    }
    //printf("%s*\n",tmp);
  }
  printf("dircnt:%d\n",dircnt);
}

int main(int argc, char *argv[]) { 
  init(argv[1]);
  //printf("%x**%x\n",(int)(intptr_t)start,(int)(intptr_t)tmp_start);
  find_sde();
  find_lde();
  merge_lde();
  munmap(tmp_start, FILE_SZ);
  close(fd);
  return 0;
}
