#ifndef XML_PARSER_H_
#define XML_PARSER_H_   1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <libxml/parser.h>
#include <libxml/valid.h>
#include <libxml/xmlschemas.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#define FILE_EXTENSION      ".mpd"
#define SUFFIX              "_out"
#define SUFFIX_LEN          4
#define BASEURL_XPATH       "//*[name()='BaseURL']"
#define XSD_SCHEMA_PATH     "DASH-MPD.xsd"
#define VALIDATE_XML        1

int write_result(char *filename, xmlDocPtr xml_doc);
int parse(xmlDocPtr xml_doc, const xmlChar *xpathExpr);

#endif
