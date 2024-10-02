#include <iostream>
#include <fstream>
#include <libxml/parser.h>
#include <libxml/tree.h>

// Function to write generated C++ code to a file
void generateCppFunction(const std::string& functionName, const std::string& symbol, std::ofstream& cppFile) {
    cppFile << "// " << functionName << " function\n";
    cppFile << "double " << functionName << "(double a, double b) {\n";
    cppFile << "    return a " << symbol << " b;\n";
    cppFile << "}\n\n";
}

int main() {
    // Initialize libxml
    xmlInitParser();
    
    // Parse the XML file
    xmlDoc *doc = xmlReadFile("operations.xml", NULL, 0);
    if (doc == NULL) {
        std::cerr << "Could not parse XML file." << std::endl;
        return -1;
    }

    // Get the root element node
    xmlNode *root_element = xmlDocGetRootElement(doc);

    // Open the file to write the generated C++ code
    std::ofstream cppFile("generated_operations.cpp");
    cppFile << "// Auto-generated math operations from operations.xml\n\n";
    cppFile << "#include <iostream>\n\n";

    // Iterate through XML nodes
    for (xmlNode *currentNode = root_element->children; currentNode; currentNode = currentNode->next) {
        if (currentNode->type == XML_ELEMENT_NODE && xmlStrcmp(currentNode->name, (const xmlChar *)"operation") == 0) {
            std::string functionName, symbol;

            // Traverse each operation node's children to get the details
            for (xmlNode *childNode = currentNode->children; childNode; childNode = childNode->next) {
                if (childNode->type == XML_ELEMENT_NODE) {
                    std::string content = (char *)xmlNodeGetContent(childNode);

                    if (xmlStrcmp(childNode->name, (const xmlChar *)"function") == 0) {
                        functionName = content;
                    } else if (xmlStrcmp(childNode->name, (const xmlChar *)"symbol") == 0) {
                        symbol = content;
                    }
                }
            }

            // Generate C++ function
            generateCppFunction(functionName, symbol, cppFile);
        }
    }

    // Clean up and free the XML document
    xmlFreeDoc(doc);
    xmlCleanupParser();

    // Close the C++ file
    cppFile.close();

    std::cout << "C++ code has been generated in 'generated_operations.cpp'." << std::endl;

    return 0;
}
