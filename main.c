#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "main.h"
char *HTTP_builder(const char *txt, ...);
void HTTP_write_buf(const char *buf, int sockd);
char *HTTP_read_buf(int sockd);
char *itoa(int a);
char *ltoa(long a);
char *ifcl(int length);
char *sfcl(const char* length);
char *lfcl(long length);
char *generate_HTTP_content(const char *front, const char *back);
char *json_f_to_string(const char *fpath);
char *new_JSON_buff();
char *currentDeck;
void end_JSON_buff(char *HTTP);
#define fmt_Content_Length(x) _Generic((x), int:ifcl,long:lfcl,unsigned long:lfcl, const char *:sfcl,char*:sfcl)(x);

static char *base;
static char *end;

//main
int main(int argc, char **argv)
{

  char *fpath = NULL;
  if(argc >= 3){
      for(int i = 1 ; i < argc; ++i){
          if(strcmp(argv[i], "-f") == 0){
              if(i+1 < argc){
                  fpath = argv[i+1];
                  i++;
              }
              else puts("-f given but no filepath supplied\n");
          }else if(strcmp(argv[i], "--json") == 0){
              print_json = true;
          }else{
              printf("Uknown argument: '%s'\n",argv[i]);
              return 1;
          }
      }
  }else{
      puts("Too few arguments");
      return 1;
  }
  if(fpath == NULL){
      puts("Filepath not supplied");
      return 1;
  }
  FILE *f = fopen(fpath ,"r");
  if(f == NULL)
  {
    printf("Cannot open file: %s\n", fpath);
    return 1;
  }

  base = malloc(strlen("{\r\n\"action\": \"addNotes\",\r\n\"version\": 6,\r\n\"params\": {\r\n\"notes\": [\r\n") + 1);
  *base = '\0';
  strcpy(base, "{\r\n\"action\": \"addNotes\",\r\n\"version\": 6,\r\n\"params\": {\r\n\"notes\": [\r\n");

  end = malloc(strlen("}\r\n}\r\n]\r\n}\r\n}\r\n") + 2);
  *end = '\0';
  strcpy(end, "\r\n]\r\n}\r\n}\r\n");


  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in socket;
  socket.sin_addr.s_addr = inet_addr("127.0.0.1");
  socket.sin_family = AF_INET;
  socket.sin_port = htons(8765);

  int server = connect(sockfd, (const struct sockaddr *)&socket, sizeof(socket));
  if(server == -1)
  {
    puts("Cannot connect to AnkiConnect!");
    exit(1);
  }

  char buff[1024];

  currentDeck = malloc(1);
  *currentDeck = '\0';
  char *buf= new_JSON_buff();
  cards.back= malloc(1);
  cards.front = malloc(1);
  // cards.nums = 0;

  // strcpy(buf, "POST /deckNames HTTP/1.1");
  while(fgets(buff, 1023,f))
  {
    char **parse = parser(buff);
    if(parse == NULL) continue;
    add_elem(&cards, *parse,parse[1], currentDeck);
    //printf("%s\n", currentDeck);
    char *content = generate_HTTP_content(*parse,parse[1] );
    buf = realloc(buf, strlen(content) + strlen(buf) + 1);
    strcat(buf, content);

  }
  buf[strlen(buf) - 3] = '\r';
  buf[strlen(buf) - 2] = '\n';
  buf[strlen(buf) - 1] = '\0';
  //end_JSON_buff(buf);
  buf = realloc(buf, strlen(buf) + strlen(end) + 4);
  strcat(buf, end);
  strcat(buf, "\r\n");
  //printf("%s\n", buf);
  char *content_length = fmt_Content_Length(strlen(buf));
  char *HTTP = HTTP_builder("POST /deckNames HTTP/1.1",content_length,"\r\n",buf,NULL);
  if (print_json == true) printf("%s\n", HTTP);
  else{
    puts("");
    for(size_t i = 0; i < cards.nums; ++i)
    {
        printf("[%lu] Adding Card (%s,%s) -> %s\n", i+1, cards.front[i], cards.back[i], cards.deck[i]);
    }
    puts("");
  }

  HTTP_write_buf(HTTP, sockfd);
  char *ret = HTTP_read_buf(sockfd);
  if(print_json) printf("%s\n", ret);
  else parse_return(ret);
  v_free(&cards);
  //printf("%s\n", ret);


  // char *mo = json_f_to_string("add.json");
  // char *content = generate_HTTP_content("HELLO","WORLD","Default" );
  // char *cl = fmt_Content_Length(strlen(mo));
  // printf("%s\n", mo);
  // char *HTTP = HTTP_builder("POST /deckNames HTTP/1.1",cl,"\r\n",mo,NULL);
  // HTTP_write_buf(HTTP,sockfd);
  // char *buf = HTTP_read_buf(sockfd);
  // printf("%s\n", buf);
  return 0;
}
char *HTTP_builder(const char *txt, ...)
{
    va_list arg;

    char *HTTP = malloc(1);
    *HTTP = '\0';
    size_t len = 0;
    for(va_start(arg, txt); txt != NULL ; txt = va_arg(arg, const char *))
    {
        HTTP = realloc(HTTP, strlen(txt) + len + 3);
        //strcat(HTTP, strndup(txt, strcspn(txt, "\n")));
        strcat(HTTP, txt);
        strcat(HTTP, "\r\n");
        len += strlen(txt) + 3;
    }
    //HTTP = realloc(HTTP, len + 3);
    //strcat(HTTP, "\r\n");

    return HTTP;
}

void HTTP_write_buf(const char *buf, int sockd)
{
    int total = strlen(buf), written = 0, bytes = 0;

    while(written < total)
    {
        bytes = write(sockd, buf+written, total-written);
        written += bytes;
        printf("\033[34mBytes written: \033[0m%d bytes\n", bytes);
        if(bytes < 0){
            perror("Cannot write to socket");
        }else if(bytes == 0) break;
    }

    printf("\033[36mTotal bytes written: \033[0m%d bytes\n\n", written);

}
char *HTTP_read_buf(int sockd)
{
    int bytes = 0, total = 4096, received = 0;
    char *buff = malloc(total + 1);
    *buff = '\0';
    while(received < total){
        int curr = 0;
        bytes = read(sockd, buff+received, total-received);
        printf("\033[34mBytes received: \033[0m%d bytes\n", bytes);
        if(bytes < 0)
        {
                perror("Cannot read from socket");
                free(buff);
                exit(1);
        }
        if(bytes == 0) break;
        received += bytes;
        if(received == curr) break;
    }
    printf("\033[36mTotal bytes received:\033[0m %d bytes\n\n", received);
    buff[received] = '\0';

    return buff;
}
char *itoa(int a)
{
    int len = ceil(log10(a) + 1);
    char *ret = malloc(len + 1);
    snprintf(ret, len, "%d", a);

    return ret;
}
char *ltoa(long a)
{
    int len = ceil(log10(a) + 1);
    char *ret = malloc(len + 1);
    snprintf(ret, len, "%ld", a);

    return ret;
}


char *ifcl(int length)
{
    char *txt = malloc(strlen("Content-Length: ") + 1);
    strcpy(txt, "Content-Length: ");
    char *len = itoa(length);

    size_t len_txt = strlen(txt);
    txt = realloc(txt, len_txt + strlen(len) + 1);
    strcat(txt, len);
    return txt;
}
char *sfcl(const char* length)
{
    char *txt = malloc(strlen("Content-Length: ") + strlen(length) + 1);
    strcpy(txt, "Content-Length: ");
    strcat(txt, length);
    return txt;
}
char *lfcl(long length)
{
    char *txt = malloc(strlen("Content-Length: ") + 1);
    strcpy(txt, "Content-Length: ");
    char *len = ltoa(length);

    size_t len_txt = strlen(txt);
    txt = realloc(txt, len_txt + strlen(len) + 1);
    strcat(txt, len);
    return txt;
}
char *json_f_to_string(const char *fpath)
{
  FILE *f = fopen(fpath, "r");
  if(f == NULL)
  {
    perror("Cannot open file!");
  }
  char buff[256];
  char *m = malloc(1);
  *m = '\0';
  size_t len = 0;
  while(fgets(buff, 255, f))
  {
    //there will always be wasted butes in the buffer (potentially)
    m = realloc(m, len += strlen(buff) + 4);
    strncat(m, buff, strcspn(buff, "\n"));
    strcat(m, "\r\n");
    // len += strlen(buff) + 4;


  }

  return m;
}

char *generate_HTTP_content(const char *front, const char *back)
{
    // char *var =
    // "\"deckName\": \"Default\",\r\n"
    //             "\"modelName\": \"Basic\",\r\n"
    //             "\"fields\": {\r\n"
    //                 "\"Front\": \"front content12345\",\r\n"
    //                 "\"Back\": \"back content\"\r\n";
    char *deckName = strndup(currentDeck, strcspn(currentDeck, "\n"));
    char *construct = malloc(strlen("{\r\n\"deckName\": \"\",\r\n") + strlen(deckName) + strlen("\"modelName\": \"Reversed\",\r\n") + strlen("\"fields\": {\r\n") + strlen("\"Front\": ") + 1);
    *construct = '\0';
    strcat(construct, "{\r\n");
    //strcat(construct, "\"deckName\": \"Default\",\r\n");
    strcat(construct, "\"deckName\": \"");
    strcat(construct, deckName);
    strcat(construct, "\",\r\n");
    strcat(construct, "\"modelName\": \"Reversed\",\r\n");
    strcat(construct, "\"fields\": {\r\n");
    strcat(construct, "\"Front\": ");

    construct = realloc(construct, strlen(construct) + strlen(front) + 6);
    strcat(construct, "\"");
    strcat(construct, front);
    strcat(construct, "\",");
    strcat(construct, "\r\n");


    construct = realloc(construct, strlen(construct) + strlen("\"Back\":")+ strlen("\r\n")+ strlen(back) + 12);
    strcat(construct, "\"Back\": ");
    strcat(construct, "\"");
    strcat(construct, back);
    strcat(construct, "\"\r\n");
    strcat(construct, "}\r\n},");
    strcat(construct, "\r\n");


    return construct;


}

char *new_JSON_buff()
{


    char *begin = malloc(strlen(base) + 1);
    *begin = '\0';
    strcpy(begin, base);

    return begin;
}


void end_JSON_buff(char *HTTP)
{


    HTTP = realloc(HTTP, strlen(end) + strlen(HTTP) + 8);
    strcat(end, HTTP);
    strcat(end, "\r\n");
}
