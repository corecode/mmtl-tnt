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
AUTOMAKE_OPTIONS = foreign

#---------------------------------------------------------------
# tcl/tk scripts to be installed.
#---------------------------------------------------------------
pkglibexec_SCRIPTS = \
	canvas_functions.tcl \
	gui.tcl \
	gui_build.tcl \
	gui_canvas.itcl \
	gui_elements.tcl \
	gui_help.tcl \
	gui_initialize.tcl \
	gui_xmission.tcl \
	pkgIndex.tcl \
	splash.tcl \
	style.tcl



#---------------------------------------------------------------
#  Data files to be installed
#---------------------------------------------------------------
pkgdata_DATA = \
	MayoSPPDGlogo.gif \
	conductivity.list \
	loss_tangent.list \
	loss_tangent2.list \
	permittivity.list \
	permittivity2.list

#---------------------------------------------------------------
#  Define an installation directory that includes
#  the version number (from AC_INIT) like "bltmath1.0"
#---------------------------------------------------------------
pkglibexecdir = $(libexecdir)/$(PACKAGE)$(PACKAGE_VERSION)
pkgdatadir = $(datadir)/$(PACKAGE)$(PACKAGE_VERSION)
