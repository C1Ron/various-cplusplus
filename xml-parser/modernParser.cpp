#include <iostream>
#include <fstream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <memory>
#include <stdexcept>

// Function to write generated C++ code to a file
void generateCppFunction(const std::string& functionName, const std::string& symbol, std::ofstream& cppFile) {
    cppFile << "// " << functionName << " function\n";
    cppFile << "double " << functionName << "(double a, double b) {\n";
    cppFile << "    return a " << symbol << " b;\n";
    cppFile << "}\n\n";
}

// Custom deleter for xmlDocPtr to ensure cleanup
struct XmlDocDeleter {
    void operator()(xmlDocPtr doc) const {
        if (doc) {
            xmlFreeDoc(doc);
        }
    }
};

// Parse the XML and generate the corresponding C++ functions
void parseAndGenerateCode(const std::string& xmlFilename, const std::string& outputFilename) {
    // Initialize libxml parser
    xmlInitParser();

    // Load and parse the XML file
    std::unique_ptr<xmlDoc, XmlDocDeleter> doc(xmlReadFile(xmlFilename.c_str(), nullptr, 0));
    if (!doc) {
        throw std::runtime_error("Could not parse XML file: " + xmlFilename);
    }

    // Get the root element node
    xmlNode* root_element = xmlDocGetRootElement(doc.get());
    if (!root_element) {
        throw std::runtime_error("Empty XML document: " + xmlFilename);
    }

    // Open the file to write the generated C++ code
    std::ofstream cppFile(outputFilename);
    if (!cppFile) {
        throw std::runtime_error("Failed to open output file: " + outputFilename);
    }

    cppFile << "// Auto-generated math operations from " << xmlFilename << "\n\n";
    cppFile << "#include <iostream>\n\n";

    // Iterate through the XML nodes
    for (xmlNode* currentNode = root_element->children; currentNode; currentNode = currentNode->next) {
        if (currentNode->type == XML_ELEMENT_NODE && xmlStrcmp(currentNode->name, (const xmlChar*)"operation") == 0) {
            std::string functionName, symbol;

            // Iterate through the children of the <operation> element to extract details
            for (xmlNode* childNode = currentNode->children; childNode; childNode = childNode->next) {
                if (childNode->type == XML_ELEMENT_NODE) {
                    std::string content = reinterpret_cast<const char*>(xmlNodeGetContent(childNode));

                    if (xmlStrcmp(childNode->name, (const xmlChar*)"function") == 0) {
                        functionName = content;
                    } else if (xmlStrcmp(childNode->name, (const xmlChar*)"symbol") == 0) {
                        symbol = content;
                    }
                }
            }

            // Generate the C++ function based on the parsed details
            generateCppFunction(functionName, symbol, cppFile);
        }
    }

    // Clean up libxml
    xmlCleanupParser();
}

int main() {
    try {
        // XML file to parse and output C++ file
        parseAndGenerateCode("operations.xml", "generated_operations.cpp");
        std::cout << "C++ code has been generated in 'generated_operations.cpp'." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
