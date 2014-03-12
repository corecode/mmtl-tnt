#----------------------------------------------------------------
#
#  configure.ac
#
#  Autoconf source file for Boundary Element MMTL (bem).
#  This file, along with aclocal, autoconf, Makefile.am,
#  and automake can produce a highly functional configuration
#  script and Makefile.
#
#  Sharon Zahn, Bob Techentin
#  October 3, 2002
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: configure.ac,v 1.11 2004/09/10 20:04:53 techenti Exp $
#
#----------------------------------------------------------------


#----------------------------------------------------------------
#  Declare the package name and version.
#  All the bouquets and brickbats go to Sharon. :-)
#----------------------------------------------------------------
AC_INIT([TNT MMTL],[1.2.2],[techentin.robert@mayo.edu])

#----------------------------------------------------------------
#  Give autoconf a point of reference, and tell it that
#  we're going to use automake later.
#----------------------------------------------------------------
AC_CONFIG_SRCDIR(tnt.tcl)
AM_INIT_AUTOMAKE

#----------------------------------------------------------------
#  Check platform name so we can specify particular
#  compilers and switches that we like.  Not used
#  at this level, but some subdirectories need this.
#----------------------------------------------------------------
AC_CANONICAL_HOST

#----------------------------------------------------------------
#  On Windows using Cygwin
#  Set a WINDOWS conditional for automake so
#  that	the makefile can install extra executables.
#  (we just copy PrFile32.exe to ./bin)
#----------------------------------------------------------------
AM_CONDITIONAL(WINDOWS, [[[[ $host = *cygwin* ]]]] )



AC_CONFIG_SUBDIRS(bem calcCAP calcRL csdl gui units sppdgTcllib)
 
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([doc/Makefile])
AC_OUTPUT







