libgeek
=======

[![Build Status](https://travis-ci.org/geekprojects/libgeek.svg?branch=develop)](https://travis-ci.org/geekprojects/libgeek)

What?
-----------

This suite contains lots of useful bits of code that I often end up recreating
or copying between my projects.

I've divided the code up in to separate libraries (Hence the word suite) to
make it easier to just just the bits I'm after:

* core - Useful for all sorts of projects
* gfx  - Useful for graphics and drawing
* gtk  - Useful gtkmm widgets etc

Why?
----

I've largely been maintaining these for my own benefit. I know the same
functionality will be available in other libraries but I've learnt a lot by
doing it myself and they also more closely match the way I do things, rather
than having to match someone else's style.

But I hope they will also be of use to someone else!


Where?
------

I've been developing this library on both Mac OS X (10.11) and Linux.


Who?
----

Ian Parker <ian@geekprojects.com?


Installation
------------

./autogen.sh (If cloned from git)
./configure
make
sudo make install


License
-------

These libraries are available under the LGPL v3 license. You can use them
in any open or closed source projects, but if you make changes to any of
my code, you have to make those changes available.

libgeek is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libgeek is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.


