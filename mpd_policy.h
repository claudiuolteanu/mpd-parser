#ifndef MPD_POLICY_H_
#define MPD_POLICY_H_   1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#define LOGICAL_TYPE        "Logical"
#define COMPOSITION_TYPE    "Composition"

#define OPERATOR_AND        "AND"
#define OPERATOR_OR         "OR"

#define XML_ATTR_OPERATOR_VALUE    "OperatorValue"
#define XML_ATTR_OPERATOR_TYPE     "OperatorType"

#define ATTRIBUTES_GROUP    "AttributesGroup"
#define ATTRIBUTE_ELEMENT   "Attribute"
#define ATTRIBUTE_NAME      "Name"
#define ATTRIBUTE_VALUE     "Value"
#define ATTRIBUTE_OPERATOR  "Operator"

#define ATTRIBUTES_ROOT     "//*[name()='Representation']"

#define MAX_POLICY_LENGTH   1024

char *parse_attribute(xmlNode *node);
char *parse_attributes_group(xmlNode *node);

#endif
