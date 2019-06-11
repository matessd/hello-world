#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include <errno.h>
int g_cnt = 0;
int main(int argc, char *argv[]) {
  char s_in[1000], tmp[100], tmpc, *filename;
  //FILE *fp=NULL;
  //int fd;
  while(scanf("%[^\n]%c",s_in,&tmpc)!=EOF){
    sscanf(s_in,"%s",tmp);
    if(strcmp(tmp,"int")==0){
      //sprintf(tmp,"%d.c",++g_cnt);
      char tt[] = "XXXXXX";
      filename = mktemp(tt);
      //printf("%s\n",filename);
      //assert(filename!=NULL);
      //fp = fopen(filename,"r+");
      //fprintf(stderr, "errno: %s\n", strerror(errno));
      //assert(fd!=-1);
      assert(filename!=NULL);
      //filename = fcb[fd].filename;
      printf("%s\n",filename);
      /*assert(0);
      int i = 0;
      while(s_in[i]!='\0'){
        fputc(s_in[i++],fp);
      }
      sprintf(tmp,"gcc -c %s",filename);
      system(tmp);
      fclose(fp);
      printf("%s\n",filename);*/
      continue;
    }
    //int (*func)() = func_lookup(id);
    //int value = func();
    //printf(">> %s\n",value);
  }
  return 0;
}
