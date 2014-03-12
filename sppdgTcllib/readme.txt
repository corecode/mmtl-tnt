Documentation for adding a module to the sppdgTcllib library
Last modified by djs, 05/10/04

0) In the source code module you are adding to the library, put this line of code:

package provide myModule 1.0


1) checkout the sgraph project

2) copy the files Makefile.am, bootstrap, and configure.ac into the directory of your new module.

3) Edit the Makefile.am file to reflect your module files

4) Edit the configure.ac file to reflect your module files

5) create a pkgIndex file using the pkg-mkIndex command

6) checkout sppdgTcllib from cvs

7) create the modules sub-directory under sppdgTcllib

8) copy all of your modules files into the new sun-directory.

9) Add the new directory into cvs

10) Add the new files into cvs

11) Commit the changes to cvs

12) At this point everything is in cvs. Now install the new library for use.

13) edit the sppdgTcllib Makefile.am and configure.ac files and add (append) to them the
new sun-directory paths

14) run

./bootstrap

./configure --prefix=//sppdgfs/software/lib/common

make

make install

mv //sppdgfs/software/lib/common/lib/sppdgTcllib* \
   //sppdgfs/software/lib/common
  
rmdir //sppdgfs/software/lib/common/lib


