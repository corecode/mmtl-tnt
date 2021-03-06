# Tcl package index file, version 1.1
# This file is generated by the "pkg_mkIndex -load Itcl" command
# and sourced either when an application starts up or
# by a "package unknown" script.  It invokes the
# "package ifneeded" command to set up package-related
# information so that packages will be loaded automatically
# in response to "package require" commands.  When this
# script is sourced, the variable $dir must contain the
# full path name of this file's directory.
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: pkgIndex.tcl,v 1.5 2004/07/22 18:58:00 techenti Exp $

package ifneeded calcRL 1.0 "
 [list set env(CALCRL_LIBRARY) $dir]
 [list source [file join $dir calcRL.tcl]]
"


