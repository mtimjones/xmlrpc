/*
 *  Lightweight Embedded XML-RPC Server main
 *
 *  mtj@mtjones.com
 *
 */

#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include "xmlparser.h"

const char *notimplemented={"HTTP/1.1 501 Not Implemented\n\n"};
const char *success={"HTTP/1.1 200 OK\n"};
const char *separator={"\015\012\015\012"};

#define MAX_ELEMENT	64


/*---------------------------------------------------------------------------
 * findMsgBody() - Find the message body of an HTTP Request Message
 *-------------------------------------------------------------------------*/
char *findMsgBody( char *buf )
{
  char *temp;

  temp = strstr(buf, separator);

  if (temp == NULL) return NULL;
  else return temp+4;
}


/*---------------------------------------------------------------------------
 * getHeaderElement() - Find the HTTP header and return it's value.
 *-------------------------------------------------------------------------*/
int getHeaderElement( char *buffer, char *header, char *value, int size )
{
  char *temp;
  int  i=0;

  temp = strstr(buffer, header );
  if (temp) {

    /* Skip the header element */
    temp += strlen(header);

    /* Skip any white-space */
    while (*temp == ' ') temp++;

    /* Copy the rest to the value parameter */
    while ((*temp != ' ') && (*temp != '\n') && (i < size)) {
      value[i++] = *temp++;
    }
    value[i] = 0;

    return i;

  }

  return -1;
}


/*---------------------------------------------------------------------------
 * handleConnection() - Parse and handle the current HTTP request message
 *-------------------------------------------------------------------------*/
void handleConnection(int fd)
{
  fd_set rfds;
  struct timeval tv;
  int ret, len, max=0, payloadLength = -1;
  char buffer[4096]={0}, value[MAX_ELEMENT+1];
  char *temp;

  /*
   * Read in the Request Header 
   */
  do {

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    ret = select(fd+1, &rfds, NULL, NULL, &tv);

    if (ret > 0) {

      if (FD_ISSET(fd, &rfds)) {

        len = recv( fd, &buffer[max], 1024, 0);

        if (len > 0) {
          max += len;
          buffer[max] = 0;

          ret = getHeaderElement(buffer, "Content-Length:", value, MAX_ELEMENT);
          if (ret > 0) payloadLength = atoi(value);

        } else {
          ret = -1;
          break;
        }

      }

    } else {
      /* Timeout... */
      ret = -1;
      break;
    }

    temp = strstr(buffer, separator);

    if (temp) {
      if (strlen(temp)-4 == payloadLength) break;
    }

  } while (1);

  /*
   * Determine request 
   */
  if (!strncmp(buffer, "POST", 4)) {
    temp = findMsgBody(buffer);
    parseMessage(fd, temp);
  } else {
    write(fd, notimplemented, strlen(notimplemented));
  }
}


/*---------------------------------------------------------------------------
 * main() - The embedded HTTP server main
 *-------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  int listenfd, connfd, on=1;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;

  extern void userInit();

  /* Init the dynamic content test func */
  userInit();

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

  bzero((void *)&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(80);

  bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  listen(listenfd, 5);

  for ( ; ; ) {

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    if (connfd <= 0) break;

    handleConnection(connfd);
    close(connfd);

  }

  close(listenfd);
  return(0);
}

