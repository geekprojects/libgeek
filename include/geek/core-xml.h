/*
 * libgeek - The GeekProjects utility suite
 * Copyright (C) 2014, 2015, 2016 GeekProjects.com
 *
 * This file is part of libgeek.
 *
 * libgeek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libgeek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgeek.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    std::string readTextNode(std::string path, xmlNodePtr parent);
    std::vector<xmlNodePtr> evalPath(std::string path);
    std::vector<xmlNodePtr> evalPath(std::string path, xmlNodePtr parent);
    xmlNodePtr evalPathFirst(std::string path, xmlNodePtr node);
};

}
}

#endif
