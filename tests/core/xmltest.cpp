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


#include <stdio.h>
#include <geek/core-xml.h>

using namespace std;
using namespace Geek::Core;

int main(int argc, char** argv)
{
    XMLDocument doc("xmltest.xml");

    vector<xmlNodePtr> values = doc.evalPath("/root/child1/child2");

    printf("xmltest: value count=%lu\n", values.size());

    vector<xmlNodePtr>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
    {
        xmlNodePtr node = *it;
        printf("xmltest: value: %s\n", node->children->content);
    }

    return 0;
}

