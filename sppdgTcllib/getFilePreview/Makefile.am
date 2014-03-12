#----------------------------------------------------------------
#
#  Makefile.am
#
#  Automake source file for the getFilePreview widget package
#
#  This file, along with aclocal, configure.ac autoconf,
#  and automake can produce a highly functional configuration
#  script and Makefile.
#
#  Bob Techentin
#  January 22, 2004
#  Copyright 2004 Mayo Foundation.  All Rights Reserved
#
#  $Id: Makefile.am,v 1.2 2004/04/23 16:22:40 techenti Exp $
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
AUTOMAKE_OPTIONS = foreign

#---------------------------------------------------------------
# tcl/tk scripts that may need to be installed.
#---------------------------------------------------------------
pkglibexec_SCRIPTS = \
	getFilePreview.tcl \
	pkgIndex.tcl

#---------------------------------------------------------------
#  Define an installation directory that includes
#  the version number (from AC_INIT) like "getFilePreview1.0"
#---------------------------------------------------------------
pkglibexecdir = $(libexecdir)/$(PACKAGE)$(PACKAGE_VERSION)
