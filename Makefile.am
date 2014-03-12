#----------------------------------------------------------------
#
#  Makefile.am
#
#  Automake source file for TNT
#  This file, along with aclocal, configure.ac autoconf,
#  and automake can produce a highly functional configuration
#  script and Makefile.
#
#  Sharon Zahn, Bob Techentin
#  October 3, 2002
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: Makefile.am,v 1.15 2004/07/22 14:23:32 techenti Exp $
#
#----------------------------------------------------------------

#----------------------------------------------------------------
#  Note that this package _does_ require some of the support
#  files like install-sh, so if you're running aclocal,
#  autoconf, and automake for the first time, you'll need to
#  supply command line switches --add-missing --copy.
#
#  Or see (or run) the 'bootstrap' script for details.
#----------------------------------------------------------------

#---------------------------------------------------------------
#  tcl/tk scripts that need to be installed.
#---------------------------------------------------------------
bin_SCRIPTS = tnt.tcl tnt

#---------------------------------------------------------------
#  Examples to be installed
#---------------------------------------------------------------
examplesdir = $(prefix)/examples
examples_DATA = \
	examples/9-7-00.xsctn \
	examples/coplanar.xsctn \
	examples/example-microstrip-2.xsctn \
	examples/example-microstrip-5.xsctn \
	examples/example-stripline-2.xsctn \
	examples/generic.xsctn \
	examples/test1.xsctn \
	examples/trap_test.xsctn \
	examples/w10t2.5.xsctn \
	examples/w20t5.xsctn

#---------------------------------------------------------------
#  Application documents get installed into the $(prefix)
#  directory, which will be nice if the installer specifies
#  a prefix.  If not, then I suppose README will go into
#  /usr.
#---------------------------------------------------------------
appdocsdir = $(prefix)
appdocs_DATA = \
	AUTHORS$(TXTEXT) \
	COPYING$(TXTEXT) \
	ChangeLog$(TXTEXT) \
	NEWS$(TXTEXT) \
	README$(TXTEXT) \
	THANKS$(TXTEXT)

#---------------------------------------------------------------
#  Convert application documents to text files on Windows.
#---------------------------------------------------------------
if WINDOWS
TXTEXT=.txt
%$(TXTEXT) : %
	cp $< $@
	unix2dos $@
endif

#---------------------------------------------------------------
#  Windows installation need a helper application to 
#  send postscript files through the print queue.
#  Copy PrFile32.exe and it's zip distribution to ./bin.
#---------------------------------------------------------------
if WINDOWS
printfiledir = $(bindir)
printfile_DATA = printfile215-32.zip PrFile32.exe
endif

#---------------------------------------------------------------
#  Subdirectories for automake and make
#---------------------------------------------------------------
SUBDIRS = bem calcCAP calcRL doc csdl gui units sppdgTcllib

EXTRA_DIST = $(bin_SCRIPTS) $(printfile_DATA) $(examples_DATA)
