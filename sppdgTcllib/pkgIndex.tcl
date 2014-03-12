# Tcl package index file, version 1.0
#
#  Genereated by hand by RWT on 10/14/2003.
#  Patterned after ActiveState TclDevKit tcllib library.
#
#  $Id: pkgIndex.tcl,v 1.1 2004/01/05 22:17:50 techenti Exp $
#

#  All sppdgTcllib packages need Tcl 8.4
#  We use Tcl 8 namespaces, and the auto_path extension
#  trick works only on 8.3.1 and higher.  See tcllib if
#  you want tricks to make this work with earlier versions.
if {![package vsatisfies [package provide Tcl] 8.4]} {return}

# Extend the auto_path to make sppdgTcllib packages available
if {[lsearch -exact $::auto_path $dir] == -1} {
    lappend ::auto_path $dir
}



