#----------------------------------------------*-TCL-*------------
#
#  mmtl_iterate.tcl
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: bem_iterate.tcl,v 1.6 2004/07/22 21:44:47 techenti Exp $
#
#----------------------------------------------*-TCL-*------------
package require Itcl
package require csdl

package provide bem 1.0


############################################################################
# Build the gui interface containing all items that the
# application can iterate.
#
# nodename: name of the cross-section file (without the extension)
# cSegs   : number of C segments to mesh
# pSegs   : number of P segments to mesh
# doGui   : 1 - being run from mmtl.tcl  0 - outside of mmtl.tcl
# parent  : widget id or 0
#
############################################################################
proc ::bem::bemRunIterateMMTL { nodename cSegs pSegs {doGui 0} {parent 0} \
	{wdgt 0} } {


    set ::bem::Csegs $cSegs
    set ::bem::Psegs $pSegs
    set ::bem::Gui $doGui
    set ::bem::Parent $parent
    

    if { $::bem::Gui } {
	# set the cursor bitmap to busy
	 #::gui::guiSetCursorBusy $parent

	# pop-up the window showing the program is busy
	#::gui::guiPopupBusy $wdgt "Running an iteration........"
    }

    #--------------------------------------------
    # Allow for iterating the width, height, conductivi
    # and pitch of a RectangleConductors object.
    #--------------------------------------------

    set ::bem::BuildCount 0

    #------------------------------------------------------------
    # unset any data from previous runs.
    #------------------------------------------------------------
    array unset ::bem::_arrStartList
    array unset ::bem::_arrEndImpList
    array unset ::bem::_arrEndOddImpList
    array unset ::bem::_arrEndEvenImpList
    array unset ::bem::_arrNumList
    array unset ::bem::_arrRectWidthList
    array unset ::bem::_arrCircDiamList
    array unset ::bem::_arrTrapWidthTopList
    array unset ::bem::_arrTrapWidthBotList


    #--------------------------------------------------------------
    # Build the iteration window.
    #--------------------------------------------------------------
    set ::bem::_iterateDialog [Dialog .iterate -title "BEM MMTL Iterations" \
				   -side bottom -anchor e -cancel 1]
    $::bem::_iterateDialog add -text "Ok"
    $::bem::_iterateDialog add -text "Cancel"

    set f [$::bem::_iterateDialog getframe]

    grid [label $f.title -text "Select Iteration Parameter"]

    #--------------------------------------------------------------
    # Build a check-box for each attribute the user may change.  The
    # values that may change are the dimensions on the conductors, but
    # the user must pick only one.
    #--------------------------------------------------------------
    set rbcount 0
    foreach itm [itcl::find objects -class RectangleConductors] {
	set ::bem::_arrRectWidthList($itm) 0
	incr rbcount
	set rb [radiobutton $f.rb$rbcount -text "$itm width" \
		    -variable ::bem::_iterateObject -value "$itm"]
	grid $rb -sticky w
    }

    foreach itm [itcl::find objects -class CircleConductors] {
	set ::bem::_arrCircDiamList($itm) 0
	incr rbcount
	set rb [radiobutton $f.rb$rbcount -text "$itm diameter" \
		    -variable ::bem::_iterateObject -value "$itm"]
	grid $rb -sticky w
    }

    foreach itm [itcl::find objects -class TrapezoidConductors] {
	set ::bem::_arrTrapWidthTopList($itm) 0
	incr rbcount
	set rb [radiobutton $f.rb$rbcount -text "$itm Top Width" \
		    -variable ::bem::_iterateObject -value "$itm"]
	grid $rb -sticky w
    }

    #--------------------------------------------------------------
    #  Display the dialog
    #--------------------------------------------------------------
    set result [$::bem::_iterateDialog draw]
    destroy $::bem::_iterateDialog
    if { $result == 0 } {

	#  decode the selected item into a flag that corresponds
	#  to the way that _doIterate wants to get the data.
	set obj $::bem::_iterateObject
	set class [$obj info class]
	switch -- $class {
	    ::RectangleConductors {set ::bem::_arrRectWidthList($obj)    1}
	    ::CircleConductors    {set ::bem::_arrCircDiamList($obj)     1}
	    ::TrapezoidConductors {set ::bem::_arrTrapWidthTopList($obj) 1}
	}

	::bem::_doIterate
    }

}

############################################################################
# _setArgToProcess
#    flg - 0  Process a rectangle.
#          1  Process a circle
#          2  Process a trapezoid
############################################################################
proc ::bem::_setArgToProcess { flg } {


    switch -- $flg {
	0 {
	    foreach itm [itcl::find objects -class CircleConductors] {
		set ::bem::_arrCircDiamList($itm) 0
	    }
	    foreach itm [itcl::find objects -class TrapezoidConductors] {
		set ::bem::_arrTrapWidthTopList($itm) 0
	    }
	}
	1 {
	    foreach itm [itcl::find objects -class RectangleConductors] {
		set ::bem::_arrRectWidthList($itm) 0
	    }
	    foreach itm [itcl::find objects -class TrapezoidConductors] {
		set ::bem::_arrTrapWidthTopList($itm) 0
	    }
	}
	2 {
	    foreach itm [itcl::find objects -class RectangleConductors] {
		set ::bem::_arrRectWidthList($itm) 0
	    }
	    foreach itm [itcl::find objects -class CircleConductors] {
		set ::bem::_arrCircDiamList($itm) 0
	    }
	    foreach itm [itcl::find objects -class TrapezoidConductors] {
		set ::bem::_arrTrapWidthBotList($itm) 0
	    }
	}
    }
    update
}



############################################################################
############################################################################
proc ::bem::_doIterate {} {

    set ::bem::_row 1
    set ::bem::_col 0

    set ::bem::_spFrame [toplevel .width \
	    -relief sunken -borderwidth 5]
    wm iconname $::bem::_spFrame "Iterate1"
    wm iconify $::bem::_spFrame


    #--------------------------------------------
    # Allow for iterating the width of a
    # RectangleConductors object.
    #--------------------------------------------

    foreach nme [itcl::find objects -class RectangleConductors] {
	if { $::bem::_arrRectWidthList($nme) } {
	    ::bem::_buildIterateEntryFrame $nme RW [$nme cget -width] \
		    "$nme Width"
	}
    }


    #--------------------------------------------
    # Allow for iterating the diameter of a
    # CircleConductors object.
    #--------------------------------------------

    foreach nme [itcl::find objects -class CircleConductors] {
	if { $::bem::_arrCircDiamList($nme) } {
	    ::bem::_buildIterateEntryFrame $nme CD \
		    [$nme cget -diameter] \
		    "$nme Diameter"
	}
    }


    #--------------------------------------------
    # Allow for iterating the top width
    # of a TrapezoidConductors object.
    #--------------------------------------------

    foreach nme [itcl::find objects -class TrapezoidConductors] {
	if { $::bem::_arrTrapWidthTopList($nme) } {
	    ::bem::_buildIterateEntryFrame $nme TWT \
		    [$nme cget -topWidth] \
		    "$nme TopWidth" [$nme cget -bottomWidth]
	}
    }

    set but1 [::gui::guiBuildButton $::bem::_spFrame "OK" \
	    "::bem::_finishIterate" "Proceed with the requested iterate"]
    set but2 [::gui::guiBuildButton $::bem::_spFrame "Cancel" \
	    "::bem::_cancelBldIterate" "Cancel the iterate"]
    grid $but1 $but2 -sticky nw

    wm deiconify $::bem::_spFrame
    grab $::bem::_spFrame
}

############################################################################
############################################################################
proc ::bem::_buildIterateEntryFrame { nme type value txtStrg {botwidth ""} } {

    set ::bem::_maxIterations 100
    set indx [format "%s-$type" $nme]
    set ::bem::_arrStartList($indx) $value
    set ::bem::_arrIncrList($indx) 0
    set ::bem::_arrEndImpList($indx) 0
    set ::bem::_arrEndOddImpList($indx) 0
    set ::bem::_arrEndEvenImpList($indx) 0

    ::bem::_buildIterateParameters $::bem::_spFrame $txtStrg \
	    ::bem::_arrStartList($indx) ::bem::_arrIncrList($indx) \
	    ::bem::_arrEndImpList($indx) ::bem::_arrEndOddImpList($indx) \
	    ::bem::_arrEndEvenImpList($indx) ::bem::_maxIterations $botwidth
}


############################################################################
############################################################################
proc ::bem::_cancelBldIterate {} {

    destroy $::bem::_spFrame
}

############################################################################
############################################################################
proc ::bem::_buildIterateParameters { parent title  startVar incrVar \
	imp1 imp2 imp3  maxIter botwidth } {

    incr ::bem::BuildCount
    set frme [frame $parent.frme$::bem::BuildCount]
    set seglbl [Label $frme.seglbl$::bem::BuildCount \
		-justify center -text $title]
    grid $seglbl -columnspan 2 -sticky w

    set startlab [label $frme.startlab -text "Starting Value"]
    set startent [entry $frme.startent -textvariable $startVar]
    grid $startlab $startent -sticky news -padx 2 -pady 2

    set inclab [label $frme.inclab -text "Increment"]
    set incent [entry $frme.incent -textvariable $incrVar]
    grid $inclab $incent -sticky news -padx 2 -pady 2

    #-------------------------------------------------------------------
    # Is the object being processed a trapezoid?
    #-------------------------------------------------------------------
    if { [string length $botwidth] > 0 } {

	set ::bem::_botWidthStart $botwidth
	set botstartlab [label $frme.botstartlab -text "Starting Bottom Width"]
	set botstartent [entry $frme.botstartent \
			     -textvariable ::bem::_botWidthStart]
	grid $botstartlab $botstartent -sticky news -padx 2 -pady 2

	set ::bem::_botWidthIncr 0
	set botinclab [label $frme.botinclab -text "Increment Bottom Width"]
	set botincent [entry $frme.botincent \
			   -textvariable ::bem::_botWidthIncr]
	grid $botinclab $botincent -sticky news -padx 2 -pady 2
    }

    set lblImp [label $frme.lblImp \
	    -text "Enter value for only one of 3 impedance fields"]
    grid $lblImp -columnspan 2 -sticky w


    set imp1lab [label $frme.imp1lab -text "Target Impedance"]
    set imp1ent [entry $frme.imp1ent -textvariable $imp1]
    grid $imp1lab $imp1ent -sticky news -padx 2 -pady 2

    set imp2lab [label $frme.imp2lab -text "Target Odd Impedance"]
    set imp2ent [entry $frme.imp2ent -textvariable $imp2]
    grid $imp2lab $imp2ent -sticky news -padx 2 -pady 2

    set imp3lab [label $frme.imp3lab -text "Target Even Impedance"]
    set imp3ent [entry $frme.imp3ent -textvariable $imp3]
    grid $imp3lab $imp3ent -sticky news -padx 2 -pady 2



    set numlab [label $frme.numlab -text "Maximum # of Iterations"]
    set nument [entry $frme.nument -textvariable $maxIter]
    grid $numlab $nument -sticky news -padx 2 -pady 2


    grid $frme -row $::bem::_row -column $::bem::_col -sticky new

    if { $::bem::_col == 0 } {
	set ::bem::_col 1
    } else {
	incr ::bem::_row
	set ::bem::_col 0
    }

}

############################################################################
############################################################################
proc ::bem::_buildIterateScript { fp nme value strt incrv impedanceType \
    endImpedance } {

    #--------------------------------------------------------
    # Call the length function in case a conversion is needed.
    #--------------------------------------------------------
    set vala [length $strt]
    set incra [length $incrv]

    if { $vala <= 0 } {
	::gui::guiPopupWarning \
		"The x dimension must be greater than 0."
	return 1
    }
    if { $incra == 0 } {
	::gui::guiPopupWarning \
		"The increment cannot be 0."
	return 1
    }

    if { $endImpedance  <= 0 } {
	::gui::guiPopupWarning \
		"The impedance cannot be 0."
	return 1
    }

    if { ([scan $::bem::_maxIterations {%d} numLoops] != 1) || \
	 ($numLoops < 1) } {
	::gui::guiPopupWarning \
		"The # of iterations must an integer > 0."
	return 1
    }


    if { [string compare $value "topWidth"] == 0 } {

	#--------------------------------------------------------
	# Call the length function in case a conversion is needed.
	#--------------------------------------------------------
	set valb [length $::bem::_botWidthStart]
	set incrb [length $::bem::_botWidthIncr]
	if { $valb <= 0 } {
	    ::gui::guiPopupWarning \
		    "The x dimension must be greater than 0."
	    return 1
	}
	if { $incrb == 0 } {
	    ::gui::guiPopupWarning \
		    "The increment cannot be 0."
	    return 1
	}
    }
    
    #---------------------------------------------------------
    # Number of nested loops being created in the processing 
    # script.
    #---------------------------------------------------------

    incr ::bem::_curls

    #---------------------------------------------------------
    # Create the for loop.
    #---------------------------------------------------------
    puts $fp "  set val$::bem::_curls $vala"
    if { [string compare $value "topWidth"] == 0 } {
	puts $fp "  set valbot $valb"
    }
    puts $fp "  set impType $impedanceType"
    puts $fp "  set closeTo $endImpedance"
    puts $fp "  set oldDiff  99999999"
    puts $fp "  set diff     88888888"
    puts $fp "  set oldImp   0"
    puts $fp "  set loopct 0"

    set strg [ format "  while \{ 1 \} \{"]

    puts $fp $strg

    puts $fp "    $nme configure -$value \$val$::bem::_curls "
    if { [string compare $value "topWidth"] == 0 } {
	puts $fp "    $nme configure -bottomWidth \$valbot"
    }
    puts $fp "    set cname $nme"
    puts $fp "    set cdimName $value"
    puts $fp "    set cdimValue \$val$::bem::_curls"


    puts $fp "    set val$::bem::_curls \[ expr \{ \$val$::bem::_curls + $incra \}\]"
    if { [string compare $value "topWidth"] == 0 } {
	puts $fp "    puts \"Conductor $nme:  botWidth of \$valbot\""
	puts $fp "    set valbot \[ expr \{ \$valbot + $incrb \}\]"
    }
    
    return 0
}


############################################################################
############################################################################
proc ::bem::_setWhichImpedance { indx } {

    set ::bem::_impedanceType 0
    set prefix " "
    set ::bem::_endImpedance $::bem::_arrEndImpList($indx)
    
    if { $::bem::_arrEndOddImpList($indx) > 0 } {
	set ::bem::_impedanceType 1
	set prefix " Odd"
	set ::bem::_endImpedance $::bem::_arrEndOddImpList($indx)
    }
    
    if { $::bem::_arrEndEvenImpList($indx) > 0 } {
	set ::bem::_impedanceType 2
	set prefix " Even"
	set ::bem::_endImpedance $::bem::_arrEndEvenImpList($indx)
    }
    return $prefix
}

############################################################################
############################################################################
proc ::bem::_finishIterate {} {

    set ::bem::_curls 0

    set flenme "$::gui::_nodename.iterate_script"

    set fp [open $flenme w]
    puts $fp "proc _runIterate \{\} \{"
    puts $fp "  global _nodename"
    puts $fp "  global _num_c_segs"
    puts $fp "  global _num_p_segs"
    puts $fp "  global oldDiff"
    puts $fp "  global oldImp"
    puts $fp ""
    puts $fp "  set resultsFle \[ open \"$::gui::_nodename.iterate_result\" w \] "
    puts $fp "  set logFle \[ open \"$::gui::_nodename.iterate_result_log\" w \] "
    puts $fp "  puts \"_____________________________________________\""
    


    #--------------------------------------------
    # Allow for iterating the width, height, conductivity
    # and pitch of a RectangleConductors object.
    #--------------------------------------------

    foreach nme [itcl::find objects -class RectangleConductors] {
	if { $::bem::_arrRectWidthList($nme) } {

	    set indx [ format "%s-RW" $nme ]
	    
	    set prefix [_setWhichImpedance $indx]

	    puts $fp "  # RectangleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    incr: $::bem::_arrIncrList($indx) \
		    $prefix Impedance close to: $::bem::_endImpedance"
	    if { [_buildIterateScript $fp $nme "width" $::bem::_arrStartList($indx) \
		    $::bem::_arrIncrList($indx) $::bem::_impedanceType $::bem::_endImpedance ] } {
		return
	    }
	}
    }


    #--------------------------------------------
    # Allow for iterating the diameter, conductivity
    # and pitch of a CircleConductors object.
    #--------------------------------------------

    foreach nme [itcl::find objects -class CircleConductors] {
	if { $::bem::_arrCircDiamList($nme) } {

	    set indx [ format "%s-CD" $nme ]

	    set prefix [_setWhichImpedance $indx]

	    puts $fp "  # CircleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    incr: $::bem::_arrIncrList($indx) \
		    Impedance close to: $::bem::_endImpedance"
	    if { [_buildIterateScript $fp $nme "diameter" \
		    $::bem::_arrStartList($indx) \
		    $::bem::_arrIncrList($indx) $::bem::_impedanceType $::bem::_endImpedance ] } {
		return
	    }
	}
    }


    #--------------------------------------------
    # Allow for iterating the top and bottom width, height, 
    # conductivity and pitch of a TrapezoidConductors object.
    #--------------------------------------------

    foreach nme [itcl::find objects -class TrapezoidConductors] {
	if { $::bem::_arrTrapWidthTopList($nme) } {

	    set indx [ format "%s-TWT" $nme ]

	    set prefix [_setWhichImpedance $indx]

	    puts $fp "  # TrapezoidConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    incr: $::bem::_arrIncrList($indx) \
		    Impedance close to: $::bem::_endImpedance"
	    if { [_buildIterateScript $fp $nme "topWidth" \
		    $::bem::_arrStartList($indx) \
		    $::bem::_arrIncrList($indx) $::bem::_impedanceType $::bem::_endImpedance ] } {
		return
	    }
	}
    }

    ::bem::_finishIterateFile $fp

    close $fp


    ::bem::_runTheSimulations iterate_script _runIterate \
	    "The generated script will run the\
	    simulation until the Impedance is closest to $::bem::_endImpedance." 

}


proc ::bem::_runTheSimulations { type command message } {

    destroy $::bem::_spFrame

    set retrn [tk_dialog .tkd "Warning" \
	$message warning {} "Run Simulation" "Cancel"]

    if { $retrn == 0 } {
	#--------------------------------------------------
	# Source and run the iterate.
	#--------------------------------------------------

	if { $::bem::Gui } {
	    console::show
	}

	#--------------------------------------------------
	# Save a copy of the current display -- as a temp file.
	#--------------------------------------------------
	set flename "$::gui::_nodename.xsctn.temp"
	puts "Write: \"$flename\""
	::csdl::csdlWriteTCL $flename $::bem::Title $::bem::Csegs $::bem::Psegs

	#--------------------------------------------------
	# Run the iterations.
	#--------------------------------------------------
	::csdl::csdlReadTCL $::gui::_nodename.$type


	# make the feedback window the top window on the screen.
	#::gui::guiRaiseConsole
	$command

	if { $::bem::Gui } {
	    # set the display data to the user's choice
	    ::gui::_setDisplayData
	}
    }

}

##########################################################
# Output the rectangle conductor attributes.
##########################################################
proc ::bem::bemPutRectangleConductorsToFile { nme fp } {

    set rname         $nme
    set rheight       [$nme cget -height]
    set rwidth        [$nme cget -width]
    set rconductivity [$nme cget -conductivity]
    set rnumber       [$nme cget -number]
    set rpitch        [$nme cget -pitch]
    set rxoffset      [$nme cget -xOffset]
    set ryoffset      [$nme cget -yOffset]
    puts $fp "$nme: RectangleConductors"
    puts $fp " height      : $rheight"
    puts $fp " width       : $rwidth"
    puts $fp " conductivity: $rconductivity"
    puts $fp " number      : $rnumber"
    puts $fp " pitch       : $rpitch"
    puts $fp " x-offset    : $rxoffset"
    puts $fp " y-offset    : $ryoffset"
}



##########################################################
# Output the circle conductor attributes.
##########################################################
proc ::bem::bemPutCircleConductorsToFile { nme fp } {

    set cname         $nme
    set cdiameter     [$nme cget -diameter]
    set cconductivity [$nme cget -conductivity]
    set cnumber       [$nme cget -number]
    set cpitch        [$nme cget -pitch]
    set cxoffset      [$nme cget -xOffset]
    set cyoffset      [$nme cget -yOffset]
    puts $fp "$nme: CircleConductors"
    puts $fp " diameter    : $cdiameter"
    puts $fp " conductivity: $cconductivity"
    puts $fp " number      : $cnumber"
    puts $fp " pitch       : $cpitch"
    puts $fp " x-offset    : $cxoffset"
    puts $fp " y-offset    : $cyoffset"
}

##########################################################
# Output the trapezoid conductor attributes.
##########################################################
proc ::bem::bemPutTrapezoidConductorsToFile { nme fp } {

    set tname         $nme
    set theight       [$nme cget -height]
    set ttopwidth     [$nme cget -topWidth]
    set tbotwidth     [$nme cget -bottomWidth]
    set tconductivity [$nme cget -conductivity]
    set tnumber       [$nme cget -number]
    set tpitch        [$nme cget -pitch]
    set txoffset      [$nme cget -xOffset]
    set tyoffset      [$nme cget -yOffset]

    puts $fp "$nme: TrapezoidConductors"
    puts $fp " height      : $theight"
    puts $fp " topWidth    : $ttopwidth"
    puts $fp " botWidth    : $tbotwidth"
    puts $fp " conductivity: $tconductivity"
    puts $fp " number      : $tnumber"
    puts $fp " pitch       : $tpitch"
    puts $fp " x-offset    : $txoffset"
    puts $fp " y-offset    : $tyoffset"
}


##########################################################
# Output the dielectric attributes.
##########################################################
proc ::bem::bemPutDielectricLayerToFile { nme fp } {

    set dname         $nme
    set thickness     [$nme cget -thickness]
    set permittivity  [$nme cget -permittivity]
    set permeability  [$nme cget -permeability]
    set lossTangent   [$nme cget -lossTangent]
    puts $fp "$nme: DielectricLayer"
    puts $fp " thickness   : $thickness"
    puts $fp " permittivity: $permittivity"
    puts $fp " permeability: $permeability"
    puts $fp " lossTangent : $lossTangent"
}

##########################################################
# Output the ground plane attributes.
##########################################################
proc ::bem::bemPutGroundPlaneToFile { nme fp } {

    set gname         $nme
    puts $fp "$nme: GroundPlane"
}

############################################################################
############################################################################
proc ::bem::_finishIterateFile { fp } {
    global env

    puts $fp "    puts \$resultsFle \"\""
    puts $fp "    puts \$resultsFle \"________________________\
	    _______________________________\""
    puts $fp "    puts \$logFle \"____________________________\
	    ___________________________\""
    puts $fp "    puts \$resultsFle \"\""
    puts $fp "    puts \$logFle \"\""
    puts $fp "    foreach itm \$Stackup\:\:structureList \{"
    puts $fp "      set lgt \[string length \$itm]"
    puts $fp "      set nme \[string range \$itm 2 \$lgt]"
    puts $fp "      if \{ \[\$nme isa GroundPlane\] \} \{"
    puts $fp "        ::bem::bemPutGroundPlaneToFile \$nme \$resultsFle"
    puts $fp "        ::bem::bemPutGroundPlaneToFile \$nme \$logFle"
    puts $fp "      \}"
    puts $fp "      if \{ \[\$nme isa DielectricLayer\] \} \{"
    puts $fp "        ::bem::bemPutDielectricLayerToFile \$nme \$resultsFle"
    puts $fp "        ::bem::bemPutDielectricLayerToFile \$nme \$logFle"
    puts $fp "      \}"
    puts $fp "      if \{ \[\$nme isa RectangleConductors\] \} \{"
    puts $fp "        ::bem::bemPutRectangleConductorsToFile \$nme \$resultsFle"
    puts $fp "        ::bem::bemPutRectangleConductorsToFile \$nme \$logFle"
    puts $fp "      \}"
    puts $fp "      if \{ \[\$nme isa CircleConductors\] \} \{"
    puts $fp "        ::bem::bemPutCircleConductorsToFile \$nme \$resultsFle"
    puts $fp "        ::bem::bemPutCircleConductorsToFile \$nme \$logFle"
    puts $fp "      \}"
    puts $fp "      if \{ \[\$nme isa TrapezoidConductors\] \} \{"
    puts $fp "        ::bem::bemPutTrapezoidConductorsToFile \$nme \$resultsFle"
    puts $fp "        ::bem::bemPutTrapezoidConductorsToFile \$nme \$logFle"
    puts $fp "      \}"
    puts $fp "    \}"


    puts $fp "    ::gui::_replaceNamedValues"
    set workingNode [format {%sTMP} $::gui::_nodename]
    puts $fp "    ::csdl::csdlWriteTCL \"$workingNode.xsctn\" \"$::gui::_title\" \
	    $::bem::Csegs $::bem::Psegs"
    set cmdName [auto_execok bem]
    puts $fp "    set cmmnd {$cmdName \
	    \"$workingNode\" \$::bem::Csegs \$::bem::Psegs } "
    set strg [ format "    set msg \[catch \{  eval exec \$cmmnd \} \
	    result]" ]
    puts $fp $strg
    puts $fp "    set fin \[open \"$workingNode.result\" r\]"
   
    puts $fp "    set results \[read \$fin\]"
    puts $fp "    puts \$resultsFle \$results"
    puts $fp "    close \$fin"
    puts $fp "    set done \[::bem::bemParseIterationResults \$results \
	    \$cname \$cdimName \$cdimValue \$impType \$closeTo\]"
    puts $fp "    if \{ \$done > 0 \} \{"
    puts $fp "      close \$logFle"
    puts $fp "      close \$resultsFle"
    puts $fp "      return"
    puts $fp "    \}"

    puts $fp "    puts \$resultsFle \"\""
    puts $fp "    puts \$logFle \"\""
     puts $fp "    puts \"__________________________________\
	     __________________\""
    puts $fp "    puts \$logFle \$result"
    puts $fp "    incr loopct"

    #------------------------------------------------------------
    # # of iterations
    #------------------------------------------------------------

    puts $fp "    if \{ \$loopct > $::bem::_maxIterations \} \{"
    puts $fp "      close \$logFle"
    puts $fp "      close \$resultsFle"
    puts $fp "      return"
    puts $fp "    \}"

    #------------------------------------------------------------
    # # of nested loops
    #------------------------------------------------------------
    global ::bem::_curls

    for { set i 0 } { $i < $::bem::_curls } { incr i } {
	set strg "  \} "
	puts $fp $strg
    }
    puts $fp "  close \$logFle"
    puts $fp "  close \$resultsFle"
    set strg " \}"
    puts $fp $strg
}


############################################################################
############################################################################
proc ::bem::bemParseIterationResults { results cName cdimName cdimValue \
	impType closeTo } {

    set indx [string first "Asymmetry Ratios:" $results]
    set newstrg2 [string range $results $indx [string length $results] ]

    set indx1 [string first "Asymmetry ratio for" $newstrg2 ]
    set newstrg [string range $newstrg2 [expr { $indx1 + 42 }] \
	    [string length $newstrg2]]

    set indx2 [string first "*******" $newstrg2]
    set tmp [string range $newstrg2 $indx2 [expr { $indx2 + 150 }] ]

    set indx [string first "%" $newstrg]
    set maxx [ string range $newstrg 0 $indx ]

    set newstrg2 [string range $newstrg [expr {$indx + 7 }] \
	    [expr { $indx + 100 }] ]
    set indx [string first "%" $newstrg2]
    set average [string range $newstrg2 0 $indx ]

    if { $impType == 0 } {
	set indx [string first Impedance $results]
	set newstrg [string range $results $indx  [string length $results]]
	set indx [string first $cName $newstrg]
	set newstrg [string range $newstrg $indx [string length $newstrg]]
	set indx [string first "=" $newstrg]
	set newstrg [string range $newstrg $indx  [string length $newstrg]]
	set num [ scan $newstrg {= %g} impedance ]
	if { $num < 1 } {
	puts "Cannot continue because the impedance value for $cName\
		is set to \"nan\""
	    return 1
	}
	puts "$cName:    $cdimName = $cdimValue     Impedance = $impedance"
    }

    #----------------------------------------------------------------
    # Are we processing the old impedance values?
    #----------------------------------------------------------------
    if { $impType == 1 } {
	set indx [string first odd= $results]
	if { $indx < 0 } {
	    puts stderr "**************************************************"
	    puts stderr "Cannot continue because the odd impedance value\
		    for $cName is not in the result file"
	    puts stderr "**************************************************"
	    return 1
	}
	set newstrg [string range $results [expr { $indx + 4 }] \
		[string length $results]]
	set num [ scan $newstrg {%g} impedance ]
	if { $num < 1 } {
	    puts stderr "**************************************************"
	    puts stderr "Cannot continue because the odd impedance value\
		    for $cName is set to \"nan\""
	    puts stderr "**************************************************"
	    return 1
	}
	puts "$cName:    $cdimName = $cdimValue     Odd Impedance = $impedance"
    }

    #----------------------------------------------------------------
    # Are we processing the even impedance values?
    #----------------------------------------------------------------
    if { $impType == 2 } {
	set indx [string first even= $results]
	if { $indx < 0 } {
	    puts stderr "**************************************************"
	    puts stderr "Cannot continue because the even impedance value\
		    for $cName is not in the result file"
	    puts stderr "**************************************************"
	    return 1
	}
	set newstrg [string range $results [expr { $indx + 5 }] \
		[string length $results]]
	set num [ scan $newstrg {%g} impedance ]
	if { $num < 1 } {
	    puts stderr "**************************************************"
	    puts stderr "Cannot continue because the even impedance value\
		    for $cName is set to \"nan\""
	    puts stderr "**************************************************"
	    return 1
	}
	puts "$cName:    $cdimName = $cdimValue     \
		Even Impedance = $impedance"
    }

    if { $indx1 > $indx2 } {
	puts "** Asymmetry ratio for inductance matrix:" 
	puts "$maxx (max),  $average (average) **"
    }
    set diff [expr { $closeTo - $impedance }]
    if { $diff < 0 } {
	set diff [expr { $diff * -1 }]
    }

    global oldDiff
    global oldImp

    if { $diff > $oldDiff } {
	puts "Impedance of $oldImp closest to $closeTo"
	return 1
    } else {
	set oldImp $impedance
	set oldDiff $diff
    }
    return -1
}

















