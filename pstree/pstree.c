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
#define MAX_LINE_LEN 1024
char aa_out[MAX_PID][MAX_LINE_LEN];

void fnRead_proc(FILE* fp);
void fnMake_tree();

#define true 1
#define false 0

void fnRead_data(){
  //below is read dir
  DIR *dir = opendir("/proc");
  assert(dir);
  struct dirent *ptr;
  char file_name[512];
  while((ptr = readdir(dir))){
      if(ptr->d_name[0]>'0' && ptr->d_name[0]<='9'){
          //只读取以数字命名的目录
          //printf("%s\n",ptr->d_name);
          sprintf(file_name,"/proc/%s/status",ptr->d_name);
          FILE* fp = fopen(file_name,"r");
          fnRead_proc(fp);
          
          //read task dir
          char sub_file_name[512];
          char dir_name[255];
          struct dirent *sub_ptr;
          sprintf(dir_name,"/proc/%s/task",ptr->d_name);
          DIR* sub_dir = opendir(dir_name);
          for(int i=0; i<3; i++){
              //跳过task中的前三个文件夹. .. 和自身线程
              sub_ptr = readdir(sub_dir);
          }
          while( (sub_ptr = readdir(sub_dir)) ){
              sprintf(sub_file_name,"%s/%s/status",dir_name,sub_ptr->d_name);
              //printf("%s\n",sub_file_name);
              fp = fopen(sub_file_name,"r");
              fnRead_proc(fp);
          } 
      }
  }
}

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
  fnRead_data();
  fnMake_tree(); 
  return 0;
}

#define NAME_LINE 1//名称行
#define PID_LINE 6//pid行
#define PPID_LINE 7//ppid行
#define BUFF_LEN 1024//读取一行缓冲区的最大长度
#define TPID_LINE 4//tpid行
void fnRead_line(FILE* fp, char* line_buff, int ln){
    //ln 代表要读的行号
    fseek(fp,0,SEEK_SET);//定位文件头
    for(int i=1; i<=ln; i++){
        fgets(line_buff,BUFF_LEN,fp);
    }
}

void fnRead_proc(FILE* fp){
    char tmp[32];
    char line_buff[BUFF_LEN];//暂存行
    
    //read name
    fnRead_line(fp, line_buff, NAME_LINE);
    char name[256];
    sscanf(line_buff,"%s %s",tmp,name);
    
    //read pid
    fnRead_line(fp, line_buff, PID_LINE);
    int pid;
    sscanf(line_buff,"%s %d",tmp,&pid);
    
    //read ppid
    fnRead_line(fp, line_buff, PPID_LINE);
    int ppid;
    sscanf(line_buff,"%s %d",tmp,&ppid);

    //read tpid
    fnRead_line(fp, line_buff, TPID_LINE);
    int tpid;
    sscanf(line_buff,"%s %d",tmp,&tpid);
    
    //线程的父亲认为是tpid
    ppid = (pid==tpid)?ppid:tpid;

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

int cmp(const void* a, const void* b){
    stProcess* aa = (stProcess*)a;
    stProcess* bb = (stProcess*)b;
    if(a_op[1]==true){
        //-n模式，按pid大小排序
        if(aa->pid > bb->pid)
            return 1;
        else return 0;
    }
    //非-n，按字母序排序
    if(strcmp(aa->name,bb->name)>0)
        return 1;
    else return 0;
}

int fnDFS(int pid,  char* name, int x, int y){
    a_vis[pid] = true;
    //printf("%d\n",pid);
    int loop_flag = true;
    char name_num[512];
    if(a_op[0]==true){
        sprintf(name_num,"%s(%d)",name,pid);
    }else{
        sprintf(name_num,"%s",name);
    }
    strcpy(&aa_out[x][y], name_num);
    //先把进程名拷进来
    y += strlen(name_num);
    //printf("%d&\n",y);
    int width = 0;
    int x0 = x;
    while(loop_flag){
        for(int i=1; i<=a_pid_num; i++){
            int child_pid = a_process[i].pid;
            if(a_process[i].ppid==pid && a_vis[child_pid]==false){
                if(width==0){
                    strcpy(&aa_out[x][y],"─┬─");
                    y+=3;
                }else{
                    for(int j=1; j<width; j++)
                        strcpy(&aa_out[x+j][y],"│");
                    x+=width;
                    strcpy(&aa_out[x][y],"├─");
                }
                /*for(int j=1; j<=width; j++){
                    aa_out[x+j][y] = '|';
                }
                printf("%d*\n",y);*/
                width = fnDFS(child_pid, a_process[i].name, x, y+6);
                break;
            }
            if(i==a_pid_num)
                loop_flag = false;
        }
    }
    x += width;
    int ret = (width==0)?1:x-x0;
    return ret;
}

void fnMake_tree(){
    //a_pid_num不可能连2个都没有
    qsort(a_process+2,a_pid_num-1,sizeof(a_process[0]),cmp);
    memset(aa_out,' ',sizeof(aa_out));
    int line_cnt = fnDFS(1,a_process[1].name, 0, 0);
    for(int i=0; i<line_cnt; i++){
        puts(&aa_out[1][0]);
    }   
}


