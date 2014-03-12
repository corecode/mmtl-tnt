#----------------------------------------------------------------
#
#  Makefile.am
#
#  Automake source file for the sgraph "supergraph"
#  for BLT graph widgets.
#
#  This file, along with aclocal, configure.ac autoconf,
#  and automake can produce a highly functional configuration
#  script and Makefile.
#
#  Bob Techentin
#  November 17, 2003
#
#  Copyright 2003 (C) Mayo Foundation.  All Rights Reserved
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
pkglib_SCRIPTS = \
	sgraph.tcl \
	pkgIndex.tcl

#---------------------------------------------------------------
#  Define an installation directory that includes
#  the version number (from AC_INIT) like "sgraph1.0"
#---------------------------------------------------------------
pkglibdir = $(libdir)/$(PACKAGE)$(PACKAGE_VERSION)


