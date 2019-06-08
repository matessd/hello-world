#include "kvdb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int kvdb_open(kvdb_t *db, const char *filename){
  FILE *fp = NULL;
  fp = fopen(filename, a+);
  if(fp==NULL) return 1;
  db->fp = fp;
  db->ifopen = 1;
  return 0;
}

int kvdb_close(kvdb_t *db){
  db->ifopen = 0;
  int ret = fclose(db->fp);
  db->fp = NULL;
  return ret;
}

int kvdb_put(kvdb_t *db, const char *key, const char *value){
  //db->fix = 0;//开始修改 
  if(db->ifoen==0) return 1;
  if(fseek(db->fp,0,SEEK_END)!=0) return 2;
  int ret = fprintf(db->fp,"%s %s\n",key,value);
  //db->fix = 1;
  return ret<0? 3:0;
}

char *kvdb_get(kvdb_t *db, const char *key){
  if(db->ifopen==0) return NULL;
  if(fseek(db->fp,0,SEEK_SET)!=0) return NULL;
  char tkey[130];
  char *value = malloc(16*1024*1024)//16MB
  if(value==NULL) return NULL;
  tkey[0] = '\0';
  while(strcmp(tkey,key)!=0){
    if(fscanf("%s %s",tkey,value)==EOF)
      return NULL;
  }
  return value;
}

