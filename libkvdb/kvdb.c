#include "kvdb.h"
//#include <assert.h>
int SEEK1 = 16*1024*1024+512;
int SEEK2 = 4;

int recover(kvdb_t *db){
  fseek(db->fp,SEEK2,SEEK_SET);
  int case_num = 0, off1=0, off2=0, len;
  fscanf(db->fp,"%d",&case_num);
  char key[130];
  char *value = malloc(16*1024*1024);
  if(value==NULL) return -1;
  if(case_num==1){
    fscanf(db->fp,"%d %s",&off1,value);
    fseek(db->fp,off1,SEEK_SET);
    fprintf(db->fp,"%s\n",value);
  }else if(case_num==2){
    fscanf(db->fp,"%d %d %d %s %s\n",&off1,&off2,&len,key,value);
    fseek(db->fp,off1,SEEK_SET);
    fprintf(db->fp,"0");
    fseek(db->fp,off2,SEEK_SET);
    fprintf(db->fp,"%d %s 1 %s\n",len,key,value);
  }else if(case_num==3){
    fscanf(db->fp,"%d %d %s %s",&off1,&len,key,value);
    fseek(db->fp,off1,SEEK_SET);
    fprintf(db->fp,"%d %s 1 %s\n",len,key,value);
  }else{
    return -1;
  }
  free(value);
  fseek(db->fp,0,SEEK_SET);
  fprintf(db->fp,"0 0\n");
  //if(fsync(db->fd)==-1) return -1;
  return 0;
}

int kvdb_open(kvdb_t *db, const char *filename){
  FILE *fp = NULL;
  fp = fopen(filename, "r+");
  if(fp==NULL){
    //no such file
    if(errno==2){
      fp = fopen(filename, "w+");
      if(fp==NULL) return 1;
      fprintf(fp,"0 0\n");
      for(int i=SEEK2; i<SEEK1; i++)
        fputc(' ',fp);
    }
  }
  //assert(fp!=NULL);
  if(fp==NULL) return 1;
  int fd = fileno(fp);
  //assert(fd!=-1);
  if(fd==-1) return -1;

  //log恢复完
  if(pthread_mutex_init(&db->mutex,NULL) return -1;
  pthread_mutex_lock(&db->mutex);
  fseek(fp,0,SEEK_SET);
  int a,b;
  fscanf(fp,"%d %d",&a,&b);
  if(a==1&&b==1) 
    if(recover(db))
      return -1;
  pthread_mutex_unlock(&db->mutex);
  db->fd = fd;
  db->fp = fp;
  db->ifopen = 1;
  return 0;
}

int kvdb_close(kvdb_t *db){
  db->ifopen = 0;
  int ret = fclose(db->fp);
  db->fp = NULL;
  db->fd = -1;
  return ret;
}

int kvdb_put(kvdb_t *db, const char *key, const char *value){
  pthread_mutex_lock(&db->mutex);
  if(db->ifopen==0) return 1;
  char tkey[130], tmpc; 
  tkey[0] = '\0';
  int used=0, cnt=0, ok=0;
  fseek(db->fp,SEEK1,SEEK_SET);
  while(1){
    fscanf(db->fp,"%d %s %d%c",&cnt,tkey,&used,&tmpc);
    //printf("*%d*%s*\n",cnt,tkey);
    if(strcmp(tkey,key)==0&&used==1) {
      ok = 1; break;
    }
    if(feof(db->fp)) break;
    fseek(db->fp,cnt+1,SEEK_CUR);
  }

  int len = strlen(value), off1 = 0, off2 = 0;
  if(ok==1){
    if(len<=cnt){
      off1 = ftell(db->fp);
      //ret = fprintf(db->fp,"%s\n",value);
      fseek(db->fp, SEEK2, SEEK_SET);
      fprintf(db->fp,"1\n%d %s\n",off1,value);
    }else {
      if(fseek(db->fp,-2,SEEK_CUR)!=0)
        return 2;
      off1 = ftell(db->fp);
      fseek(db->fp,0,SEEK_END);
      off2 = ftell(db->fp);
      fseek(db->fp, SEEK2, SEEK_SET);
      fprintf(db->fp,"2\n%d \n%d %d %s %s\n",off1,off2,len,key,value);
      //fpritnf(db->fp,"%d %s 1 %s\n",len,key,value);
    }
  }else{
    fseek(db->fp,0,SEEK_END);
    off1 = ftell(db->fp);
    fseek(db->fp,SEEK2,SEEK_SET);
    //ret = fprintf(db->fp,"%d %s 1 %s\n",len,key,value);
    fprintf(db->fp,"3\n%d %d %s %s\n",off1,len,key,value);
  }
  fseek(db->fp,0,SEEK_SET);
  fprintf(db->fp,"1 1\n");
  if(recover(db)) return -1;
  pthread_mutex_unlock(&db->mutex);
  //if(fsync(db->fd)==-1) return -1;
  return 0;
}

char *kvdb_get(kvdb_t *db, const char *key){
  pthread_mutex_lock(&db->mutex);
  if(db->ifopen==0) return NULL;
  if(fseek(db->fp,SEEK1,SEEK_SET)!=0) return NULL;
  char tkey[130], tmpc; tkey[0]='\0';

  char *value = malloc(16*1024*1024);//16MB
  if(value==NULL) return NULL;
  int cnt, used;
  while(1){
    if(fscanf(db->fp,"%d %s %d%c",&cnt,tkey,&used,&tmpc)==EOF)
      return NULL;
    if(strcmp(tkey,key)==0&&used==1){
      fscanf(db->fp,"%s",value);
      break;
    }
    fseek(db->fp,cnt+1,SEEK_CUR);
  }
  pthread_mutex_unlock(&db->mutex);
  return value;
}

