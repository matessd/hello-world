#include <klib.h>
#include <stdarg.h>
#include <x86.h>

#define LOCKDEF(name) \
  static volatile intptr_t name##_locked = 0; \
  static int name##_lock_flags[8]; \
  void name##_lock() { \
    name##_lock_flags[_cpu()] = get_efl() & FL_IF; \
    cli(); \
    while (1) { \
      if (0 == _atomic_xchg(&name##_locked, 1)) break; \
      pause(); \
    } \
  } \
  void name##_unlock() { \
    _atomic_xchg(&name##_locked, 0); \
    if (name##_lock_flags[_cpu()]) sti(); \
  }
LOCKDEF(pr)

//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
char p_out[65536];
int printf(const char *fmt, ...) {
  pr_lock();
  p_out[0] = '\0';
  char *s = &p_out[0];
  va_list ap;
  int ret=0;
  va_start(ap, fmt);
  ret = vsprintf(s, fmt, ap);
  va_end(ap);
  for(; *s; s++){
    _putc(*s);
  }
  //_putc('*');
  //_putc(p_out[18]);
  pr_unlock();
  return ret;
}

int p_d(char* out, int src){
  if(src==0){
    *out = '0';
	*++out = '\0';
	return 1;
  }
  int ret = 0;
  while(src){
    int tmp = src%10;
	*(out+(ret++)) = tmp + '0';
    src/=10;	
  }
  *(out+ret)='\0';
  return ret; 
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char* str;
  for(str=out; *fmt; fmt++){
	//char *ts;
	//for(ts=out; *ts; ts++)
		//_putc(*ts);
    if(*fmt!='%'){
      *str++ = *fmt;
	  *str = '\0';
      continue;
    }
    fmt++;//能到这说明前一个是%
	//_putc(*fmt);
    if(*fmt=='s'){
      char *s = va_arg(ap,char*);//获得下一个参数
      int len = strlen(s);
	  //for(int i=0; i<len; i++)
		 // _putc(*(s+i));
	  //*str = '\0';//if no it, 
      //strcat(str, s);
	  strcpy(str, s);
	  //_putc(*str);
      while(len--){
		//_putc(*str);
		//_putc('\n');
        str++;
	  }
	  *str = '\0';
    }else if(*fmt=='d'){
      int deci = va_arg(ap,int);
	  char arr[16];
	  int i = p_d(arr, deci);
      while(--i>=0){
        *str++ = arr[i];
      }
	  *str = '\0';
    }else if(*fmt=='0'){
      if(*++fmt=='2'){
		if(*++fmt=='d'){
		  int deci = va_arg(ap,int);
		  char arr[16];
		  int i = p_d(arr, deci);
		  if(i<2) *str++ = '0';
		  while(--i>=0) *str++ = arr[i];
		  *str = '\0';
		}
	  }
	}
  }
  //_putc('*');
  //_putc('\n');
  *str = '\0';
  size_t t_len = strlen(out);
  return t_len;
}

int sprintf(char *out, const char *fmt, ...) {
  assert(out!=NULL);
  va_list ap;
  int ret=0;
  va_start(ap, fmt);
  ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
