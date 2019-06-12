#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<errno.h>
#include<dlfcn.h>
#include<unistd.h>
int g_cnt = 0;
void *handler[1000];

void gen_file(char *s_in){
  char suffix[]="tmp-XXXXXX",so_name[20], filename[20], tmp[100];
  FILE *fp=NULL;
  char *tmpname = mktemp(suffix);
  //fprintf(stderr, "errno: %s\n", strerror(errno));
  assert(tmpname!=NULL);
  sprintf(tmp,"%s",tmpname);
  sprintf(filename,"%s.c",tmp);
  sprintf(so_name,"%s.so",tmp);
  //printf("%s\n",tmp);
  fp = fopen(filename,"a+");
  assert(fp!=NULL);
  int i = 0;
  while(s_in[i]!='\0'){
    fputc(s_in[i++],fp);
  }
  fclose(fp);
  //把filename也加上会缓冲区溢出？
  //不对，隔断字符串后sprint和strcat也不行
  //但在中间加printf，然后又改回来，
  //它自己就突然好了，什么情况
  sprintf(tmp,"gcc -w -shared -fPIC -nostartfiles -m32 -o %s %s",so_name, filename);
  //printf("%s*\n",tmp);
  system(tmp);
  sprintf(tmp,"./%s",so_name);
  handler[g_cnt] = dlopen(tmp,RTLD_NOW|RTLD_GLOBAL);
  //if(handler[g_cnt]==NULL)
    //fprintf (stderr, "Error:%s\n", dlerror());
  unlink(filename);
  unlink(so_name);
  //assert(handler[g_cnt]!=NULL);
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
    sprintf(tmp,"int _exprXXX(){return ");
    strcat(tmp,s_in);
    strcat(tmp,";}");
    gen_file(tmp);
    //assert(0);
    //handler can be NULL
    int (*func)() = dlsym(handler[g_cnt-1],"_exprXXX");
    if(func==NULL){
      printf(">> Compile Error\n");
      g_cnt--;
      //dlclose(handler[--g_cnt]);
      continue;
    }
    //assert(func!=NULL);
    int value = func();
    printf(">> %d\n",value);
    dlclose(handler[--g_cnt]);
  }
  for(int i=0; i<g_cnt; i++){
    dlclose(handler[i]);
    //unlink(so_name[i]);
  }
  return 0;
}
