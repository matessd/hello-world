#include "klib.h"

//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__)||defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  assert(s!=NULL);
  size_t cnt = 0;
  while(*s){
    cnt++;
    //_putc(*s);
    s++;
  }
  return cnt;
}

char *strcpy(char* dst,const char* src) {
  assert(dst!=NULL && src!=NULL);
  char *ret = dst;
  while(*src){
    *dst = *src++;//这样最后的'\0'传了吗？
    dst++;
  }
  *dst = '\0';//似乎是必要的
  return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
  return NULL;
}

char* strcat(char* dst, const char* src) {
  assert(dst!=NULL && src!=NULL);
  char* ret = dst;
  while(*dst){
    dst++;
  }
  while(*src){
    *dst = *src++;
    //_putc(*dst);
    dst++;
  }
  *dst='\0';
  return ret;
}

int strcmp(const char* s1, const char* s2) {
  assert(s1!=NULL && s2!=NULL);
  int ret = 0;
  /*while(!(ret=*(unsigned char*)s1 - *(unsigned char*)s2) && *s2&&*s1){
    s1++;
  //_putc(*s1);
  //_putc('\n');
  s2++;
  }*/
  //int i=0;
  while((ret==0) && (*s1) ){
    //_putc(i+'0');
    ret = *(unsigned char*)s1 - *(unsigned char*)s2;
    s1++;
    s2++;
    //_putc((i++)+'0');
  }
  //_putc('*');
  //_putc('\n');
  if(ret<0){
    return -1;
  }
  if(ret>0) return 1;
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  return 0;
}

void* memset(void* v,int c,size_t n) {
  assert(v!=NULL);
  void* ret = v;
  while(n--){
    *(char*)v = (char) c;
    v++;
  }
  return ret;
}

void* memcpy(void* out, const void* in, size_t n) {
  assert(out!=NULL && in!=NULL);
  void* ret = out;
  char* dst = (char *)out;
  char* src = (char *)in;
  while(n--){
    *dst = *src++;
    dst++;
  }
  //*dst='\0';
  return ret;
}

void* memmove(void* dst, const void* src, size_t n){
  assert(dst!=NULL && src!=NULL);
  char *s_dst = (char*)dst;
  char *s_src = (char*)src;
  if(s_dst>s_src && (s_src+n>s_dst)) { 
    s_dst = s_dst+n-1;
    s_src = s_src+n-1;
    while(n--) {
      *s_dst-- = *s_src--;
    }
  }else {
    while(n--) {
      *s_dst++ = *s_src++;
    }
  }
  return dst;
}

int memcmp(const void* s1, const void* s2, size_t n){
  assert(s1!=NULL && s2!=NULL);
  if(n==0){
    return 0;
  }
  int ret = 0;
  char* dst = (char *)s1;
  char* src = (char *)s2;
  while((n--) && !(ret = *(unsigned char*)dst - *(unsigned char*)src)){
    dst++;
    src++;
  }
  if(ret<0) return -1;
  if(ret>0) return 1;
  return 0;
}

#endif
