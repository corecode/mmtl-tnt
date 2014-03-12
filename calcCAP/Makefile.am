#----------------------------------------------------------------
#
#  Makefile.am
#
#  Automake source file for Boundary Element MMTL (bem).
#  This file, along with aclocal, configure.ac autoconf,
#  and automake can produce a highly functional configuration
#  script and Makefile.
#
#  Sharon Zahn, Bob Techentin
#  October 3, 2002
#
#  Copyright 2002 (C) Mayo Foundation.  All Rights Reserved
#
#----------------------------------------------------------------

#----------------------------------------------------------------
#  Just for the record, this isn't a GNU project, and
#  doesn't have all the support files like LICENSE and
#  ChangeLog.  We're also compiling in a subdirectory, so
#  we supply a couple of options to automake.
#
#  Note that this package _does_ require some of the support
#  files like install-sh, so if you're running aclocal,
#  autoconf, and automake for the first time, you'll need to
#  supply command line switches --add-missing --copy.
#----------------------------------------------------------------
AUTOMAKE_OPTIONS = foreign subdir-objects

#---------------------------------------------------------------
# tcl/tk scripts that may need to be installed.
#---------------------------------------------------------------
pkglibexec_SCRIPTS = \
	lib/calcCAP_exe.tcl  \
	lib/calcCAP.tcl \
	lib/epl.bit \
	lib/pkgIndex.tcl

#----------------------------------------------------------------
#  nmmtl is our main target
#----------------------------------------------------------------
bin_PROGRAMS = calcCAP

#----------------------------------------------------------------
#  Each program depends on (mostly the same) source files
#----------------------------------------------------------------
calcCAP_SOURCES = $(cpp_SOURCES) src/calcCAP.cpp

#----------------------------------------------------------------
#  Most of nmmtl is written in C++
#----------------------------------------------------------------
cpp_SOURCES = \
src/adfunc.cpp \
src/bicgstab.cpp \
src/cmplxmat.cpp \
src/cmplxvec.cpp \
src/coif4.cpp \
src/coif6.cpp \
src/complex.cpp \
src/data.cpp \
src/daub2.cpp \
src/diffrm.cpp \
src/gauleg.cpp \
src/ivectorl.cpp \
src/matrix.cpp \
src/matrix3d.cpp \
src/myfunc.cpp \
src/prony.cpp \
src/sparse.cpp \
src/stdafx.cpp \
src/systsolv.cpp \
src/vector.cpp
