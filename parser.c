#include "main.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
char **parser(const char *line)
{
  const char *p = line;
  bool is_empty = true;
  for(; *p != '\0'; ++p)
  {
    if(!isspace(*p) && isascii(*p))
    {
      is_empty = false;
      break;
    }
  }
  if(is_empty) return NULL;

  char *dup = strdup(line);
  char *tok = strtok(dup, ",");

  char **txt = malloc(sizeof(char *) * 3);

  int elems = 0;
  for(elems = 0;elems < 3; ++elems)
  {
    txt[elems] = malloc(strlen(tok) + 1);
    strcpy(txt[elems], tok);

    tok = strtok(NULL, ",");
    if(tok == NULL){
        size_t len_elem_last = strlen(txt[elems]);
        if(txt[elems][len_elem_last - 1]  == '\n')
            txt[elems][len_elem_last -1] = '\0'; //step back to remove newline
        break;
    }
  }
  free(dup);
  if(elems < 2)
  {
      if(*currentDeck != '\0'){
        return txt;
      }else{
      puts("No deck name specified");
      exit(1);
      }
  }

  //currentDeck = txt[2];
  currentDeck = realloc(currentDeck, strlen(txt[2]) + 1);
  strcpy(currentDeck, txt[2]);
  
  return txt;
}
