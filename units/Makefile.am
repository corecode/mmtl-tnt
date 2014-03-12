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
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: Makefile.am,v 1.2 2004/02/05 22:24:23 techenti Exp $
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
	units.tcl \
	pkgIndex.tcl \
	units.html
 
#---------------------------------------------------------------
#  Define an installation directory that includes
#  the version number (from AC_INIT) like "bltmath1.0"
#---------------------------------------------------------------
pkglibdir = $(libdir)/$(PACKAGE)$(PACKAGE_VERSION)


