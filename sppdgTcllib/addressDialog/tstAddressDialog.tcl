#!/bin/sh
# -*- tcl -*-
# The next line is executed by /bin/sh, but not tcl \
exec tclsh "$0" ${1+"$@"}

package require Tk
package require Tcl
package require Itcl

# Find the directory that the script started in
set scriptDir [file dirname [info script]]
source [file join $scriptDir addressDialog.tcl]

set adr 21
while { $adr != false } {
	set adr [ShowAddressDlg $adr]
}

exit
