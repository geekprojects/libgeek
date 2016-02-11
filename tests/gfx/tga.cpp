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


#include <geek/gfx-surface.h>

using namespace Geek;
using namespace Geek::Gfx;

int main(int argc, char** argv)
{
    Surface* test1 = Surface::loadImage("test.tga");
if (test1 != NULL)
{
test1->saveJPEG("out1.jpg");
delete test1;
}

    Surface* test2 = Surface::loadImage("test2.tga");
if (test2 != NULL)
{
test2->saveJPEG("out2.jpg");
delete test2;
}
    return 0;
}

