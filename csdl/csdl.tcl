#----------------------------------------------*-TCL-*------------
#
#  csdl.tcl
#
#  Cross Section Description Language
#
#  This module defines the csdl namespace and procedures
#  for reading and writing csdl descriptions to and
#  from files.
#
#  Copyright 2001-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: csdl.tcl,v 1.11 2004/07/19 18:22:01 techenti Exp $
#
#-----------------------------------------------------------------
package require Itcl

package provide csdl 1.0.1


namespace eval ::csdl:: {

    namespace export csdlReadTCL
    namespace export csdlWriteTCL

}




##########################################################
# Read the <node>.xsctn file.
##########################################################
proc ::csdl::csdlReadTCL { filename } {

    namespace eval :: [list source $filename]

    #  As of today, BEM MMTL doesn't convert coupling length into
    #  default units correctly, so we saved coupling length in
    #  meters, and must now convert it into default units.
    set ::Stackup::couplingLength [::units::convert "$::Stackup::couplingLength meters" $::Stackup::defaultLengthUnits]

}

##########################################################
# Write the <node>.xsctn file.
##########################################################
proc ::csdl::csdlWriteTCL { filename title cSegs pSegs } {

    if { [file exists $filename] && ([string first temp $filename] < 1) } {
	# Make a copy of this file.
	# puts "Move this file to $filename.backup"
	set bckup "$filename.backup"
	file copy -force $filename $bckup
    }

    #--------------------------------------------------
    # Open the file for write.
    #--------------------------------------------------
    set fp [open $filename w]

    puts $fp "#----------------------------------"
    puts $fp "# File:  $filename"
    puts $fp "# [clock format [clock seconds]]"
    puts $fp "#----------------------------------"
    puts $fp ""
    puts $fp "package require csdl"
    puts $fp ""
    puts $fp "set _title \"$title\""

    #  As of today, BEM MMTL doesn't convert coupling length into
    #  default units correctly, so convert coupling length to default
    #  units, then convert it into meters.
    set lenDef [length $::Stackup::couplingLength]
    set cLength [::units::convert "$lenDef $::Stackup::defaultLengthUnits" "meters"]
    puts $fp "set ::Stackup::couplingLength \"$cLength\""
    puts $fp "set ::Stackup::riseTime \"$::Stackup::riseTime\""
    puts $fp "set ::Stackup::frequency \"$::Stackup::frequency\""
    puts $fp "set ::Stackup::defaultLengthUnits\
	    \"$::Stackup::defaultLengthUnits\""
    puts $fp "set CSEG $cSegs"
    puts $fp "set DSEG $pSegs"
    puts $fp ""

    foreach item $::Stackup::structureList {
	set nme [string range $item 2 \
		[expr {[string length $item] - 1} ]]
	#-------------------------------------------------
	# Find out the component-type of $nme.
	#-------------------------------------------------
	if { [$nme isa GroundPlane] } {
	    set type 0
	    set lst "GroundPlane $nme "
	}
	if { [$nme isa DielectricLayer] } {
	    set type 1
	    set lst "DielectricLayer $nme "
	}
	if { [$nme isa RectangleDielectric] } {
	    set type 1
	    set lst "RectangleDielectric $nme "
	}
	if { [$nme isa RectangleConductors] } {
	    set type 2
	    set lst "RectangleConductors $nme "
	}
	if { [$nme isa CircleConductors] } {
	    set type 2
	    set lst "CircleConductors $nme "
	}
	if { [$nme isa TrapezoidConductors] } {
	    set type 2
	    set lst "TrapezoidConductors $nme "
	}

	#-------------------------------------------------
	# Loop through the attributes of this item. 
	#-------------------------------------------------
	foreach def [$item configure] {
	    set flg [lindex $def 0]
	    set vle [lindex $def 2]
	    append lst " \\\n\t $flg $vle"
	}
	puts $fp $lst
    }
    close $fp
}

