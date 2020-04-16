/*
 *  Lightweight Embedded XML-RPC Server initialization functions
 *
 *  mtj@mtjones.com
 *
 */

#include <string.h>
#include "userfunc.h"
#include "errors.h"

methodEntry_t methods[MAX_METHODS];
int methodMax = 0;


int callUserFunc( xmlCall_t *xmlCall )
{
  int i, ret = XML_NO_SUCH_FUNCTION;

  /* Find a function name match */
  for (i = 0 ; i < methodMax ; i++) {

    if (xmlCall->methodName[0] == methods[i].methodName[0]) {
      if (!strcmp(xmlCall->methodName, methods[i].methodName)) {
        ret = methods[i].func( xmlCall );
        break;
      }
    }

  }

  return ret;
}


int addMethod( int (*func)(xmlCall_t *), char *name )
{
  if (methodMax == MAX_METHODS) return -1;

  strcpy( methods[methodMax].methodName, name );
  methods[methodMax].func = func;

  methodMax++;

  return 0;
}


void userInit( void )
{
  int ret;

  (void) ret;

  ret = addMethod( getDeviceStats, "getDeviceStats" );

  /* Add your new methods here */
}  

