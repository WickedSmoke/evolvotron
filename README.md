Evolvotron
==========

Evolvotron is interactive "generative art" software to evolve
images/textures/patterns through an iterative process of random
mutation and user-selection driven evolution.  If you like lava-lamps,
and still think the Mandelbrot set is cool, this could be the software
for you.

It uses C++, Boost, and the Qt GUI toolkit (v5.5 or later).
It's multithreaded (using Qt's threading API).

Home page: http://www.bottlenose.net/share/evolvotron

This file describes how to build evolvotron.
See the USAGE file for details of what the built executable can do.

If you manage to make practical use of evolvotron, especially
if evolvotron derived imagery makes it into print or other
mass media, I'd love to hear about it: please email!

Have fun
Tim

![Screenshot](https://wickedsmoke.github.io/image/evolvotron.jpg)

LICENSE
-------
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

[The license should be in the LICENSE file in the same directory as this
README]

BUILDING
--------
There's no reason it shouldn't work on any platform with a correctly
set up qmake.  You do NOT need to be root (there is no install stage).

In the top level directory, you can either do

    qmake "VERSION_NUMBER=x.x.x" main.pro
    make

or just have both done for you by doing

    ./BUILD.sh

which will pick up the VERSION_NUMBER from the file VERSION in this directory.

Make will recurse into and build the libfunction & libevolvotron directories
(which is 99% of the work) and some additional subdirectories with 
executables.

Among other things, this will give you an `evolvotron`
executable which you can run immediately with

    ./evolvotron/evolvotron

and/or copy where you like (there are no shared libs or "resource files"
needing additional attention). 

See the `USAGE` file (or in-app manual) for instructions.

The author mainly tracks Debian stable.

### Debugging builds

Many build failures are simply because the necessary Qt build tools
aren't in your path:

    which qmake
    which moc

should both find something.

If you have gcc/c++ compile problems problems:

If you have to change anything, the chances are it should be changed in
`common.pro`.  Remember that any Makefiles you can see are generated
by qmake from .pro files and therefore hacking on the Makefiles is
generally a fruitless exercise.

Some source releases have had problems with other versions of gcc than
the ones I test on.  A COMPLETE record of a failed build would be
appreciated (including the initial display of the gcc version).
If you can fix it, then patches would be even better!


INSTALL
-------
The evolvotron sources don't try to provide an installer.
The assumption is that packagers will have a better idea of where
files should be copied to on their systems, and the tools to do it.
Doing `make install` will recursively descend into the various build
directories, but does nothing in each one.

The things you're likely to want to install are
(in order of interest):

The main executable and man page:

    ./evolvotron/evolvotron
    ./man/man1/evolvotron.1

User documentation (standalone version of the builtin manual):

    USAGE

An HTML version of the above:

    ./evolvotron.html

Command-line driven tools and their man pages:

    ./evolvotron_render/evolvotron_render
    ./man/man1/evolvotron_render.1
    ./evolvotron_mutate/evolvotron_mutate
    ./man/man1/evolvotron_mutate.1

There are NO extra supporting files built
(e.g shared libraries, config files, "resource" files)
which need to be in special places for the software to work.

PACKAGING
---------
There are a few things which might be useful to packagers:

mkdeb
 - script to build .deb files (using the strangely unpopular
"yada", and pbuilder).  This is used to build the .debs put
up on sourceforge, but the "official" Debian ones (and Ubuntu
derivatives) are more conventionally packaged by Gurkan Sengun. 
Yada is obsoleted as of Debian's "Wheezy" release however.

evolvotron.spec
 - For building RPM packages.

CODE DOCUMENTATION
------------------
If you have doxygen (and graphviz too) and want to build
the source code documentation, execute `doxygen` at the top level.
The documentation then appears in `./doc/html/`.
