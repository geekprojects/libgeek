
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

vector<xmlNodePtr> XMLDocument::evalPath(std::string path)
{
    vector<xmlNodePtr> results;

    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xpathCtx = xmlXPathNewContext(m_doc);
    xpathObj = xmlXPathEvalExpression((xmlChar*)path.c_str(), xpathCtx);

    if (xpathObj == NULL)
    {
        return results;
    }

    int i;
    for (i = 0; i < xpathObj->nodesetval->nodeNr; i++)
    {
        xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
        results.push_back(node);
    }

    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);

    return results;
}

string XMLDocument::readTextNode(std::string path)
{
    vector<xmlNodePtr> nodes = evalPath(path);

    string result = "";
    if (!nodes.empty())
    {
        xmlNodePtr node = nodes.at(0);
        result = string((char*)node->children->content);
    }
    return result;
}


