/*
 *  Lightweight Embedded XML-RPC Server Error Definitions
 *
 *  mtj@mtjones.com
 *
 */

#ifndef ERRORS_H
#define ERRORS_H

#define XML_NO_ERROR			(0)
#define XML_PARSE_ERROR			(-1)
#define XML_NO_SUCH_FUNCTION		(-2)
#define XML_UNEXPECTED_INTEGER_ARG	(-3)
#define XML_UNEXPECTED_BOOLEAN_ARG	(-4)
#define XML_UNEXPECTED_DOUBLE_ARG	(-5)
#define XML_UNEXPECTED_STRING_ARG	(-6)
#define XML_BAD_RESPONSE_ARG		(-7)

#define XML_INTERNAL_ERROR		(-99)

#endif /* ERRORS_H */

