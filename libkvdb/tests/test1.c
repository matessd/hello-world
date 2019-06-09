#include "../kvdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
volatile int cnt;

void *test1(void *_db) {
  kvdb_t *db = _db;
  char key[20],v[20];
  int i = 0;
  while(i++<10000){
    sprintf(key,"%d\0",++cnt);
    strcpy(v,key);
    //strcat(v,key);
    //strcat(v,key);
    //printf("%s\n",v);
    assert(kvdb_put(db, key, v)==0);
    char *value = kvdb_get(db,key);
    assert(value!=NULL);
    printf("[key:%s][value:%s]\n",key,value);
    free(value);
  } 
  return NULL;
}

void *test2(void *_db) {
  kvdb_t *db = _db;
  char key[20];
  int i = 0;
  while(i++<400){
    sprintf(key,"%d\0",++cnt);
    char *value = kvdb_get(db,key);
    assert(value!=NULL);
    printf("[key:%s][value:%s]\n",key,value);
    free(value);
  } 
  return NULL;
}
#define THREADS 4

int main(int argc, char *argv[]) {
  kvdb_t *db = malloc(sizeof(kvdb_t));
  assert(db != NULL);

  //printf("1\n");
  assert(kvdb_open(db, "b.db")==0);
  //printf("2\n");
  pthread_t pt[THREADS];
  for(int i = 0; i < THREADS; i++) {
    pthread_create(&pt[i], NULL, test1, db);
  }
  for(int i = 0; i < THREADS; i++) {
    pthread_join(pt[i], NULL);
  }
  assert(kvdb_close(db)==0);
  free(db);
  return 0;
}
