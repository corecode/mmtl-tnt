# Tcl package index file for units package
#
# This file is sourced either when an application starts up or by a
# "package unknown" script.  It invokes the "package ifneeded" command
# to set up package-related information so that packages will be
# loaded automatically in response to "package require" commands.
# When this script is sourced, the variable $dir must contain the full
# path name of this file's directory.
#
# This particular index file first looks for a binary shared library,
# then the Tcl-only implementation.  If the shared library is found,
# it is loaded.  Otherwise the (default) Tcl implementation is
# sourced.  This style is based on the BLT package.

proc units_makepkgindex { dir } {
    set suffix [info sharedlibextension]
    set binary_library [file join $dir units$suffix]
    set tcl_library [file join $dir units.tcl]
    if { [file exists $binary_library] } {
	package ifneeded units 1.0 [list load $binary_library]
    } else {
	package ifneeded units 1.0 [list source $tcl_library]
    }
}

units_makepkgindex $dir
rename units_makepkgindex ""

