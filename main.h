#ifndef _BULK_H
#define _BULK_H 1
#include <stdlib.h>
#include <stdbool.h>
typedef struct
{
    char **front;
    char **back;
    char **deck;
    unsigned long nums;
} vec;
extern vec cards;
extern char **parser(const char *line);
extern char *currentDeck;
extern void parse_return(const char *ret);
extern void add_elem(vec *v, const char *front, const char *back, const char *deck);
extern void v_free(vec *v);
extern bool print_json;
#endif
