/*
 *  Lightweight Embedded XML-RPC Server Types and Definitions
 *
 *  mtj@mtjones.com
 *
 */

#ifndef XMLPRIV_H
#define XMLPRIV_H

typedef struct {
  char *buf;
  int  len;
  int  index;
} parseBuf_t;

#define TAG	0
#define VALUE	1
#define DONE	2

#define MAX_STRING	80

typedef struct {
  union {
    int i;
    char boolean;
    double d;
    char string[MAX_STRING+1];
  } u;
} arg_t;

#define MAX_ARGS	10
#define MAX_RESPONSE	2048

typedef struct {
  char  methodName[MAX_STRING];
  arg_t arguments[MAX_ARGS];
  char  argList[MAX_ARGS];
  int   argCount;
  int   argIndex;
  char  response[MAX_RESPONSE];
  int   error;
} xmlCall_t;

#define MAX_METHODS	10

typedef struct {
  int (*func)( xmlCall_t * );
  char methodName[MAX_STRING+1];
} methodEntry_t;

int parseMessage ( int sock, char *buffer );

int getIntegerArg( xmlCall_t *xmlCall, int *arg );
int getBooleanArg( xmlCall_t *xmlCall, int *arg );
int getDoubleArg( xmlCall_t *xmlCall, double *arg );
int getStringArg( xmlCall_t *xmlCall, char *arg );

int callUserFunc( xmlCall_t *xmlCall );

int xmlBuildResponse( xmlCall_t *, char *, ... );

#endif

