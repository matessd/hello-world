#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<errno.h>
#include<dlfcn.h>
#include<unistd.h>
int g_cnt = 0;
char so_name[100][30];
void *handler[100];

void gen_file(char *s_in){
  char suffix[]="tmp-XXXXXX", filename[20], tmp[100];
  FILE *fp=NULL;
  char *tmpname = mktemp(suffix);
  //fprintf(stderr, "errno: %s\n", strerror(errno));
  assert(tmpname!=NULL);
  sprintf(tmp,"%s",tmpname);
  sprintf(filename,"%s.c",tmp);
  sprintf(so_name[g_cnt],"%s.so",tmp);
  //printf("%s\n",tmp);
  fp = fopen(filename,"a+");
  assert(fp!=NULL);
  int i = 0;
  while(s_in[i]!='\0'){
    fputc(s_in[i++],fp);
  }
  fclose(fp);
  //printf("%s\n",filename);
  //把filename也加上会缓冲区溢出？
  sprintf(tmp,"gcc -m32 -shared -fPIC -nostartfiles -o %s ",so_name[g_cnt]);
  //printf("%s^^\n",tmp);
  //strcat(tmp,so_name[g_cnt]);
  strcat(tmp, filename);
  printf("%s*\n",tmp);
  system(tmp);
  sprintf(tmp,"./%s",so_name[g_cnt]);
  //printf("%s&\n",tmp);
  handler[g_cnt] = dlopen(tmp,RTLD_LAZY|RTLD_GLOBAL);
  if(handler[g_cnt]==NULL)
    fprintf (stderr, "Error:%s\n", dlerror());
  //fclose(fp);  
  //printf("filename:%s\n",filename);
  unlink(filename);
  unlink(so_name[g_cnt]);
  assert(handler[g_cnt]!=NULL);
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
