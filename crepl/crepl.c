#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
int g_cnt = 0;
int main(int argc, char *argv[]) {
  char s_in[1000], tmp[100], tmpc, *filename;
  FILE *fp=NULL;
  while(scanf("%[^\n]%c",s_in,&tmpc)!=EOF){
    sscanf(s_in,"%s",tmp);
    if(strcmp(tmp,"int")==0){
      printf("%s\n",s_in);
      //sprintf(tmp,"%d.c",++g_cnt);
      filename = tmpnam("1");
      fopen("1","r+");
      int i = 0;
      while(s_in[i]!='\0'){
        fputc(s_in[i++],fp);
      }
      fclose(fp);
      system("gcc -c 1");
      printf("%s\n",filename);
      continue;
    }
    //int (*func)() = func_lookup(id);
    //int value = func();
    //printf(">> %s\n",value);
  }
  return 0;
}
