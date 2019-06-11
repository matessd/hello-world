#include<stdio.h>
#include<string.h>
#include<assert.h>
int g_cnt = 0;
int main(int argc, char *argv[]) {
  char s_in[1000], tmp[100], tmpc;
  FILE *fp=NULL;
  while(scanf("%[^\n]%c",s_in,&tmpc)!=EOF){
    sscanf(s_in,"%s",tmp);
    if(strcmp(tmp,"int")==0){
      //sprintf(tmp,"%d.c",++g_cnt);
      fp = tmpfile();
      int i = 0;
      while(s_in[i]!='\0'){
        fputc(s_in[i++],fp);
      }
      fclose(fp);
      continue;
    }
    int (*func)() = func_lookup(id);
    int value = func();
    printf(">> %s\n",value);
  }
  return 0;
}
