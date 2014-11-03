#include "xmlparser.h"

static int validate_xml(xmlDocPtr xml_doc) {
    int status = 0;
    xmlDocPtr schema_doc = NULL;
    xmlSchemaParserCtxtPtr parser_ctxt = NULL;
    xmlSchemaPtr schema = NULL;
    xmlSchemaValidCtxtPtr valid_ctxt = NULL;
    
    assert(xml_doc);

    if (! (schema_doc = xmlReadFile(XSD_SCHEMA_PATH, NULL, XML_PARSE_NONET))) {
        printf ("Failed to open %s file\n", XSD_SCHEMA_PATH);
        return 1;
    }

    if (! (parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc))) {
        printf ("Failed to create a parser using the schema document\n");
        status = 1;
        goto cleanup;
    }

    if (! (schema = xmlSchemaParse(parser_ctxt))) {
        status = 1;
        printf ("Failed to parse the schema\n");
        goto cleanup;
    }

    if (! (valid_ctxt = xmlSchemaNewValidCtxt(schema))) {
        status = 1;
        printf ("Failed to create the validation context\n");
        goto cleanup;
    }

    if (xmlSchemaValidateDoc(valid_ctxt, xml_doc)) {
        printf("The document is invalid\n");
        
        status = 1;
        goto cleanup;
    }

cleanup:
    if (valid_ctxt) xmlSchemaFreeValidCtxt(valid_ctxt);
    if (schema) xmlSchemaFree(schema);
    if (parser_ctxt) xmlSchemaFreeParserCtxt(parser_ctxt);
    if (schema_doc) xmlFreeDoc(schema_doc);

    return status;
}

static void
update_nodes(xmlNodeSetPtr nodes) {
    int i, size, old_len;
    char *new_value;
    
    size = (nodes) ? nodes->nodeNr : 0;
    
    for(i = size - 1; i >= 0; i--) {
        assert(nodes->nodeTab[i]);
        old_len = strlen((char*) nodes->nodeTab[i]->last->content);
        new_value = malloc((old_len + SUFFIX_LEN + 1) * sizeof(char));
        assert(new_value);
        
        //TODO use xmlStrcat function to concatenate
        /* Concatenate "_out" to the old value */ 
        memset(new_value, 0, old_len + SUFFIX_LEN + 1);
        memcpy(new_value, (char*) nodes->nodeTab[i]->last->content, old_len);
        memcpy(new_value + old_len, SUFFIX, SUFFIX_LEN);
        new_value[old_len + SUFFIX_LEN] = '\0';

        xmlNodeSetContent(nodes->nodeTab[i], BAD_CAST new_value);
        if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
            nodes->nodeTab[i] = NULL;
        
        free(new_value);
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
    update_nodes(xpathObj->nodesetval);

    /* Cleanup of XPath data */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 
    
    return(0);
}

static char *get_output_file(char *input) {
    char *output_file;
    int i, input_len;
    int pos;
    int suffix_added = 0;

    input_len = strlen(input);
    
    output_file = malloc((input_len + SUFFIX_LEN + 1) * sizeof(char)); 
    if (!output_file) {
        printf ("Failed to allocate memory\n");
        return NULL;
    }
    memset (output_file, (input_len + SUFFIX_LEN + 1), 0);
    
    pos = 0;
    for (i = 0; i < input_len; i++) {
        if (input[i] != '.') {
            output_file[pos++] = input[i];
        } else {
            memcpy(&output_file[pos], SUFFIX, SUFFIX_LEN);
            pos += SUFFIX_LEN;
            output_file[pos++] = input[i];
            suffix_added = 1;
        }
    }
        
    if (!suffix_added) {
        memcpy(&output_file[pos], SUFFIX, SUFFIX_LEN);
        pos += SUFFIX_LEN;
    }

    output_file[pos] = '\0';
    return output_file;    
}

int write_result(char *filename, xmlDocPtr xml_doc) {
    FILE *out;
    
    if (! (out = fopen(filename, "w")))
        return 1;
    
    /* Dump the content */
    xmlDocDump(out, xml_doc);
    
    /* Close the file */
    fclose(out);

    return 0;
}

int main(int argc, char* argv[]) {
    xmlDocPtr xml_doc;
    char *output_file;
    int validate = 0;

    if (argc < 2 || argc > 4) {
        printf ("Wrong number of arguments!\n"
                "\tThe correct usage is ./xmparser input [output] [validate], where:\n"
                "\t\tinput = the input file\n"
                "\t\toutput = the output file (optional parameter)\n"
                "\t\tvalidate = 0 or 1 (optional parameter). The default value is 0.\n"
                "\tIf validate = 1 then the program will try to validate the input file using the %s schema\n",
                XSD_SCHEMA_PATH);
        return 0;
    }

    if (argc > 2) {
        output_file = argv[2];
        if (argc == 4)
            validate = atoi(argv[3]);
    } else {    
        output_file = get_output_file(argv[1]);
        if (!output_file)
            return 1;
    }
    
    if (! (xml_doc = xmlReadFile(argv[1], NULL, XML_PARSE_NONET))) {
        printf ("Failed to open %s file\n", argv[1]);
        return 1;
    }

    if (validate == VALIDATE_XML) {
        if (validate_xml(xml_doc)) {
            printf ("The file is not valid\n");
            xmlFreeDoc(xml_doc);
            return 0;
        } else {
            printf ("The file is valid. Trying to convert it...\n");
        }
    }
    
    /* Parse the document and change the values from BaseURL elements */
    parse(xml_doc, BAD_CAST BASEURL_XPATH);
    
    /* Write the result */
    if (write_result(output_file, xml_doc)) {
        printf ("Failed to open the file\n");
    } else {
        printf ("You can find the result in the \"%s\" file.\n", output_file);
    }

    xmlFreeDoc(xml_doc);

    return 0;    
}
