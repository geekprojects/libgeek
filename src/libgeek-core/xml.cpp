
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
    return evalPath(path, xmlDocGetRootElement(m_doc));
}

vector<xmlNodePtr> XMLDocument::evalPath(std::string path, xmlNodePtr start)
{
    vector<xmlNodePtr> results;

    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xpathCtx = xmlXPathNewContext(m_doc);
    xpathObj = xmlXPathNodeEval(start, (xmlChar*)path.c_str(), xpathCtx);

    if (xpathObj == NULL || xpathObj->nodesetval == NULL)
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

xmlNodePtr XMLDocument::evalPathFirst(std::string path, xmlNodePtr parent)
{
    vector<xmlNodePtr> nodes = evalPath(path, parent);
    if (!nodes.empty())
    {
        return nodes.at(0);
    }
    return NULL;
}

string XMLDocument::readTextNode(std::string path)
{
    return readTextNode(path, xmlDocGetRootElement(m_doc));
}

string XMLDocument::readTextNode(std::string path, xmlNodePtr parent)
{
    vector<xmlNodePtr> nodes = evalPath(path, parent);

    string result = "";
    if (!nodes.empty())
    {
        xmlNodePtr node = nodes.at(0);
        if (node->children != NULL)
        {
            xmlChar* str = node->children->content;
            if (str != NULL)
            {
                result = string((char*)str);
            }
        }
    }
    return result;
}

