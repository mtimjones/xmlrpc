/*
 *  Lightweight Embedded XML-RPC Server XML Parser
 *
 *  mtj@mtjones.com
 *
 */

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "xmlparser.h"
#include "errors.h"

xmlCall_t xmlCall;

static char data[81];

char *errorStrings[]={
        /*  0 */        "Internal error (unknown)",
        /*  1 */        "Parse Error...",
        /*  2 */        "Function not found...",
	/*  3 */	"Unexpected Integer Argument...",
	/*  4 */	"Unexpected Boolean Argument...",
	/*  5 */	"Unexpected Double Argument...",
	/*  6 */	"Unexpected String Argument...",
	/*  7 */	"Bad Response Argument..."
};

#define MAX_ERROR_CODE  (sizeof(errorStrings)/sizeof(char *))


int getElement( parseBuf_t *pBuf, char *data, int dataSize )
{
  int j=0;
  int ret=XML_NO_ERROR;

  while ( !isprint(pBuf->buf[pBuf->index]) ) pBuf->index++;

  if (pBuf->index >= pBuf->len) return DONE;

  if (pBuf->buf[pBuf->index] == '<') ret = TAG;
  else ret = VALUE;

  data[j++] = pBuf->buf[pBuf->index++];

  while (j < dataSize) {

    if (pBuf->buf[pBuf->index] == '>') {
      data[j++] = pBuf->buf[pBuf->index++];
      break;
    } else if ((pBuf->buf[pBuf->index] == '\n') ||
               (pBuf->buf[pBuf->index] == '<')) {
      break;
    } else {
      data[j++] = pBuf->buf[pBuf->index++];
      if (j >= dataSize) ret = XML_PARSE_ERROR;
    }

  }

  data[j] = 0;

  return ret;
}


int parseParameter( parseBuf_t *parseBuffer )
{
  int type;

  /* Next, we need a <value> tag */
  type = getElement( parseBuffer, data, 80 );
  if (!((type == TAG) && (!strncmp(data, "<value>", 7)))) 
    return XML_PARSE_ERROR;

  /* Now we get a variable tag, the type of the value */
  type = getElement( parseBuffer, data, 80 );
  if (type != TAG) return XML_PARSE_ERROR;

  if (!strncmp(data, "<i4>", 4)) {
    xmlCall.argList[xmlCall.argCount] = 'i';
  } else if (!strncmp(data, "<int>", 5)) {
    xmlCall.argList[xmlCall.argCount] = 'i';
  } else if (!strncmp(data, "<boolean>", 9)) {
    xmlCall.argList[xmlCall.argCount] = 'b';
  } else if (!strncmp(data, "<double>", 8)) {
    xmlCall.argList[xmlCall.argCount] = 'd';
  } else if (!strncmp(data, "<string>", 8)) {
    xmlCall.argList[xmlCall.argCount] = 's';
  } else {
    return XML_PARSE_ERROR;
  }

  /* Now, parse the actual value */
  type = getElement( parseBuffer, data, 80 );
  if (type != VALUE) return XML_PARSE_ERROR;

  switch( xmlCall.argList[xmlCall.argCount] ) {
    case 'i':
    case 'b':
      xmlCall.arguments[xmlCall.argCount].u.i = atoi( data );
      break;
    case 'd':
      xmlCall.arguments[xmlCall.argCount].u.d = atof( data );
      break;
    case 's':
      strcpy(xmlCall.arguments[xmlCall.argCount].u.string, data);
      break;
    default:
      return XML_PARSE_ERROR;
  }

  xmlCall.argCount++;

  /* Now we close out the tag, starting with the type */
  type = getElement( parseBuffer, data, 80 );
  if (!((type == TAG) && (!strncmp(data, "</", 2)))) return XML_PARSE_ERROR;

  /* Next, look for the </value> close */
  type = getElement( parseBuffer, data, 80 );
  if (!((type == TAG) && (!strncmp(data, "</value>", 8)))) 
    return XML_PARSE_ERROR;

  /* Finally, close out the </param> tag */
  type = getElement( parseBuffer, data, 80 );
  if (!((type == TAG) && (!strncmp(data, "</param>", 8)))) 
    return XML_PARSE_ERROR;

  return XML_NO_ERROR;
}


int parseParameters( parseBuf_t *parseBuffer )
{
  int type, ret = XML_PARSE_ERROR;

  /* First, look for the params tag */
  type = getElement( parseBuffer, data, 80 );
  if ((type == TAG) && (!strncmp(data, "<params>", 8))) {

    while (1) {

      /* Get next tag */
      type = getElement( parseBuffer, data, 80 );
      if ((type == TAG) && (!strncmp(data, "<param>", 7))) {
        ret = parseParameter( parseBuffer );
      } else if ((type == TAG) && (!strncmp(data, "</params>", 9))) {
        return XML_NO_ERROR;
      } else {
        return XML_PARSE_ERROR;
      }

    }

  }

  return ret;
}


int parseMethodName( parseBuf_t *parseBuffer )
{
  int type, ret = XML_PARSE_ERROR;

  bzero( (void *)&xmlCall, sizeof(xmlCall_t) );

  /* Look for the methodName tag */
  type = getElement( parseBuffer, data, 80 );
  if ((type == TAG) && (!strncmp(data, "<methodName>", 12))) {

    /* get the method name for the call */
    type = getElement( parseBuffer, data, 80 );
    if (type == VALUE) {

      /* Save the method name */
      strcpy( xmlCall.methodName, data );

      /* Find the closing /methodCall */
      type = getElement( parseBuffer, data, 80 );
      if ((type == TAG) && (!strncmp(data, "</methodName>", 13))) {

        /* Now, it's time to parse the parameters */
        ret = parseParameters( parseBuffer );

      }

    }

  }

  return ret;
}


void sendFault( int fault )
{
  fault = -fault;
  if (fault >= MAX_ERROR_CODE) fault = 0;

  xmlBuildResponse( &xmlCall, "{is}", 
                     "faultCode", fault, "faultString", errorStrings[fault]);
}


int parseMessage( int sock, char *buffer )
{
  parseBuf_t parseBuffer;
  int type;
  int ret = XML_PARSE_ERROR;

  parseBuffer.buf = buffer;
  parseBuffer.len = strlen(buffer);
  parseBuffer.index = 0;

  /* Parse the xml header tag */
  type = getElement( &parseBuffer, data, 80 );
  if ((type == TAG) && (!strncmp(data, "<?xml", 5))) {

    type = getElement( &parseBuffer, data, 80 );
    if ((type == TAG) && (!strncmp(data, "<methodCall>", 12))) {

      /* Parse the remaining tags within the methodCall tag */
      parseMethodName( &parseBuffer );

      /* Check for the closing /methodCall */
      type = getElement( &parseBuffer, data, 80 );
      if ((type == TAG) && (!strncmp(data, "</methodCall>", 13))) {

        /* Successful parse, try to call a user function */
        ret = callUserFunc( &xmlCall );

      }

    }

  }

  if (ret == 0) {
    write(sock, xmlCall.response, strlen(xmlCall.response));
  } else {
    /* Send fault response */
    xmlCall.error = 1;
    sendFault( ret );
    write(sock, xmlCall.response, strlen(xmlCall.response));
  }

  return ret;
}

