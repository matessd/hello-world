#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

int a_op[3];//分别代表-p，-n和-V是否存在
#define MAX_PID 32768
#define MAX_NAME 256
typedef struct{
    int pid;//进程号
    int ppid;//父亲
    char name[MAX_NAME];//进程名
}stProcess;
stProcess a_process[MAX_PID];
int a_grand[MAX_PID];//pid的祖先
int a_pid_num = 0;//不考虑祖先为2的情况下，当前找到几个进程
#define MAX_LINE_LEN 25
char aa_out[MAX_PID][MAX_LINE_LEN];

void fnRead_proc(FILE* fp);
void fnMake_tree();

#define true 1
#define false 0
int main(int argc, char *argv[]) {
  //printf("Hello, World!\n");
  int i;		
  for (i = 0; i < argc; i++) { 
    assert(argv[i]); // always true
	if(!strcmp(argv[i],"-p")||!strcmp(argv[i],"--show-pids"))
		a_op[0] = true;
	if(!strcmp(argv[i],"-n")||!strcmp(argv[i],"--numeric-sort"))
		a_op[1] = true;
	if(!strcmp(argv[i],"-V")||!strcmp(argv[i],"--version"))
		a_op[2] = true;
    //printf("argv[%d] = %s\n", i, argv[i]); //teacher's code
  }
  assert(!argv[argc]); // always true
  if(a_op[2]==true){
      printf("This is my version\n");
      return 0;
  } 

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
          fnRead_proc(fp);
          //fnmake_tree();
       }
  }
  fnMake_tree(); 
  return 0;
}

#define NAME_LINE 1//名称行
#define PID_LINE 6//pid行
#define PPID_LINE 7//ppid行
#define BUFF_LEN 1024//读取一行缓冲区的最大长度
void fnRead_proc(FILE* fp){
    char tmp[32];
    char line_buff[BUFF_LEN];//暂存行
    
    //read name
    fseek(fp,0,SEEK_SET);//定位文件头
    for(int i=1; i<=NAME_LINE; i++){
        fgets(line_buff,BUFF_LEN,fp);
    }
    char name[256];
    sscanf(line_buff,"%s %s",tmp,name);
    //printf("%s\n",name);
    
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
    
    //处理得到的信息
    a_grand[pid] = a_grand[ppid];
    if(pid==1||pid==2){
        a_grand[pid] = pid;
    } 
    if(a_grand[pid]==2){
        return;
    }
    a_process[++a_pid_num].pid = pid;
    a_process[a_pid_num].ppid = ppid;
    strcpy(a_process[a_pid_num].name, name);
    //printf("%d %d %d\n",pid,ppid,a_pid_num);
}

int a_vis[MAX_PID];
int a_pos[MAX_PID][2];

int comp(const void* a, const void* b){
    stProcess* aa = (stProcess*)a;
    stProcess* bb = (stProcess*)b;
    if(strcmp(aa->name,bb->name)<0)
        return 1;
    else return 0;
}

int fnDFS(int pid,  char* name){
    a_vis[pid] = true;
    //printf("%d\n",pid);
    int loop_flag = true;
    int x = a_pos[pid][0];
    int y = a_pos[pid][1];
    strcpy(&aa_out[x][y], name);
    //先把进程名拷进来
    y += strlen(name);
    while(loop_flag){
        int width = 0;
        for(int i=1; i<=a_pid_num; i++){
            int child_pid = a_process[i].pid;
            if(a_process[i].ppid==pid && a_vis[child_pid]==false){
                a_pos[child_pid][0] = x;
                a_pos[child_pid][1] = y+2;
                //puts(a_process[i].name);
                aa_out[x][y] = '-';
                aa_out[x][y+1] = '-';
                width = fnDFS(child_pid, a_process[i].name);
                for(int j=1; j<=width; j++){
                    aa_out[x+j][y] = '|';
                }
                x+=width+1;
                break;
            }
            if(i==a_pid_num)
                loop_flag = false;
        }
    }
    int ret = x-a_pos[pid][0];
    a_pos[pid][0] = x;
    a_pos[pid][1] = y;
    return ret;
}

void fnMake_tree(){
    if(a_op[1]==false){//按字母序排序
        //a_pid_num不可能连2个都没有
        qsort(a_process+2,a_pid_num-1,sizeof(a_process[0]),comp);
    }
    memset(aa_out,' ',sizeof(aa_out));
    fnDFS(1,a_process[1].name);
    printf("%d&&&&&\n",a_pos[1][0]);
    for(int i=0; i<=a_pos[1][0]; i++){
        puts(&aa_out[i][0]);
    }   
}


