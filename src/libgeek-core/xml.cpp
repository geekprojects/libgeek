
#include <geek/core-xml.h>

using namespace std;
using namespace Geek::Core;

XMLDocument::XMLDocument(string filename)
{
    m_filename = filename;

    m_doc = xmlReadFile(
        filename.c_str(),
        NULL,
        XML_PARSE_NOCDATA | XML_PARSE_NOBLANKS);
}

XMLDocument::~XMLDocument()
{
    xmlFreeDoc(m_doc);
}

string XMLDocument::getFilename()
{
    return m_filename;
}

xmlNodePtr XMLDocument::evalPath(std::string path)
{
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xpathCtx = xmlXPathNewContext(m_doc);
    xpathObj = xmlXPathEvalExpression((xmlChar*)path.c_str(), xpathCtx);

    if (xpathObj == NULL || xpathObj->nodesetval->nodeNr == 0)
    {
        xmlXPathFreeContext(xpathCtx);
        return NULL;
    }

    xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];

    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);

    return node;
}

string XMLDocument::readTextNode(std::string path)
{
    xmlNodePtr node = evalPath(path);
    string result((char*)node->children->content);
    return result;
}


