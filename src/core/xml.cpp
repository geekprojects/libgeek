/*
 *  libgeek - The GeekProjects utility suite
 *  Copyright (C) 2014, 2015, 2016 GeekProjects.com
 *
 *  This file is part of libgeek.
 *
 *  libgeek is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libgeek is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with libgeek.  If not, see <http://www.gnu.org/licenses/>.
 */


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

    if (start == NULL)
    {
        return results;
    }

    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xpathCtx = xmlXPathNewContext(m_doc);

    xpathCtx->node = start;
    xpathObj = xmlXPathEval((xmlChar*)path.c_str(), xpathCtx);

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

