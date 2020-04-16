/*
 *  Lightweight Embedded XML-RPC Server User Functions
 *
 *  mtj@mtjones.com
 *
 */

#include <string.h>
#include "xmlparser.h"
#include "errors.h"

int getDeviceStats( xmlCall_t *xmlCall )
{
  char username[80], password[80];
  char lastCommand[80], curState[80];
  int request = 0, status, ret;

  do {

    ret = getStringArg( xmlCall, username );
    if (ret != XML_NO_ERROR) break;

    ret = getStringArg( xmlCall, password );
    if (ret != XML_NO_ERROR) break;

    ret = getIntegerArg( xmlCall, &request );
    if (ret != XML_NO_ERROR) break;
  
  } while (0);

  if (ret == XML_NO_ERROR) {

    /* Dummy up some data... */
    status = 1;
    strcpy(lastCommand, "reboot");
    strcpy(curState, "Normal Operation");

    ret = xmlBuildResponse( xmlCall, "{iss}", 
                    "status", status, 
                    "lastCommand", lastCommand, 
                    "currentState", curState );

  }

  return ret;
}

