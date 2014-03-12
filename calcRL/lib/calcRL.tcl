#----------------------------------------------*-TCL-*------------
#
#  calcRL.tcl
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: calcRL.tcl,v 1.8 2004/07/29 13:03:14 techenti Exp $
#
#-----------------------------------------------------------------
package require Itcl

package provide calcRL 1.0


namespace eval ::calcRL:: {
    namespace export runBatch
    namespace export genInputFiles
}

##########################################################
# Write the input file for the program the calculates 
# resistance/inductance.
##########################################################
proc ::calcRL::genInputFile { nodename frequency } {

    set structureList $Stackup::structureList
    set totWidth  0.0
    set totHeight 0.0
    set nRectConds  0
    set nCircConds  0
    set nTrapConds  0
    set condCount 1
    set condList ""
    set circList ""
    set trapList ""
    set conductivity 5e7
    foreach struct $structureList {
        #--------------------------------------------------------------
        # Add the heights of the ground-planes and dielectrics.
        #--------------------------------------------------------------
        if {[$struct isa GroundPlane] || [$struct isa DielectricLayer]} {
	    set totHeight [expr {$totHeight + [$struct height]}]
	    continue
        }

        #--------------------------------------------------------------
        # Get the largest conductor width and the shortest conductor
        # height.
        #--------------------------------------------------------------
	set width [$struct width]
	if { $width > $totWidth } {
	    set totWidth $width
	}
	if { [$struct isa RectangleConductors] } {
	    set conductivity [$struct cget -conductivity]
	    set nRectConds [expr { $nRectConds + [$struct cget -number] }]
	}
	if { [$struct isa CircleConductors] } {
	    set conductivity [$struct cget -conductivity]
	    set nCircConds [expr { $nCircConds + [$struct cget -number] }]
	}
	if { [$struct isa TrapezoidConductors] } {
	    set conductivity [$struct cget -conductivity]
	    set nTrapConds [expr { $nTrapConds + [$struct cget -number] }]
	}
    }
    set expWidth [expr { $totWidth * 3 }]
    puts "Width: $totWidth ($expWidth)   Height: $totHeight"

    set units $::Stackup::defaultLengthUnits
    puts "units: $units"
    #--------------------------------------------------------------
    #  Convert lengths to SI units
    #--------------------------------------------------------------
    set scaleFactors(meters) 1
    set scaleFactors(microns) 1e-6
    set scaleFactors(inches) 0.0254
    set scaleFactors(mils) 0.0254e-3
    set scaleFactors(UNDEFINED) UNDEFINED
    puts " $scaleFactors($units)"
    set scale $scaleFactors($units)
    puts "scale $scale"

    set maxXcoord 700.00
    set maxYcoord 400.00

    set rlOut [open $nodename.ri.dat w]
    set inOut [open $nodename.ri.in w]
    
    puts $rlOut "[expr { $expWidth * $scale }]"
    puts $rlOut "[expr { $totHeight * $scale }]"
    puts $rlOut "[expr { $maxXcoord / ($expWidth * $scale) }]"
    puts $rlOut "[expr { $maxYcoord / ($expWidth * $scale) }]"
##    puts $rlOut "[expr { $totWidth / $scale }]"
##    puts $rlOut "[expr { $totWidth / $scale }]"
    puts $rlOut "-[expr { $totWidth * 0.5 * $scale }]"
    puts $rlOut "0"
    puts $rlOut "1024"
    puts $rlOut "40"
    puts $rlOut "6"
    puts $rlOut "8"
    puts $rlOut "10"
    puts $rlOut "5"
    puts $rlOut "0.0"
    puts $rlOut "1.0e5"
    puts $rlOut "1"
    puts $rlOut "1.0e6"
    puts $rlOut "$conductivity"
    puts $rlOut "0 $nRectConds $nCircConds 0 0 $nTrapConds 1"
    puts $rlOut ""

    puts $inOut "1"
    puts $inOut "$nRectConds"
    puts $inOut "$nCircConds"
    puts $inOut "$nTrapConds"
    #
    #  Ground plane
    #
    puts $inOut "0"
    puts $inOut "0"
    puts $inOut "[expr { $expWidth * $scale }]"
    puts $inOut "[expr { 10 * $scale }]"
    puts $inOut "0"
    
    set yat 0.0
    set numConds  0
    set condCount 1
    set condList ""
    set circList ""
    set trapList ""
    foreach struct $structureList {
        #--------------------------------------------------------------
        # Ground Planes and Dielectrics
        #--------------------------------------------------------------
        if {[$struct isa GroundPlane] || [$struct isa DielectricLayer]} {
	    set yat [expr {$yat + [$struct height]}]
	    continue
        }
	if {[$struct isa RectangleDielectric]} {
	    continue
	}

	set numConds [expr { $numConds + [$struct cget -number] }]    
	set xat [expr { ( $totWidth + \
		[$struct cget -xOffset]) * $scale }]
	set pitch [expr { [$struct cget -pitch] * $scale }]
	for { set ix 0 } { $ix < [$struct cget -number]} { incr ix } {
	    
	    if { [$struct isa RectangleConductors] } {
		set width [expr { [$struct cget -width] * $scale }]
		set height [expr { [$struct cget -height] * $scale }]
		set yloc [expr { $yat * $scale }]
		append condList "REC [expr { $condCount + 1 }]\
			recTags$condCount $condCount $xat \
			$yloc $width \
			$height 0 "
		#
		# rectangular conductor
		#
		puts $inOut "$xat"
		puts $inOut "$yloc"
		puts $inOut "$width"
		puts $inOut "$height"
		puts $inOut "0"
	    }
	    if { [$struct isa CircleConductors] } {
		set radius [expr { [$struct cget -diameter] * $scale * 0.5 }]
		set yloc [expr { $yat * $scale }]
		append circList "CIR [expr { $condCount + 1 }]\
			recTags$condCount $condCount $xat \
			$yloc $radius 0 "
		#
		# rectangular conductor
		#
		puts $inOut "$xat"
		puts $inOut "$yloc"
		puts $inOut "$radius"
		puts $inOut "0"
	    }
	    if { [$struct isa TrapezoidConductors] } {
		set topwidth [expr { [$struct cget -topWidth] * $scale }]
		set botwidth [expr { [$struct cget -bottomWidth] * $scale }]
		set height [expr { [$struct cget -height] * $scale }]
		set ybot [expr { $yat * $scale }]
		set ytop [expr { $ybot + $height }]
		set xatT [expr { $xat - (($topwidth - $botwidth) * 0.5) }]
		set xrgt [expr { $xat + $botwidth }]
		set xrgtT [expr { $xatT + $topwidth }]
		append trapList "POLY [expr { $condCount + 1 }]\
			recTags$condCount $condCount 4 $xat $ybot $xat\
			$ytop $xrgtT $ytop $xrgt $ybot 0 "
		#
		# rectangular conductor
		#
		puts $inOut "$xat"
		puts $inOut "$ybot"
		puts $inOut "$xat"
		puts $inOut "$ytop"
		puts $inOut "$xrgtT"
		puts $inOut "$ytop"
		puts $inOut "$xrgt"
		puts $inOut "$ybot"
		puts $inOut "0"
	    }
	    set xat [expr { $xat + $pitch }]
	}
    }

    puts $rlOut "$condList"
    puts $rlOut "$circList"
    puts $rlOut ""
    puts $rlOut ""
    puts $rlOut "$trapList"
    puts $rlOut "GND 1 gndTags1 1 0 0 [expr { $expWidth * $scale }]\
	    [expr { 10 * $scale }] 0"
    close $rlOut

    puts $inOut "1024"
    puts $inOut "40"
    puts $inOut "6"
    puts $inOut "8"
    puts $inOut "10"
    puts $inOut "5"
    puts $inOut "0.0"
    puts $inOut "1.0e5"
    puts $inOut "1"
    puts $inOut "$frequency"
    puts $inOut "$conductivity"
    close $inOut
}

proc ::calcRL::calcRL_RunGraphical { nodename } {
    global env

    puts "Running the graphical version of calcRL!"

    set cmdName [file join $env(CALCRL_LIBRARY) calcRL_exe.tcl]
    set cmmnd {$cmdName $nodename.ri.dat}
    if { [catch {  eval exec $cmmnd} result] } {
	puts "Finished with calcRL_exe.tcl"
    }
    puts $result

    puts "Display the data using xmgr!"
    set cmdName [file join $env(CALCRL_LIBRARY) xmgrPlotParameters]
    set cmmnd {$cmdName $nodename.ri.out}
    if { [catch {  eval exec $cmmnd} result] } {
	puts "xmgr display does not work!"
    }
    puts $result
}

proc ::calcRL::runBatch { nodename } {

    set cmdName [auto_execok calcRL]
    set ret [catch {exec $cmdName $nodename.ri} result]

    return $ret
}

