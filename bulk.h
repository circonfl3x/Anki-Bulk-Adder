#ifndef _BULK_
#define _BULK_ 1
#include <stdbool.h>


typedef struct{

  char **front;
  char **back;
  unsigned long nums;
  
} vec;


extern vec cards;

extern bool print_json;
extern char *currentDeck;
extern void add_elem(vec *v, const char *front, const char *back);
extern void v_free(vec *v);
extern char ** parser(const char *line);
extern void parse_return(const char *ret);
#endif
