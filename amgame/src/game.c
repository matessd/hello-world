#include <game.h>
#include <klib.h>
#define FPS 30
#define genHZ 3//每秒生成两次地鼠
#define ctlHZ 100//每秒处理100次输入
//函数
void init_screen();
void new_draw_rect(int x, int y, int w, int h, uint32_t color);
void init_game();
void game_draw();
void gen_mouse();
void game_ctl();
//void splash(int x,y);
//int read_key();

//全局变量
static int scan_code[]={
    _KEY_1, _KEY_2, _KEY_3, _KEY_4, _KEY_5,
    _KEY_6, _KEY_7, _KEY_8, _KEY_9,
};
int mouse_cnt=0;//场内地鼠数量
int kill_cnt=0;//已击中地鼠数量
//game_init
int block_pos[9][2];
int block_state[9];//每格状态
//game_ctl
int game_over=0;//是否退出游戏循环
int game_win=0;//游戏输赢

int main() {
  // Operating system is a C program
  _ioe_init();
  init_screen();
  /* teacher's code:
   * splash();
  while (1) {
    read_key();
  }*/
  //below is my
  init_game();
  game_draw();
  unsigned long time, next_gen=0, next_refresh=0, next_ctl=0;;//next_gen是下一次生成地鼠的时间，next_refresh是下一次刷新屏幕的时间,next_ctl是下一次接受玩家输入并处理的时间
  srand(uptime());
  while(1){
    while(1){
        time = uptime();
        if(time >= next_ctl) break;
    }
    next_ctl += 1000/ctlHZ;
    game_ctl();
    if(time >= next_gen){
        if(kill_cnt+mouse_cnt<20)
            gen_mouse();
        next_gen += 1000/genHZ;
    }
    if(time >= next_refresh){
        next_refresh += 1000/FPS;
        game_draw();
    }
    if(game_over==1) break;
  }
  return 0;
}

/*int read_key() {
  _DEV_INPUT_KBD_t event = { .keycode = _KEY_NONE };
  #define KEYNAME(key) \
    [_KEY_##key] = #key,
  static const char *key_names[] = {
    _KEYS(KEYNAME)
  };
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &event, sizeof(event));
  if (event.keycode != _KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
  return event.keycode;
}*/

int w,h;
void init_screen() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;

  //printf("%d>>%d\n",w/SIDE,w%SIDE);
}

void init_game(){
    int cnt = 0;
    for(int y=0; y<=h-h/3; y+=h/3){
        for(int x=0; x<=w-w/3; x+=w/3){
            block_pos[cnt][0]=x;
            block_pos[cnt][1]=y;
            cnt++;
        }
    }
    for(int i=0; i<9; i++)
        block_state[i] = 0;
}

void game_draw(){
    for(int i=0; i<9; i++){
        if(block_state[i]==0) new_draw_rect(block_pos[i][0], block_pos[i][1], 80, 80, 0xffffff);
        else  new_draw_rect(block_pos[i][0], block_pos[i][1], 80, 80, 0xff0000);
    }
}

void gen_mouse(){
    int i = rand()%9;
    while(1){
        if(block_state[i]==0){
            block_state[i]=1;
            mouse_cnt++;
            break;
        }
        i = rand()%9;
    }
    if(mouse_cnt==9){
        game_over=1;
        printf("Lose! You farm is filled with mice\n");
    }
}

void game_ctl(){
    int wrong_beat = 0;
    int keycode = read_key();
    for(int i=0; i<9; i++){
        if(scan_code[i]==keycode){
            if(block_state[i]==1){
                kill_cnt++;
                mouse_cnt--;
                block_state[i]=0;
            }else{
                wrong_beat = 1;
            }
            break;
        }
    }
    if(wrong_beat==1){
        game_over = 1;
        printf("Lose! You beat the wrong place\n");
    }
    if(kill_cnt==20){
        game_over = 1;
        printf("Win! You have killed all 20 mice\n");
    }
}

void new_draw_rect(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: allocated on stack
  _DEV_VIDEO_FBCTL_t event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &event, sizeof(event));
}

/*void splash() {
  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
      if ((x & 1) ^ (y & 1)) {
        draw_rect(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      }
    }
  }
}*/
