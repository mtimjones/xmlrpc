/*
 *  Lightweight Embedded XML-RPC Server Response Generator
 *
 *  mtj@mtjones.com
 *
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "xmlparser.h"
#include "errors.h"

int getIntegerArg( xmlCall_t *xmlCall, int *arg )
{
  if ((xmlCall == NULL) || (arg == NULL)) return XML_INTERNAL_ERROR;

  if ((xmlCall->argIndex < xmlCall->argCount) &&
      (xmlCall->argList[xmlCall->argIndex] == 'i')) {

    *arg = xmlCall->arguments[xmlCall->argIndex++].u.i;

    return 0;
  }

  return XML_UNEXPECTED_INTEGER_ARG;
}


int getBooleanArg( xmlCall_t *xmlCall, int *arg )
{
  if ((xmlCall == NULL) || (arg == NULL)) return XML_INTERNAL_ERROR;

  if ((xmlCall->argIndex < xmlCall->argCount) &&
      (xmlCall->argList[xmlCall->argIndex] == 'b')) {

    *arg = xmlCall->arguments[xmlCall->argIndex++].u.i;

    return 0;
  }

  return XML_UNEXPECTED_BOOLEAN_ARG;
}


int getDoubleArg( xmlCall_t *xmlCall, double *arg )
{
  if ((xmlCall == NULL) || (arg == NULL)) return XML_INTERNAL_ERROR;

  if ((xmlCall->argIndex < xmlCall->argCount) &&
      (xmlCall->argList[xmlCall->argIndex] == 'd')) {

    *arg = xmlCall->arguments[xmlCall->argIndex++].u.d;

    return 0;
  }

  return XML_UNEXPECTED_DOUBLE_ARG;
}


int getStringArg( xmlCall_t *xmlCall, char *arg )
{
  if ((xmlCall == NULL) || (arg == NULL)) return XML_INTERNAL_ERROR;

  if ((xmlCall->argIndex < xmlCall->argCount) &&
      (xmlCall->argList[xmlCall->argIndex] == 's')) {

    strcpy(arg, xmlCall->arguments[xmlCall->argIndex++].u.string);

    return 0;
  }

  return XML_UNEXPECTED_STRING_ARG;
}


int insertContentLength( xmlCall_t *xmlCall )
{
  int len, digit, xdigit = 1000, i=0;
  char *temp;
  
  temp = strstr( xmlCall->response, "<?xml" );
  len = strlen( temp );

  temp = strstr( xmlCall->response, "xyza" );

  do {

    digit = (len / xdigit);
    len -= (digit * xdigit);
    xdigit /= 10;

    if ((digit == 0) && (xdigit > 1)) temp[i++] = ' ';
    else temp[i++] = (0x30 + digit);

  } while (i < 4);

  return 0;
}


int xmlBuildResponse( xmlCall_t *xmlCall, char *args, ... )
{
  va_list argp;
  int i, ret=0, index = 0, close = 0;
  double d;
  char *s;
  int isStruct = 0;

  if ((xmlCall == NULL) || (args == NULL)) return -1;

  strcpy(xmlCall->response, "HTTP/1.1 200 OK\n"
                            "Connection: close\n"
                            "Content-length: xyza\n"
                            "Content-Type: text/xml\n"
                            "Server: Lightweight XMLRPC\n\n"
                            "<?xml version=\"1.0\"?>\n"
                            "<methodResponse>\n");

  if (xmlCall->error) {
    strcat( &xmlCall->response[strlen(xmlCall->response)], "  <fault>\n");
  } else {
    strcat( &xmlCall->response[strlen(xmlCall->response)], 
             "  <params><param>\n");
  }

  va_start(argp, args);

  while (args[index]) {

    if (isStruct) {
      if ((args[index] != '{') && (args[index] != '}')) {
        sprintf( &xmlCall->response[strlen(xmlCall->response)], "  <member>\n");
        sprintf( &xmlCall->response[strlen(xmlCall->response)], 
                 "    <name>%s</name>\n", va_arg(argp, char *));
        close = 1;
      }
    }

    switch (args[index]) {

      case '{':
        sprintf( &xmlCall->response[strlen(xmlCall->response)], 
                 "  <value><struct>\n");
        isStruct = 1;
        break;

      case '}':
        sprintf( &xmlCall->response[strlen(xmlCall->response)], 
                 "  </struct></value>\n");
        isStruct = 0;
        break;

      case 'i':
        i = va_arg(argp, int);
        sprintf( &xmlCall->response[strlen(xmlCall->response)], 
                 "    <value><int>%d</int></value>\r\n", i );
        break;

      case 'b':
        i = va_arg(argp, int);
        sprintf( &xmlCall->response[strlen(xmlCall->response)], 
                 "    <value><boolean>%d</boolean></value>\r\n", i );
        break;

      case 'd':
        d = va_arg(argp, double);
        sprintf( &xmlCall->response[strlen(xmlCall->response)], 
                 "    <value><double>%f</double></value>\r\n", d );
        break;

      case 's':
        s = va_arg(argp, char *);
        sprintf( &xmlCall->response[strlen(xmlCall->response)], 
                 "    <value><string>%s</string></value>\r\n", s );
        break;

      default:
        return( XML_BAD_RESPONSE_ARG );
        break;

    }

    if (close) {
      sprintf( &xmlCall->response[strlen(xmlCall->response)], "  </member>\n" );
      close = 0;
    }

    index++;

  }

  va_end(argp);

  if (xmlCall->error) {
    strcat( &xmlCall->response[strlen(xmlCall->response)], "  </fault>\r\n");
  } else {
    strcat( &xmlCall->response[strlen(xmlCall->response)], 
             "  </param></params>\r\n");
  }

  if (ret == 0) {
    strcat( &xmlCall->response[strlen(xmlCall->response)], 
             "</methodResponse>\r\n");

    insertContentLength( xmlCall );
  } else {
    xmlCall->response[0] = 0;
  }

  return ret;
}

