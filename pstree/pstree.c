#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>

int a_op[3];//分别代表-p，-n和-V是否存在
#define MAX_PID 4096
#define MAX_NAME 256
char a_p[MAX_PID];//父节点的pid
char a_name[MAX_PID][MAX_NAME];//pid对应的进程名

void fnread_proc(FILE* fp);
//void fnmake_tree();

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
          //printf("%s\n",ptr->d_name);
          sprintf(file_name,"/proc/%s/status",ptr->d_name);
          FILE* fp = fopen(file_name,"r");
          if(!fp)
              continue;
          assert(fp);
          fnread_proc(fp);
          //fnmake_tree();
      }
  }
  return 0;
}

#define NAME_LINE 1//名称行
#define PID_LINE 4//pid行
#define PPID_LINE 5//ppid行
#define BUFF_LEN 1024//读取一行缓冲区的最大长度
void fnread_proc(FILE* fp){
    char tmp[16];
    char line_buff[BUFF_LEN];//暂存行
    
    //read name
    fseek(fp,0,SEEK_SET);//定位文件头
    for(int i=1; i<=NAME_LINE; i++){
        fgets(line_buff,BUFF_LEN,fp);
    }
    char name[256];
    sscanf(line_buff,"%s %s",tmp,name);
    
    //read pid
    fseek(fp,0,SEEK_SET);
    for(int i=1; i<=PID_LINE; i++){
        fgets(line_buff,BUFF_LEN,fp);
    }
    int pid;
    sscanf(line_buff,"%s %d",tmp,&pid);
    
    //read ppid
    fseek(fp,0,SEEK_SET);
    for(int i=1; i<=PPID_LINE; i++){
        fgets(line_buff,BUFF_LEN,fp);
    }
    int ppid;
    sscanf(line_buff,"%s %d",tmp,&ppid);

    a_p[pid] = ppid;
    strcpy(&a_name[pid][0], name);
    printf("%d %d\n",pid,ppid);
}


