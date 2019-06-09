#include "kvdb.h"
//#define BLOCK_SZ 256

int kvdb_open(kvdb_t *db, const char *filename){
  FILE *fp = NULL;
  fp = fopen(filename, "a+");
  if(fp==NULL) return 1;
  int fd = fileno(fp);
  if(fd==-1) return -1;
  db->fd = fd;
  db->fp = fp;
  db->ifopen = 1;
  //log恢复完
  //fseek(fp,0,SEEK_SET);
  //if(fprintf(fp,"0 0\n")<0) return -2;
  return 0;
}

int kvdb_close(kvdb_t *db){
  db->ifopen = 0;
  int ret = fclose(db->fp);
  db->fp = NULL;
  db->fd = -1;
  return ret;
}

void journal_write(FILE *fp, long off, const char *key, const char *value){
}

int kvdb_put(kvdb_t *db, const char *key, const char *value){
  if(db->ifopen==0) return 1;
  char tkey[130]; tkey[0] = '\0';
  int used=0, cnt=0, ok=0;
  fseek(db->fp,0,SEEK_SET);
  while(1){
    fscanf(db->fp,"%d %s %d%[^\n]",&cnt,tkey,&used);
    //printf("*%d*%c*\n",(int)tmpc,tmpc);
    if(strcmp(tkey,key)==0&&used==1) {
      //printf("1\n");
      ok = 1; break;
    }
    if(feof(db->fp)) break;
    fseek(db->fp,cnt+1,SEEK_CUR);
  }
  int len = strlen(value);
  
  int ret = 0;
  if(ok==1){
    if(len<=cnt){
      //printf("1\n");
      ret = fprintf(db->fp,"%s",value);
    }
    else {
      if(fseek(db->fp,-2,SEEK_CUR)!=0)
        return 2;
      fprintf(db->fp,"0");
      ok = 0;
    }
  }
  if(ok==0){
    //printf("1\n");
    fseek(db->fp,0,SEEK_END);
    ret = fprintf(db->fp,"%d %s 1\n%s",len,key,value);
  }
  if(fsync(db->fd)==-1) return -1;
  return ret<0? 3:0;
}

char *kvdb_get(kvdb_t *db, const char *key){
  if(db->ifopen==0) return NULL;
  if(fseek(db->fp,0,SEEK_SET)!=0) return NULL;
  char tkey[130]; tkey[0]='\0';

  char *value = malloc(16*1024*1024);//16MB
  if(value==NULL) return NULL;
  int cnt, used;
  while(strcmp(tkey,key)!=0||used!=1){
    if(fscanf(db->fp,"%d %s %d %s",&cnt,tkey,&used,value)==EOF)
      return NULL;
  }
  return value;
}
