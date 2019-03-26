#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>

int a_op[3];//分别代表-p，-n和-V是否存在

int main(int argc, char *argv[]) {
  //printf("Hello, World!\n");
  int i;		
  for (i = 0; i < argc; i++) { 
    assert(argv[i]); // always true
	if(!strcmp(argv[i],"-p")||!strcmp(argv[i],"--show-pids"))
		a_op[0] = 1;
	if(!strcmp(argv[i],"-n")||!strcmp(argv[i],"--numeric-sort"))
		a_op[1] = 1;
	if(!strcmp(argv[i],"-V")||!strcmp(argv[i],"--version"))
		a_op[2] = 1;
    //printf("argv[%d] = %s\n", i, argv[i]); //teacher's code
  }
  assert(!argv[argc]); // always true
  
  //below is read dir
  DIR *dir = opendir("/proc");
  assert(dir);
  struct dirent *ptr;
  char file_name[512] = {0};
  while((ptr = readdir(dir))){
      if(ptr->d_name[0]>'0' && ptr->d_name[0]<='9'){
          //只读取以数字命名的目录
          sprintf(file_name,"/proc/%s/status",ptr->d_name);
          FILE* fp = fopen(file_name,"r");
          assert(fp);
          char name[512];
          //fscanf(fp,"Name");
          fscanf(fp,"%s",&name[0]);
          printf("%s * %s\n",ptr->d_name,name);
      }
  }
  return 0;
}
