#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<dlfcn.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
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
  system(tmp);
  sprintf(tmp,"./%s",so_name);
  handler[g_cnt] = dlopen(tmp,RTLD_NOW|RTLD_GLOBAL);
  //if(handler[g_cnt]==NULL)
    //fprintf (stderr, "Error:%s\n", dlerror());
  unlink(filename);
  unlink(so_name);
  g_cnt++;
}

int main(int argc, char *argv[]) {
  char s_in[1000], tmp[1000], tmpc;
  s_in[0] = '\0'; printf(">> ");
  int null_fd = open("/dev/null",O_RDWR);
  assert(null_fd>=0);
  dup2(null_fd,2);
  while(scanf("%[^\n]",s_in)!=EOF){
    scanf("%c",&tmpc);
    if(s_in[0]=='\0'){
      printf(">> ");
      continue;
    }
    sscanf(s_in,"%s",tmp);
    int flg = 0;
    if(strcmp(tmp,"int")==0){
      gen_file(s_in);
      if(handler[g_cnt-1]!=NULL) flg = 1;
      else flg = 3;//error
    }else{
      sprintf(tmp,"int _exprXXX(){return ");
      strcat(tmp,s_in); 
      strcat(tmp,";}");
      gen_file(tmp);
      //handler can be NULL
      int (*func)() = dlsym(handler[g_cnt-1],"_exprXXX");
      if(func==NULL) flg = 3;
      else flg = 2;
    }
    if(flg==1) printf("  Add:%s\n",s_in);
    else{
      dlclose(handler[--g_cnt]);
      if(flg==2){
        int value = func();
        printf("  (%s) = %d\n",s_in,value);
      }else
        printf("  Compile Error\n");
    }
    printf(">> ");
    s_in[0] = '\0';
  }
  for(int i=0; i<g_cnt; i++){
    dlclose(handler[i]);
  }
  return 0;
}
