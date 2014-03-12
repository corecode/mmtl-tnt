#----------------------------------------------------------------
#
#  Makefile.am
#
#  Automake source file for sppdgTcllib
#  around Tkcon.
#
#  This file, along with aclocal, configure.ac autoconf,
#  and automake can produce a highly functional configuration
#  script and Makefile.
#
#  Bob Techentin
#  January 22, 2004
#  Copyright 2004 Mayo Foundation.  All Rights Reserved
#
#  $Id: Makefile.am,v 1.3 2004/05/10 18:47:15 schrauf Exp $
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
#  Only one script that needs to be installed.
#---------------------------------------------------------------
pkglib_SCRIPTS = \
	pkgIndex.tcl

#---------------------------------------------------------------
#  Define an installation directory.
#
#  Normal libraries would get something like
#     pkglibdir=$(libdir)/$(PACKAGE)$(PACKAGE_VERSION)
#  which would put a nicely named and versioned library
#  into $(prefix)/lib.
#
#  But sppdgTcllib is really just a container library,
#  and SUBDIRS are supposed to go into a subdirectory
#  of sppdgTcllib.  So our configure.ac changes
#  $(libdir) to point to the installed sppdgTcllib,
#  so that SUBDIRS will go in the right place.
#
#  Thus, our pkglibdir becomes...
#---------------------------------------------------------------
pkglibdir = $(prefix)/lib/$(PACKAGE)$(PACKAGE_VERSION)


#---------------------------------------------------------------
#  Install these subdirectories
#---------------------------------------------------------------
SUBDIRS = console getFilePreview sgraph addressDialog

AM_MAKEFLAGS = "libdir=$(pkglibdir)"