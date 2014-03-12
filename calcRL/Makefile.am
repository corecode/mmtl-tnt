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
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: Makefile.am,v 1.4 2004/02/12 22:09:10 techenti Exp $
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
	lib/calcRL.tcl \
	lib/calcRL_exe.tcl \
	lib/pkgIndex.tcl

#----------------------------------------------------------------
#  nmmtl is our main target
#----------------------------------------------------------------
bin_PROGRAMS = calcRL

#----------------------------------------------------------------
#  Each program depends on (mostly the same) source files
#----------------------------------------------------------------
calcRL_SOURCES = $(cpp_SOURCES) src/calcRL.cpp

##wavecyn_SOURCES = $(cpp_SOURCES) src/wavecyn.cpp

#----------------------------------------------------------------
#  Most of nmmtl is written in C++
#----------------------------------------------------------------
cpp_SOURCES = \
src/adfunc.cpp \
src/cmplxmat.cpp \
src/complex.cpp \
src/hankel.cpp \
src/matrix.cpp \
src/stdafx.cpp \
src/bicgstab.cpp \
src/cmplxvec.cpp \
src/data.cpp \
src/ivectorl.cpp \
src/myfunc.cpp \
src/systsolv.cpp \
src/coif4.cpp \
src/gauleg.cpp \
src/kelvin.cpp \
src/sparse.cpp \
src/vector.cpp
