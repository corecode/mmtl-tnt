# Tcl package index file
#
# This file is sourced either when an application starts up or
# by a "package unknown" script.  It invokes the
# "package ifneeded" command to set up package-related
# information so that packages will be loaded automatically
# in response to "package require" commands.  When this
# script is sourced, the variable $dir must contain the
# full path name of this file's directory.
#
#  Copyright 2004 Mayo Foundation.  All rights reserved.
#  $Id: pkgIndex.tcl,v 1.4 2004/02/10 15:57:31 techenti Exp $
#

package ifneeded console 1.1 "
[list source [file join $dir tkcon.tcl]]
[list source [file join $dir console.tcl]]
"





