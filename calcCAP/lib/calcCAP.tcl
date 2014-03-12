#----------------------------------------------*-TCL-*------------
#
#  calcRL.tcl
#
#  Copyright (C) Mayo Foundation.  All Rights Reserved.
#
#-----------------------------------------------------------------
package require Itcl

package provide calcCAP 1.0


namespace eval ::calcCAP:: {

    namespace export calcCAP_RunGraphical
    namespace export calcCAP_RunBatch
    namespace export calcCAP_GenInputFiles

}
 

##########################################################
# Write the input file for the program the calculates 
# resistance/inductance.
##########################################################
proc ::calcCAP::genInputFile { nodename } {

    set structureList $Stackup::structureList
    set totWidth  0.0
    set totHeight 0.0
    set nRectConds  0
    set nCircConds  0
    set nTrapConds  0
    set numDiels  0
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
	    if { [$struct height] == 0.0 } {
		error "Invalid zero height for $struct"
	    }
	    set totHeight [expr {$totHeight + [$struct height]}]
	    if { [$struct isa DielectricLayer] } {
		incr numDiels
	    }
	    continue
        }
        if {[$struct isa RectangleDielectric]} {
	    incr numDiels
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
    #	array set scaleFactors "meters 1 microns 1e-6 inches 0.0254 mils 0.0254e-3 UNDEFINED UNDEFINED"
    set scaleFactors(meters) 1
    set scaleFactors(microns) 1e-6
    set scaleFactors(inches) 0.0254
    set scaleFactors(mils) 0.0254e-3
    set scaleFactors(UNDEFINED) UNDEFINED
    puts " $scaleFactors($units)"
    set scale $scaleFactors($units)
    puts "scale $scale"

    set capOut [open $nodename.cap.dat w]
    set inOut [open $nodename.cap.in w]

    set maxXcoord 700.00
    set maxYcoord 400.00

    #--------------------------------------------------------------
    # Total width
    # Total height
    #--------------------------------------------------------------
    puts $capOut "[expr { $expWidth * $scale }]"
    puts $capOut "[expr { $totHeight * $scale }]"
    #--------------------------------------------------------------
    # X scale factor
    # Y scale factor
    #--------------------------------------------------------------
    puts $capOut "[expr { $maxXcoord / ($expWidth * $scale) }]"
    puts $capOut "[expr { $maxYcoord / ($expWidth * $scale) }]"
    #--------------------------------------------------------------
    # x-offset
    # y-offset
    #--------------------------------------------------------------
    puts $capOut "-[expr { $totWidth * 0.5 * $scale }]"
    puts $capOut "0"

    #--------------------------------------------------------------
    # Nh   :    1024        Wavelet point number
    # Nit  :      40        Number of iterations
    # J    :       6        Resolution level
    # Nwx  :       8        Gauss quadrature x
    # Nwy  :      10        Gauss quadrature y
    # Nws  :       5        Number of segments/wavelet
    # Np   :     0.0        OPQ seperation
    # Eps  :   1.0e5        STAB accuracy
    # matr :       1        Matrix solution
    # od   :   1.0e6        Order of approximation
    # maxga:       0        Maximum approximation range
    #--------------------------------------------------------------
    puts $capOut "1024"
    puts $capOut "40"
    puts $capOut "6"
    puts $capOut "8"
    puts $capOut "10"
    puts $capOut "5"
    puts $capOut "0.0"
    puts $capOut "1.0e5"
    puts $capOut "2"
    puts $capOut "5"
    puts $capOut "$conductivity"
    puts $capOut "$numDiels $nRectConds $nCircConds 0 0 $nTrapConds 1"

    puts $inOut "1"
    puts $inOut "$nRectConds"
    puts $inOut "$nCircConds"
    puts $inOut "$nTrapConds"
    #
    #  Ground plane
    #
    puts $inOut "$numDiels"
    puts $inOut "0"
    puts $inOut "0"
    puts $inOut "[expr { $expWidth * $scale }]"
    puts $inOut "[expr { 10 * $scale }]"

    set yat 0.0
    set numConds  0
    set totCount  2
    set condCount 1
    set dielCount 1
    set dielList ""
    set trapList ""
    set dList ""
    set condList ""
    set circList ""
    set trapList ""
    foreach struct $structureList {
        #--------------------------------------------------------------
        # Ground Planes and Dielectrics
        #--------------------------------------------------------------
        if {[$struct isa GroundPlane]} {
	    set yat [expr { $yat + [$struct height] }]
	    continue
        }
	if { [$struct isa DielectricLayer] } {
	    set thck [expr { [$struct cget -thickness] * $scale }]
	    set perm [$struct cget -permittivity]
	    set xat 0
	    set width [expr {$expWidth * $scale }]
	    set yloc [expr { $yat * $scale }]
	    puts "yat: $yat  yloc: $yloc   totWidth: $totWidth"
	    append dielList "DIELE $totCount\
			recTags$dielCount $dielCount $xat \
			$yloc $width \
			$thck $perm "
	    append dList "$xat\n"
	    append dList "$yloc\n"
	    append dList "$width\n"
	    append dList "$thck\n"
	    append dList "$perm\n"
	    incr totCount
	    #incr dielCount
	    set yat [expr { $yat + [$struct height] }]
	    continue
        }
	if { [$struct isa RectangleDielectric] } {
	    set thck [expr { [$struct cget -height] * $scale }]
	    set perm [$struct cget -permittivity]
	    set xat [expr { ( $totWidth + \
		[$struct cget -xOffset]) * $scale }]
	    set width [expr {[$struct cget -width] * $scale}]
	    set yloc [expr { $yat * $scale }]
	    puts "yat: $yat  yloc: $yloc   totWidth: $totWidth"
	    append dielList "DIELE $totCount\
			recTags$dielCount $dielCount $xat \
			$yloc $width \
			$thck $perm "
	    append dList "$xat\n"
	    append dList "$yloc\n"
	    append dList "$width\n"
	    append dList "$thck\n"
	    append dList "$perm\n"
	    incr totCount
	    #incr dielCount
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
		append condList "REC $totCount\
			recTags$condCount $condCount $xat \
			$yloc $width \
			$height "
		incr totCount
		#
		# rectangular conductor
		#
		puts $inOut "$xat"
		puts $inOut "$yloc"
		puts $inOut "$width"
		puts $inOut "$height"
	    }
	    if { [$struct isa CircleConductors] } {
		set radius [expr { [$struct cget -diameter] * $scale * 0.5 }]
		set yloc [expr { $yat * $scale }]
		append circList "CIR [expr { $condCount + 1 }]\
			recTags$condCount $condCount $xat \
			$yloc $radius "
		#
		# rectangular conductor
		#
		puts $inOut "$xat"
		puts $inOut "$yloc"
		puts $inOut "$radius"
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
		append trapList "POLY $totCount\
			recTags$condCount $condCount 4 $xat $ybot $xat\
			$ytop $xrgtT $ytop $xrgt $ybot 0 "
		incr totCount
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
	    }
	    set xat [expr { $xat + $pitch }]
	}
    }
    puts $capOut "$dielList"
    puts $capOut "$condList"
    puts $capOut "$circList"
    puts $capOut ""
    puts $capOut ""
    puts $capOut "$trapList"
    puts $capOut "GND 1 gndTags1 1 0 0 [expr { $expWidth * $scale }]\
	    [expr { 10 * $scale }] 0"
    close $capOut

    set cmnd [format {%s1024} $dList]
    puts $inOut "$cmnd"
    puts $inOut "40"
    puts $inOut "6"
    puts $inOut "8"
    puts $inOut "10"
    puts $inOut "5"
    puts $inOut "0.0"
    puts $inOut "1.0e5"
    puts $inOut "1"
    puts $inOut "1.0e6"
    puts $inOut "$conductivity"
    close $inOut
}

proc ::calcCAP::calcCAP_RunGraphical { nodename } {
    global env

    set cmdName [file join $env(CALCCAP_LIBRARY) calcCAP_exe.tcl]
    set cmmnd {$cmdName $nodename.cap }
    puts "-$cmmnd-"
    if { [catch {  eval exec $cmmnd} result] } {
	puts "Finished with clcrl.tcl"
    }
    puts $result
}

proc ::calcCAP::calcCAP_RunBatch { nodename } {
    global env

    set cmdName [auto_execok calcCAP]
    set ret [catch {exec $cmdName $nodename.cap} result]

    return $ret

}
