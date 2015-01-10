#include "mpd_policy.h"

void remove_spaces(char *str) {
  char *p1 = str, *p2 = str;
  do
    while (*p2 == ' ')
      p2++;
  while (*p1++ = *p2++);
} 

char *parse_attribute(xmlNode *attr_node) {
    xmlNode *it = 0;
    char *attr = 0;
    char *attr_value = 0;
    char *attr_name = 0;
    char *attr_operator = 0;
    int attr_value_len, attr_name_len, attr_operator_len;

    for (it = attr_node; it; it = it->next) {
        if (it->type == XML_ELEMENT_NODE) {
            if (strcmp((char *)it->name, ATTRIBUTE_NAME) == 0) {
                attr_name = (char *)it->last->content;
            } else if (strcmp((char *)it->name, ATTRIBUTE_VALUE) == 0) {
                attr_value = (char *)it->last->content;
            } else if (strcmp((char *)it->name, ATTRIBUTE_OPERATOR) == 0) {
                attr_operator = (char *)it->last->content;
            } else {
                printf("Invalid Node\n");
                exit(1);
            }
        }
    }

    if (!attr_value || !attr_operator || !attr_name) {
        printf("The attribute is not complete\n");
        exit(2);
    }
    
    /* Remove white spaces */
    remove_spaces(attr_value);
    remove_spaces(attr_name);
    remove_spaces(attr_operator);

    if (strcmp(attr_value, "*") != 0) {
        attr_value_len = strlen(attr_value);
        attr_operator_len = strlen(attr_operator);
        attr_name_len = strlen(attr_name);
        
        attr = malloc((attr_value_len + attr_operator_len + attr_name_len + 7) * sizeof(*attr));
        assert(attr);
        
        sprintf(attr, "( %s %s %s )", attr_name, attr_operator, attr_value);
        attr[attr_value_len + attr_operator_len + attr_name_len + 7] = '\0';

        return attr;
    } else {
        return attr_name;
    }
}

char *parse_attributes_group(xmlNode *attr_group_node) {
    xmlNode *it = 0;
    char *operator_value = 0;
    char *operator_type = 0;
    xmlAttrPtr attr = 0;
    char *attr_group_str;
    char *res;

    attr_group_str = malloc(MAX_POLICY_LENGTH * sizeof(*attr_group_str));
    assert(attr_group_str);
    strcpy(attr_group_str, ""); 

    if (strcmp((char*)(attr_group_node->parent->name), ATTRIBUTES_GROUP) == 0) {
        for (attr = attr_group_node->parent->properties; attr ; attr = attr->next) {
            if (strcmp((char *)attr->name, XML_ATTR_OPERATOR_TYPE) == 0) {
                operator_type = (char*)attr->children->content;
            } else if (strcmp((char *)attr->name, XML_ATTR_OPERATOR_VALUE) == 0) {
                operator_value = (char*)attr->children->content;
            } else {
                printf ("Attribute %s is not a valid one\n", attr->name);
                exit(3);
            }
        }
    }

    if (operator_type && strcmp(operator_type, COMPOSITION_TYPE) == 0) {
        strcat(attr_group_str, operator_value);
        strcat(attr_group_str, " of (");
    } else {
        strcat(attr_group_str, "( ");
    }

    for (it = attr_group_node; it; it = it->next) {
        if (it->type == XML_ELEMENT_NODE) {
            if (strcmp((char *)it->name, ATTRIBUTES_GROUP) == 0) {
                res = parse_attributes_group(it->children);
            } else if (strcmp((char *)it->name, ATTRIBUTE_ELEMENT) == 0) {
                res = parse_attribute(it->children);
            } else {
                printf("BAD FORMAT\n");
                exit(1);
            }
            
            strcat(attr_group_str, res);
            free(res);

            /* Print the operator */
            if (it->next && it->next->next) {
                if (operator_type && strcmp(operator_type, LOGICAL_TYPE) == 0) {
                    strcat(attr_group_str, " ");
                    strcat(attr_group_str, operator_value);
                    strcat(attr_group_str, " ");
                } else {
                    strcat(attr_group_str, ", ");
                }
            } 
        }
    }
    
    strcat(attr_group_str, " )");

    return attr_group_str;
}

static void
parse_nodes(xmlNodeSetPtr nodes) {
    int i, size;
    char *result;

    size = (nodes) ? nodes->nodeNr : 0;
    
    for(i = size - 1; i >= 0; i--) {
        assert(nodes->nodeTab[i]);
        
        xmlNode *it = NULL;
        
        for (it = nodes->nodeTab[i]->children; it; it = it->next) {
            if (it->type == XML_ELEMENT_NODE &&
                strcmp((char *)it->name, ATTRIBUTES_GROUP) == 0) {
                result = parse_attributes_group(it);
                printf("%s\n", result);
                free(result);

                break;
            }
        }
    }
}

int parse(xmlDocPtr xml_doc, const xmlChar* xpathExpr) {
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj; 
    
    assert(xml_doc);
    assert(xpathExpr);

    /* Create xpath evaluation context */
    if (! (xpathCtx = xmlXPathNewContext(xml_doc))) {
        printf("Failed to create new XPath context\n"); 
        return -1;
    }
    
    /* Evaluate xpath expression */
    if (! (xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx))) {
        printf("Failed to evaluate xpath expression \"%s\"\n", xpathExpr);
        xmlXPathFreeContext(xpathCtx); 
        return -1;
    }
    
    /* update selected nodes */
    parse_nodes(xpathObj->nodesetval);

    /* Cleanup of XPath data */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 
    
    return(0);
}

int main(int argc, char* argv[]) {
    xmlDocPtr xml_doc;

    if (argc != 2) {
        printf ("Wrong number of arguments!\n");
        return 0;
    }
    
    if (! (xml_doc = xmlReadFile(argv[1], NULL, XML_PARSE_NONET))) {
        printf ("Failed to open %s file\n", argv[1]);
        return 1;
    }

    /* Parse the document*/
    parse(xml_doc, BAD_CAST ATTRIBUTES_ROOT);
    
    xmlFreeDoc(xml_doc);

    return 0;    
}
