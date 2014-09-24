#ifndef __GEEK_CORE_XML_H_
#define __GEEK_CORE_XML_H_

#include <string>
#include <vector>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

namespace Geek {
namespace Core {

class XMLDocument
{
 private:
    std::string m_filename;
    xmlDocPtr m_doc;

 public:
    XMLDocument(std::string file);
    ~XMLDocument();

    std::string getFilename();

    std::string readTextNode(std::string path);
    std::vector<xmlNodePtr> evalPath(std::string path);
};

}
}

#endif
