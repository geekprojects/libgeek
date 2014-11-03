
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

