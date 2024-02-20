#include "bulk.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
void parse_return(const char *ret)
{
    char *dup = strdup(ret);
    char *tok = strtok(dup, "\r\n");

    while(tok != NULL)
    {
        if(*tok == '{'){
            //shit JSON parsing but it works
            tok++;
            tok[strlen(tok) - 1] = '\0';
            char *response = strtok(tok, "]");

            char *submitted = strdup(response);
            submitted += strlen("\"result\": [");

            response = strtok(NULL, "]");
            char *error = strdup(response);
            error += strlen(", \"error\": ");
            bool is_err = strcmp(error, "null") == 0 ? false : true;
            //printf("%s:%s\n", submitted, error);

            tok = strtok(submitted, ", ");
            for(long i = 0; tok != NULL && i != cards.nums; ++i)
            {

                if(strcmp(tok, "null") == 0){
                    if(!is_err){
                        printf("Couldn't add card (%s,%s) to deck %s\n", cards.front[i], cards.back[i], currentDeck);
                    }else printf("Couldn't add card (%s,%s) to deck %s. Error: %s\n", cards.front[i], cards.back[i], currentDeck, error);
                }else printf("Successfully added card (%s,%s) to deck %s. Code: %s\n", cards.front[i], cards.back[i], currentDeck, submitted);
                tok = strtok(NULL, ", ");

            }
            puts("");
            break;
        }
        tok = strtok(NULL, "\r\n");
    }
    free(dup);
}
