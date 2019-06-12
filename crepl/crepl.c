#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<errno.h>
#include<dlfcn.h>
#include<unistd.h>
int g_cnt = 0;
char so_name[100][10];
void *handler[100];

void gen_file(char *s_in){
  char suffix[]="tmp-XXXXXX", filename[10], tmp[50];
  FILE *fp=NULL;
  char *tmpname = mktemp(suffix);
  //fprintf(stderr, "errno: %s\n", strerror(errno));
  assert(tmpname!=NULL);
  sprintf(filename,"%s.c",tmpname);
  sprintf(so_name[g_cnt],"%s.so",tmpname);
  printf("%s\n",filename);
  fp = fopen(filename,"a+");
  assert(fp!=NULL);
  int i = 0;
  while(s_in[i]!='\0'){
    fputc(s_in[i++],fp);
  }
  sprintf(tmp,"gcc -shared -fPIC -nostartfiles -o %s %s",so_name[g_cnt],filename);
  system(tmp);
  handler[g_cnt] = dlopen(so_name[g_cnt],RTLD_LAZY|RTLD_GLOBAL);
  assert(handler[g_cnt]!=NULL);
  unlink(filename);
  fclose(fp);
  g_cnt++;
}

int main(int argc, char *argv[]) {
  char s_in[1000], tmp[1000], tmpc;
  while(scanf("%[^\n]%c",s_in,&tmpc)!=EOF){
    sscanf(s_in,"%s",tmp);
    if(strcmp(tmp,"int")==0){
      gen_file(s_in);
      continue;
    }
    sprintf("int _exprXXX(){return ",tmp);
    strcat(tmp,s_in);
    strcat(tmp,";}");
    gen_file(tmp);
    int (*func)() = dlsym(handler[g_cnt-1],"_exprXXX");
    assert(func!=NULL);
    int value = func();
    printf(">> %d\n",value);
    dlclose(handler[g_cnt-1]);
    unlink(so_name[--g_cnt]);
  }
  for(int i=0; i<g_cnt; i++){
    dlclose(handler[i]);
    unlink(so_name[i]);
  }
  return 0;
}
