#-----------------------------------------------------
#
# console.tcl --
#
#  Command console for Tk GUI applications.
#
#  This package provides a wrapper around TkCon - a highly
#  functional and very portable Tcl command console.  This
#  allows application users to type Tcl commands directly
#  into the executing application's Tcl interpreter.  It
#  can also serve as a log window, catching output from
#  [puts] statements.
#
#
#  See the README file for operating instructions..
#
#
#  Bob Techentin
#  January 22, 2004
#
#  Copyright 2004 Mayo Foundation.  All rights reserved.
#  $Id: console.tcl,v 1.8 2004/02/12 20:54:19 techenti Exp $
#
#-----------------------------------------------------

package require Tk
package require tkcon
package provide console 1.1

namespace eval console {
    namespace export create options
    namespace export show
    namespace export hide

    #  Define options and default values
    variable options
    array set options {
	-showOnStartup   0
	-root            .console
	-protocol        {console::hide}
        -showMenu        0
	-title           "Command Console"
	-variable        {}
    }
}


#-----------------------------------------------------
# console::create --
#
#  Create a console window - always a toplevel
#  but doesn't show it.
#
# Arguments:
#	None.
#
# Side Effects:
#	Creates a toplevel
#
# Results:
#	A list containing the names of all known levels,
#	alphabetically sorted.
#------------------------------------------------------

proc console::create {args} {

    variable options

    #------------------------------------------------------
    #  process options
    #------------------------------------------------------
    for {set i 0} {$i < [llength $args]} {incr i} {
	set arg [lindex $args $i]
	set val [lindex $args [incr i]]

	#  Copy values directly into options array
	if { [info exists options($arg)]} {
	    set options($arg) $val
	} else {
	    set names  [lsort [array names options]]
	    return -code error \
		-errorinfo "bad option \"$arg\":  must be $names"
	}
    }

    #------------------------------------------------------
    #  Create the console with our options.
    #  Note that tkcon forces itself into the global
    #  namespace, so we use fully qualified names.
    #------------------------------------------------------
    set ::tkcon::PRIV(showOnStartup) $options(-showOnStartup)
    set ::tkcon::PRIV(root)          $options(-root)
    set ::tkcon::PRIV(protocol)      $options(-protocol)
    set ::tkcon::OPT(showmenu)       $options(-showMenu)
    set ::tkcon::OPT(exec) ""
    ::tkcon::Init
    ::tkcon title $options(-title)

    #------------------------------------------------------
    #  If we have a variable name, then we need to set
    #  up a trace to make sure that we show/hide when
    #  that variable changes.
    #------------------------------------------------------
    if { "$options(-variable)" != "" } {

	#  First we have to translate the variable name
	#  into a fully qualified name (namespace which)
	#  in the context of the caller (uplevel).
	set options(-variable) [uplevel namespace which -variable $options(-variable)]

	#  Set initial value
	set $options(-variable) $options(-showOnStartup)

	#  Now set the trace
	trace add variable $options(-variable) write \
	    [namespace code variableChange]
    }
}

#-----------------------------------------------------
# console::show --
#
#  Show (deiconify) the console window
#  and optionally set the -variable.
#
# Arguments:
#   None.
#
# Side Effects:
#   shows the console window
#
# Results:
#   none.
#------------------------------------------------------
proc console::show {} {
    variable options
    ::tkcon show
    #  If a variable is defined, set the varible
    if { $options(-variable) != "" } {
	#  but only if we're not already in a trace
	catch {info level -1} level
	if {![string match "variableChange*" $level]} {
	    set [set options(-variable)] 1
	}
    }
}

#-----------------------------------------------------
# console::hide --
#
#  Hide (iconify) the console window
#  and optionally set the -variable.
#
# Arguments:
#   None.
#
# Side Effects:
#   hides the console window
#
# Results:
#   none.
#------------------------------------------------------
proc console::hide {} {
    variable options
    ::tkcon hide
    #  If a variable is defined, set the varible
    if { $options(-variable) != "" } {
	#  but only if we're not already in a trace
	catch {info level -1} level
	if {![string match "variableChange*" $level]} {
	    set [set options(-variable)] 0
	}
    }
}


#-----------------------------------------------------
# console::variableChange --
#
#  If the console was created with a -variable
#  that controls its visibility, then this procedure
#  is attached to a variable trace.  It shows or
#  hides the console, depending on the trueness or
#  falseness of the variable.
#
# Arguments:
#	name1 name2 op   (as is the case for variable traces)
#
# Side Effects:
#	Calls console show or console hide
#
# Results:
#	none
#------------------------------------------------------

proc console::variableChange {name1 name2 op} {
    #  We _could_ assemble the right variable name in
    #  the context of the name1, name2, and upvar,
    #  but we already did that once...
    variable options
    if { [set $options(-variable)] } {
	show
    } else {
	hide
    }
}

