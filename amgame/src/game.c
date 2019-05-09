#include <game.h>
#include <klib.h>
void init_screen();
void new_draw_rect(int x, int y, int w, int h, uint32_t color);
void init_game();
void game_draw(int block_num, uint32_t color);
//void splash(int x,y);
//void read_key();

#define FPS 30
#define HZ 100
int mouse_cnt=0;
//game_init
int block_pos[9][2];

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
  for(int i=0; i<9; i++)
      game_draw(i, 0xffffff);
  while(1);
  /*unsigned long time, frames=0, next_frame=0, next_refresh=0;
  bool redraw = false;
  while(1){
      time = uptime();
      if(time >= next_frame) break;
  }
  frame++;*/
  return 0;
}

/*void read_key() {
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
    for(int x=0; x<=w-w/3; x+=w/3){
        for(int y=0; y<=h-h/3; y+=h/3){
            block_pos[cnt][0]=x;
            block_pos[cnt][1]=y;
            cnt++;
        }
    }
}

void game_draw(int block_num, uint32_t color){
    new_draw_rect(block_pos[block_num][0], block_pos[block_num][1], 80, 88, color);
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
