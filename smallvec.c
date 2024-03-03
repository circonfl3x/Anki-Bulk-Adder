#include "main.h"
#include <string.h>
#include <stdlib.h>
void add_elem(vec *v, const char *front, const char *back)
{
   v->nums ++;
   v->front = realloc(v->front, v->nums * sizeof(vec));
   v->front[v->nums - 1] = malloc(strlen(front) + 1);
   strcpy(v->front[v->nums-1], front);

   v->back = realloc(v->back, v->nums * sizeof(vec));
   v->back[v->nums - 1] = malloc(strlen(back) + 1);
   strcpy(v->back[v->nums-1], back);
}
void v_free(vec *v)
{
    for(unsigned long i = 0; i < v->nums; ++i)
        {
            free(v->front[i]);
            free(v->back[i]);
        }
}
