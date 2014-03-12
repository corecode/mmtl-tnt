#----------------------------------------------*-TCL-*------------
#
#  mmtl_sweep.tcl
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: bem_sweep.tcl,v 1.7 2004/07/28 15:40:53 techenti Exp $
#
#----------------------------------------------*-TCL-*------------

package provide bem 1.0

#--------------------------------------------------------
#
#  ::bem::runSweep
#
#  Run multiple MMTL simulations, while sweeping
#  one or more parameters.
#
#  This procedure creates the dialog of checkbuttons
#  and entries which control building of the
#  simulation sweep script.  The script is
#  written based on a number of array entries
#  in ::bem::
#
#--------------------------------------------------------

proc ::bem::runSweep { nodename cSegs pSegs } {

    #--------------------------------------------------------
    #  Create dialog, from scratch
    #--------------------------------------------------------
    set dlg .sweepBEM
    if { [winfo exists $dlg] } {destroy $dlg}
    Dialog $dlg -title "BEM MMTL Parameter Sweep" \
	-cancel 1 -modal none -transient 0
    $dlg add -text Run
    $dlg add -text Cancel

    #--------------------------------------------------------
    #  Create a scrollable frame in a scrolled window
    #  to hold the entries.  Set column properties for resize.
    #--------------------------------------------------------
    set dlgframe [$dlg getframe]
    set sw [ScrolledWindow $dlgframe.sw]
    set sf [ScrollableFrame $sw.sf -constrainedwidth 1 ]
    $sw setwidget $sf
    set f [$sf getframe]
    grid columnconfigure $f {2 3 4} -weight 1


    #--------------------------------------------------------
    #  Column headings
    #--------------------------------------------------------
    set l1 [label $f.lname  -text "Parameter"]
    set l2 [label $f.lstart -text "Start Value"]
    set l3 [label $f.lend   -text "End Value"]
    set l4 [label $f.lnum   -text "Iterations"]

    grid x $l1 $l2 $l3 $l4 -padx 4 -pady 4

    #--------------------------------------------------------
    #  Entries for "basic" parameters
    #--------------------------------------------------------
    set basicLabel [label $f.label-basicSim -text "Simulation"]
    set labelRow [lindex [grid size $f] 1]

    sweepEntry $f "Conductor Segments" ::bem::_doCseg \
	::bem::_startcs ::bem::_endcs ::bem::_numcs
    set ::bem::_startcs $cSegs

    sweepEntry $f "Dielectric Segments" ::bem::_doPseg \
	::bem::_startps ::bem::_endps ::bem::_numps
    set ::bem::_startps $pSegs

    sweepEntry $f "Coupling Length" ::bem::_doClngLgt \
	::bem::_startcl ::bem::_endcl ::bem::_numcl
    set ::bem::_startcl $::Stackup::couplingLength

    sweepEntry $f "Rise Time" ::bem::_doRseTme \
	::bem::_startrt ::bem::_endrt ::bem::_numrt
    set ::bem::_startrt $::Stackup::riseTime

    grid $basicLabel -row $labelRow -sticky sw -padx 4 -pady 4


    #--------------------------------------------------------
    #  Initialize arrays controlling sweep iterations
    #  Arrays are indexed by $struct-$varname
    #--------------------------------------------------------
    array unset _sweepFlag
    array unset _sweepStart
    array unset _sweepEnd
    array unset _sweepCount


    #--------------------------------------------------------
    #  Walk the Stackup, adding parameters for each structure
    #--------------------------------------------------------
    foreach struct $::Stackup::structureList {

	set structName [namespace tail $struct]
	set structClass [namespace tail [$struct info class]]
	#--------------------------------------------------------
	#  Choose sweep-able parameters based on
	#  the struct class.  We could just call
	#  'configure' and get a list, but it would
	#  be in alphabetical order...
	#--------------------------------------------------------
	switch -exact -- $structClass {
	    GroundPlane {
		set parameters {thickness}
	    }
	    DielectricLayer {
		set parameters {permittivity permeability lossTangent
		    thickness}
	    }
	    RectangleDielectric {
		set parameters {permittivity permeability lossTangent
		    width height
		    number pitch "x Offset" "y Offset"}
	    }
	    RectangleConductors {
		set parameters {conductivity width height
		    number pitch "x Offset" "y Offset"}
	    }
	    TrapezoidConductors {
		set parameters {conductivity topwidth bottomWidth height
		    number pitch "x Offset" "y Offset"}
	    }
	    CircleConductors {
		set parameters {conductivity diameter
		    number pitch "x Offset" "y Offset"}
	    }
	    default {
		puts stderr "Warning:  Structure $struct has unknown class $structClass"
	    }
	}

	#--------------------------------------------------------
	#  Check if we want this structure on the dialog
	#--------------------------------------------------------
	if { [llength $parameters] < 1 } {
	    continue
	}

	#--------------------------------------------------------
	#  Create a Title label for this structure
	#  (Can't grid it until after sweepEntries are
	#  created.)
	#--------------------------------------------------------
	set structLabel [label $f.label-$structName -text $structName]
	set labelRow [lindex [grid size $f] 1]

	#--------------------------------------------------------
	#  Create parameter lines within the title frame
	#--------------------------------------------------------
	foreach p $parameters {
	    set varname [string map {" " ""} $p]
	    set label [string totitle $p]
	    set index "$struct-$varname"

	    sweepEntry $f $label ::bem::_sweepFlag($index) \
		::bem::_sweepStart($index) ::bem::_sweepEnd($index) \
		::bem::_sweepCount($index)
	    set ::bem::_sweepStart($index) [$struct cget -$varname]
	}

	grid $structLabel -row $labelRow -column 0 -sticky sw -padx 4 -pady 4
    }

    #--------------------------------------------------------
    #  Final sizing for the dialog.
    #  Note that the scrolled window won't automatically
    #  take on the size of the contained widgets.
    #  So we call update to compute the size of the
    #  enclosing grid frame, then insist that the
    #  scrollable frame have the correct width.
    #--------------------------------------------------------
    pack $sw -expand y -fill both
    update idletasks
    set w [winfo width $sf.frame]
    $sf configure -width $w

    $dlg draw

}

#--------------------------------------------------------
#
#  ::bem::sweepEntry
#
#  Create and grid widgets for a sweep dialog entry.
#  This creates a checkbutton and three entry widgets for
#  start value, end value, and number of iterations.
#  the widgets are created with associated textvariables,
#  and gridded into the specified frame.
#
#--------------------------------------------------------
set ::bem::BuildCount 0
proc ::bem::sweepEntry { f desc flagvar startvar endvar numvar } {

    #  Create widgets
    set c [incr ::bem::BuildCount]
    set estart [entry $f.estart$c -width 10 -textvariable $startvar]
    set eend   [entry $f.eend$c   -width 10 -textvariable $endvar]
    set enum   [entry $f.enum$c   -width 10 -textvariable $numvar]
    set cbox [checkbutton $f.cb$c -variable $flagvar -text $desc]

    #  Set command for enable/disable of this set of fields,
    #  and set initial state.
    $cbox configure \
	-command [list ::bem::sweepEntrySelect $flagvar $estart $eend $enum]
    ::bem::sweepEntrySelect $flagvar $estart $eend $enum

    #  Arrange widgets in the parent
    grid x $cbox  $estart $eend $enum -sticky ew -padx 4 -pady 4
    grid $cbox -sticky w

}

#--------------------------------------------------------
#
#  ::bem::sweepEntrySelect
#
#  Toggles entry widgets normal/disabled for
#  the fields on the sweepBEM dialog.
#
#--------------------------------------------------------
proc ::bem::sweepEntrySelect {flagvar estart eend enum} {

    upvar $flagvar flag
    if { $flag } {
	$estart configure -state normal
	$eend   configure -state normal
	$enum   configure -state normal
    } else {
	$estart configure -state disabled
	$eend   configure -state disabled
	$enum   configure -state disabled
    }
}







#--------------------------------------------------------
# Build the gui interface containing all items that the
# application can sweep.
#
# nodename: name of the cross-section file (without the extension)
# cSegs   : number of C segments to mesh
# pSegs   : number of P segments to mesh
# doGui   : 1 - being run from mmtl.tcl  0 - outside of mmtl.tcl
# parent  : widget id or 0
#
#----------------------------------------------------------
proc ::bem::bemRunSweepMMTL { nodename cSegs pSegs  {doGui 0} {parent 0} \
	{wdgt 0} } {


    set ::gui::_nodename $nodename
    set ::bem::Csegs $cSegs
    set ::bem::Psegs $pSegs
    set ::bem::Gui $doGui
    set ::bem::Parent $parent

    #--------------------------------------------
    #  Initialize arrays of sweeping parameters.
    #  Each parameter has a start value, end value
    #  and number of iterations.
    #--------------------------------------------
    array unset ::bem::_arrStartLis
    array unset ::bem::_arrEndList
    array unset ::bem::_arrNumList

    #--------------------------------------------
    #  Dielectric layer parameters are
    #  thickness and permittivity.
    #--------------------------------------------
    array unset ::bem::_arrDielThckList
    array unset ::bem::_arrDielPrmtList

    #--------------------------------------------
    #  RectangleConductors allow sweeping
    #  width, height, conductivity and pitch,
    #  and X and Y offsets.
    #--------------------------------------------
    array unset ::bem::_arrRectWidthList
    array unset ::bem::_arrRectHeightList
    array unset ::bem::_arrRectCndctList
    array unset ::bem::_arrRectPtchList
    array unset ::bem::_arrRectXoffList
    array unset ::bem::_arrRectYoffList


    #--------------------------------------------
    #  CircleConductors allow sweeping
    #  diameter, conductivity and pitch,
    #  and X and Y offsets.
    #--------------------------------------------
    array unset ::bem::_arrCircDiamList
    array unset ::bem::_arrCircCndctList
    array unset ::bem::_arrCircPtchList
    array unset ::bem::_arrCircXoffList
    array unset ::bem::_arrCircYoffList


    #--------------------------------------------
    #  TrapezoidConductors allow sweeping
    #  top and bottom width, height, conductivity
    #  and pitch, and X and Y offsets.
    #--------------------------------------------
    array unset ::bem::_arrTrapWidthTopList
    array unset ::bem::_arrTrapWidthBotList
    array unset ::bem::_arrTrapHeightList
    array unset ::bem::_arrTrapCndctList
    array unset ::bem::_arrTrapPtchList
    array unset ::bem::_arrTrapXoffList
    array unset ::bem::_arrTrapYoffList

    if { [string length $nodename] < 1 } {
	_open_existing
    }

    set ::bem::_doCseg    0
    set ::bem::_doPseg    0
    set ::bem::_doClngLgt 0
    set ::bem::_doRseTme  0

    #--------------------------------------------
    #  Create the sweep dialog
    #--------------------------------------------
    set ::bem::_sweepParamDialog [Dialog .sweep -title "Sweep Parameters" \
				-side bottom -anchor e -default 0 -cancel 1]
    $::bem::_sweepParamDialog add -text "Ok"
    $::bem::_sweepParamDialog add -text "Cancel"

    set ::bem::_sweepFrame [$::bem::_sweepParamDialog getframe]

    set _sweepBox [ ::gui::guiCheckbox_create $::bem::_sweepFrame.rb \
			"Choose Sweep Parameters"]
    pack $_sweepBox -side left -expand true -fill x

    #--------------------------------------------
    #  Add checkbuttons for all possible
    #  "sweep-able" values
    #--------------------------------------------
    ::gui::guiCheckbox_add $_sweepBox "C-seg" ::bem::_doCseg
    ::gui::guiCheckbox_add $_sweepBox "D-seg" ::bem::_doPseg
    ::gui::guiCheckbox_add $_sweepBox "CouplingLength" ::bem::_doClngLgt
    ::gui::guiCheckbox_add $_sweepBox "RiseTime" ::bem::_doRseTme

    foreach itm [itcl::find objects -class DielectricLayer] {
	set ::bem::_arrDielThckList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm thickness" ::bem::_arrDielThckList($itm)
	set ::bem::_arrDielPrmtList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm permittivity" ::bem::_arrDielPrmtList($itm)
    }

    foreach itm [itcl::find objects -class RectangleConductors] {
	set ::bem::_arrRectWidthList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm width" ::bem::_arrRectWidthList($itm)
	set ::bem::_arrRectHeightList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm height" ::bem::_arrRectHeightList($itm)
	set ::bem::_arrRectCndctList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm conductivity" \
		::bem::_arrRectCndctList($itm)
	set ::bem::_arrRectPtchList($itm) 0
	if { [$itm cget -pitch] > 1 } {
	    ::gui::guiCheckbox_add $_sweepBox "$itm pitch" ::bem::_arrRectPtchList($itm)
	}
	set ::bem::_arrRectXoffList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm xOffset" ::bem::_arrRectXoffList($itm)
	set ::bem::_arrRectYoffList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm yOffset" ::bem::_arrRectYoffList($itm)
    }

    foreach itm [itcl::find objects -class CircleConductors] {
	set ::bem::_arrCircDiamList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm diameter" ::bem::_arrCircDiamList($itm)
	set ::bem::_arrCircCndctList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm conductivity" ::bem::_arrCircCndctList($itm)
	set ::bem::_arrCircPtchList($itm) 0
	if { [$itm cget -pitch] > 1 } {
	    ::gui::guiCheckbox_add $_sweepBox "$itm pitch" ::bem::_arrCircPtchList($itm)
	}
	set ::bem::_arrCircXoffList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm xOffset" ::bem::_arrCircXoffList($itm)
	set ::bem::_arrCircYoffList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm yOffset" ::bem::_arrCircYoffList($itm)
    }

    foreach itm [itcl::find objects -class TrapezoidConductors] {
	set ::bem::_arrTrapWidthTopList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm topWidth" ::bem::_arrTrapWidthTopList($itm)
	set ::bem::_arrTrapWidthBotList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm botWidth" ::bem::_arrTrapWidthBotList($itm)
	set ::bem::_arrTrapHeightList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm height" ::bem::_arrTrapHeightList($itm)
	set ::bem::_arrTrapCndctList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm conductivity" \
		::bem::_arrTrapCndctList($itm)
	set ::bem::_arrTrapPtchList($itm) 0
	if { [$itm cget -pitch] > 1 } {
	    ::gui::guiCheckbox_add $_sweepBox "$itm pitch" ::bem::_arrTrapPtchList($itm)
	}
	set ::bem::_arrTrapXoffList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm xOffset" ::bem::_arrTrapXoffList($itm)
	set ::bem::_arrTrapYoffList($itm) 0
	::gui::guiCheckbox_add $_sweepBox "$itm yOffset" ::bem::_arrTrapYoffList($itm)
    }

    set result [$::bem::_sweepParamDialog draw]
    destroy $::bem::_sweepParamDialog
    if { $result == 0 } {
	::bem::_doSweep
    }

}


proc ::bem::_doSweep {} {

    set ::bem::_row 1
    set ::bem::_col 0
    set ::bem::_startcs 0
    set ::bem::_endcs   0
    set ::bem::_numcs   0
    set ::bem::_startps 0
    set ::bem::_endps   0
    set ::bem::_numps   0
    set ::bem::_startcl 0
    set ::bem::_endcl   0
    set ::bem::_numcl   0
    set ::bem::_startrt 0
    set ::bem::_endrt   0
    set ::bem::_numrt   0
    set ::bem::BuildCount     0

    set ::bem::_spFrame [toplevel .parameterValues]
    wm iconname $::bem::_spFrame "Sweep1"
    wm iconify $::bem::_spFrame

    if { $::bem::_doCseg } {
	set ::bem::_startcs $::bem::Csegs
	_buildSweepParameters $::bem::_spFrame "C-Seg" \
		"::bem::_startcs" "::bem::_endcs" "::bem::_numcs"
    }

    if { $::bem::_doPseg } {
	set ::bem::_startps $::bem::Psegs
	::bem::_buildSweepParameters $::bem::_spFrame "D-Seg" \
		"::bem::_startps" "::bem::_endps" "::bem::_numps"
    }

    if { $::bem::_doClngLgt } {
	set ::bem::_startcl $::Stackup::couplingLength
	::bem::_buildSweepParameters $::bem::_spFrame "Coupling Length" \
		"::bem::_startcl" "::bem::_endcl" "::bem::_numcl"
    }

    if { $::bem::_doRseTme } {
	set ::bem::_startrt $::Stackup::riseTime
	::bem::_buildSweepParameters $::bem::_spFrame "Rise Time" \
		"::bem::_startrt" "::bem::_endrt" "::bem::_numrt"
    }

    foreach nme [itcl::find objects -class DielectricLayer] {
	if { $::bem::_arrDielThckList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme T [$nme cget -thickness] \
		    "$nme Thickness"
	}
	if { $::bem::_arrDielPrmtList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme P [$nme cget -permittivity] \
		    "$nme Permittivity"
	}
    }

    foreach nme [itcl::find objects -class RectangleConductors] {
	if { $::bem::_arrRectWidthList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme RW [$nme cget -width] \
		    "$nme Width"
	}

	if { $::bem::_arrRectHeightList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme RH [$nme cget -height] \
		    "$nme Height"
	}

	if { $::bem::_arrRectCndctList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme RC [$nme cget -conductivity] \
		    "$nme Conductivity"
	}

	if { $::bem::_arrRectPtchList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme RP [$nme cget -pitch] \
		    "$nme Pitch"
	}

	if { $::bem::_arrRectXoffList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme RX [$nme cget -xOffset] \
		    "$nme xOffset"
	}

	if { $::bem::_arrRectYoffList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme RY [$nme cget -yOffset] \
		    "$nme yOffset"
	}
    }


    foreach nme [itcl::find objects -class CircleConductors] {
	if { $::bem::_arrCircDiamList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme CD \
		    [$nme cget -diameter] \
		    "$nme Diameter"
	}

	if { $::bem::_arrCircCndctList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme CC [$nme cget -conductivity] \
		    "$nme Conductivity"
	}

	if { $::bem::_arrCircPtchList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme CP [$nme cget -pitch] \
		    "$nme Pitch"
	}
	if { $::bem::_arrCircXoffList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme CX [$nme cget -xOffset] \
		    "$nme xOffset"
	}

	if { $::bem::_arrCircYoffList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme CY [$nme cget -yOffset] \
		    "$nme yOffset"
	}
    }


    foreach nme [itcl::find objects -class TrapezoidConductors] {
	if { $::bem::_arrTrapWidthTopList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme TWT \
		    [$nme cget -topWidth] \
		    "$nme TopWidth"
	}

	if { $::bem::_arrTrapWidthBotList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme TWB \
		    [$nme cget -bottomWidth] \
		    "$nme BotWidth"
	}

	if { $::bem::_arrTrapHeightList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme TH [$nme cget -height] \
		    "$nme Height"
	}

	if { $::bem::_arrTrapCndctList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme TC [$nme cget -conductivity] \
		    "$nme Conductivity"
	}

	if { $::bem::_arrTrapPtchList($nme) } {

	    ::bem::_buildSweepEntryFrame $nme TP [$nme cget -pitch] \
		    "$nme Pitch"
	}
	if { $::bem::_arrTrapXoffList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme TX [$nme cget -xOffset] \
		    "$nme xOffset"
	}

	if { $::bem::_arrTrapYoffList($nme) } {
	    ::bem::_buildSweepEntryFrame $nme TY [$nme cget -yOffset] \
		    "$nme yOffset"
	}
    }

    set but1 [::gui::guiBuildButton $::bem::_spFrame "OK" \
	    "::bem::_finishSweep" "Proceed with the requested sweep"]
    set but2 [::gui::guiBuildButton $::bem::_spFrame "Cancel" \
	    "::bem::_cancelBldSweep" "Cancel the sweep"]
    grid $but1 $but2 -sticky nw


    wm deiconify $::bem::_spFrame
    grab $::bem::_spFrame
}

proc ::bem::_buildSweepEntryFrame { nme type value txtStrg } {

    set indx [format "%s-$type" $nme]
    set ::bem::_arrStartList($indx) $value
    set ::bem::_arrEndList($indx) 0
    set ::bem::_arrNumList($indx) 0

    ::bem::_buildSweepParameters $::bem::_spFrame $txtStrg \
	    ::bem::_arrStartList($indx) ::bem::_arrEndList($indx) \
	    ::bem::_arrNumList($indx)
}


proc ::bem::_cancelBldSweep {} {

    destroy $::bem::_spFrame
}

proc ::bem::_buildSweepParameters { parent title  startVar endVar iterVar } {

    incr ::bem::BuildCount
    set frme [frame $parent.frme$::bem::BuildCount \
		  -relief groove -borderwidth 3]
    set seglbl [label $frme.seglbl$::bem::BuildCount \
		    -justify left -text $title]
    grid $seglbl -columnspan 2 -sticky w

    set startlab [label $frme.startlab -text "Starting Value"]
    set startent [entry $frme.startent -textvariable $startVar]
    grid $startlab $startent -sticky news -padx 2 -pady 2

    set endlab [label $frme.endlab -text "Ending Value"]
    set endent [entry $frme.endent -textvariable $endVar]
    grid $endlab $endent -sticky news -padx 2 -pady 2

    set numlab [label $frme.numlab -text "# Iterations"]
    set nument [entry $frme.nument -textvariable $iterVar]
    grid $numlab $nument -sticky news -padx 2 -pady 2

    grid $frme -row $::bem::_row -column $::bem::_col -sticky new

    if { $::bem::_col == 0 } {
	set ::bem::_col 1
    } else {
	incr ::bem::_row
	set ::bem::_col 0
    }

}


proc ::bem::_finishSweep {} {

    set ::bem::_totalLoops 0
    set ::bem::_curls 0

    set flenme "$::gui::_nodename.sweep_script"

    set fp [open $flenme w]
    puts $fp "proc _runSweep \{\} \{"
    puts $fp "  global _nodename"
    puts $fp "  global _num_c_segs"
    puts $fp "  global _num_p_segs"

    puts $fp " "
    puts $fp "    foreach { nme vle } \[ array get ::bem::_changesArr \] \{"
    puts $fp "      unset ::bem::_changesArr(\$nme)"
    puts $fp "    \}"
    puts $fp "  set resultsFle \[ open \"$::gui::_nodename.swept_result\" w \] "
    puts $fp "  set logFle \[ open \"$::gui::_nodename.swept_result_log\" w \] "

    if { $::bem::_doCseg } {
	puts $fp "  # ConductorSegments  \
		start: $::bem::_startcs  end: $::bem::_endcs   \
		number of iterations: $::bem::_numcs"
	if { [_buildSweepScript $fp "" "C-Seg" $::bem::_startcs $::bem::_endcs $::bem::_numcs ""] } {
	    return
	}
    }

    if { $::bem::_doPseg } {
	puts $fp "  # DielectricSegments  \
		start: $::bem::_startps  end: $::bem::_endps   \
		number of iterations: $::bem::_numps"
	if { [_buildSweepScript $fp "" "D-Seg" $::bem::_startps $::bem::_endps $::bem::_numps ""] } {
	    return
	}
    }

    if { $::bem::_doClngLgt } {
	puts $fp "  # CouplingLength  \
		start: $::bem::_startcl  end: $::bem::_endcl   \
		number of iterations: $::bem::_numcl"
	if { [_buildSweepScript $fp "" "Coupling-length" $::bem::_startcl \
		$::bem::_endcl $::bem::_numcl $::units::default(Length)] } {
	    return
	}

    }
    if { $::bem::_doRseTme } {
	puts $fp "  # Risetime  \
		start: $::bem::_startrt  end: $::bem::_endrt   \
		number of iterations: $::bem::_numrt"
	if { [_buildSweepScript $fp "" "Risetime" $::bem::_startrt \
		$::bem::_endrt $::bem::_numrt $::units::default(Time)] } {
	    return
	}
    }

    foreach nme [itcl::find objects -class DielectricLayer] {

	if { $::bem::_arrDielThckList($nme) } {
	    puts $fp "  # Dielectric thickness  "
	     if { [_addToScript $fp $nme "T" "DielectricLayer" \
		     "thickness" $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrDielPrmtList($nme) } {
	    puts $fp "  # Dielectric permittivity  "
	     if { [_addToScript $fp $nme "P" "DielectricLayer" \
		     "permittivity" ""] } {
		return
	    }
	}
    }

    foreach nme [itcl::find objects -class RectangleConductors] {
	if { $::bem::_arrRectWidthList($nme) } {

	    set indx [ format "%s-RW" $nme ]
	    puts $fp "  # RectangleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "width" $::bem::_arrStartList($indx) \
		      $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}
	if { $::bem::_arrRectHeightList($nme) } {

	    set indx [ format "%s-RH" $nme ]
	    puts $fp "  # RectangleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "height" $::bem::_arrStartList($indx) \
		      $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrRectCndctList($nme) } {

	    set indx [ format "%s-RC" $nme ]
	    puts $fp "  # RectangleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "conductivity" \
		      $::bem::_arrStartList($indx) \
		      $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) "siemens/meter"] } {
		return
	    }
	}

	if { $::bem::_arrRectPtchList($nme) } {

	    set indx [ format "%s-RP" $nme ]
	    puts $fp "  # RectangleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "pitch" $::bem::_arrStartList($indx) \
		      $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrRectXoffList($nme) } {

	    set indx [ format "%s-RX" $nme ]
	    puts $fp "  # RectangleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "xOffset" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrRectYoffList($nme) } {

	    set indx [ format "%s-RY" $nme ]
	    puts $fp "  # RectangleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "yOffset" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}
    }


    foreach nme [itcl::find objects -class CircleConductors] {
	if { $::bem::_arrCircDiamList($nme) } {

	    set indx [ format "%s-CD" $nme ]
	    puts $fp "  # CircleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "diameter" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrCircCndctList($nme) } {

	    set indx [ format "%s-CC" $nme ]
	    puts $fp "  # CircleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "conductivity" \
		    $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) "siemens/meter"] } {
		return
	    }
	}

	if { $::bem::_arrCircPtchList($nme) } {

	    set indx [ format "%s-CP" $nme ]
	    puts $fp "  # CircleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "pitch" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrCircXoffList($nme) } {

	    set indx [ format "%s-CX" $nme ]
	    puts $fp "  # CircleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "xOffset" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrCircYoffList($nme) } {

	    set indx [ format "%s-CY" $nme ]
	    puts $fp "  # CircleConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "yOffset" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}
    }


    foreach nme [itcl::find objects -class TrapezoidConductors] {
	if { $::bem::_arrTrapWidthTopList($nme) } {

	    set indx [ format "%s-TWT" $nme ]
	    puts $fp "  # TrapezoidConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "topWidth" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrTrapWidthBotList($nme) } {

	    set indx [ format "%s-TWB" $nme ]
	    puts $fp "\# TrapezoidConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "bottomWidth" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}
	if { $::bem::_arrTrapHeightList($nme) } {

	    set indx [ format "%s-TH" $nme ]
	    puts $fp "\# TrapezoidConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "height" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrTrapCndctList($nme) } {

	    set indx [ format "%s-TC" $nme ]
	    puts $fp "\# TrapezoidConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "conductivity" \
		    $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) "siemens/meter"] } {
		return
	    }
	}

	if { $::bem::_arrTrapPtchList($nme) } {

	    set indx [ format "%s-TP" $nme ]
	    puts $fp "\# TrapezoidConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "pitch" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrTrapXoffList($nme) } {

	    set indx [ format "%s-TX" $nme ]
	    puts $fp "  # TraezpodConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "xOffset" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}

	if { $::bem::_arrTrapYoffList($nme) } {

	    set indx [ format "%s-TY" $nme ]
	    puts $fp "  # TrapezoidConductors $nme \
		    start: $::bem::_arrStartList($indx) \
		    end: $::bem::_arrEndList($indx) \
		    number of iterations: $::bem::_arrNumList($indx)"
	    if { [_buildSweepScript $fp $nme "yOffset" $::bem::_arrStartList($indx) \
		    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $::units::default(Length)] } {
		return
	    }
	}
    }


    ::bem::_finishSweepFile $fp

    close $fp

    ::bem::_runTheSimulations sweep_script _runSweep \
	    "The generated script will run the\
	    simulation $::bem::_totalLoops times."
}

proc ::bem::_addToScript { fp nme ext type txt units } {

    set indx [ format "%s-%s" $nme $ext]
    puts $fp "  \# $type $nme ($txt) \
	    start: $::bem::_arrStartList($indx) \
	    end: $::bem::_arrEndList($indx) \
	    number of iterations: $::bem::_arrNumList($indx)"
    if { [_buildSweepScript $fp $nme $txt $::bem::_arrStartList($indx) \
	    $::bem::_arrEndList($indx) $::bem::_arrNumList($indx) $units ] } {
	close $fp
	return 1
    }
    return 0
}

proc ::bem::_buildSweepScript { fp nme value strt end num units } {
    global env

    #---------------------------------------------------------
    # Make certain that the number of iterations is an interger
    # number.
    #---------------------------------------------------------
    if { ([scan $num {%d} numLoops] != 1) || \
	 ($numLoops < 1) } {
	::gui::guiPopupWarning \
		"The number of iterations must be an INTEGER > 0."
	return 1
    }
    if { ($num != $numLoops) || ($numLoops < 1 ) } {
	::gui::guiPopupWarning \
		"The number of iterations must be an INTEGER > 0." 
	return 1
    }


    #---------------------------------------------------------
    # Save the total number of loops for running nmmtl.
    #---------------------------------------------------------
    if { $::bem::_totalLoops == 0 } {
	set ::bem::_totalLoops $numLoops
    } else {
	set ::bem::_totalLoops [expr {$::bem::_totalLoops * $numLoops}]
    }

    puts "____ # iterations:  $::bem::_totalLoops ___"

    incr ::bem::_curls

    #  Convert from/to into unit-less number
    #  Adding default units, if necessary
    if { [string is double $strt] } {
        append strt $units
    }
    set from [units::convert $strt $units]
    if { [string is double $end] } {
        append end $units
    }
    set to   [units::convert $end  $units]

    #  Check for valid length units
    if { ! [catch {units::convert $units "meter"}] } {
	if { $from < 0 } {
	    ::gui::guiPopupWarning \
		"The starting dimension must be greater than 0."
	    return 1
	}
	if { $to < 0 } {
	    ::gui::guiPopupWarning \
		"The ending dimension must be greater than 0."
	    return 1
	}
    }

    #---------------------------------------------------------
    # If the number of iterations is 1, set the increment to 0.
    #---------------------------------------------------------
    if { $numLoops == 1 } {
	set incrt 0
    } else {
	set incrt [expr { double($to - $from) / ($numLoops - 1)} ]
    }

    #---------------------------------------------------------
    # Create the for loop.
    #---------------------------------------------------------

    puts $fp "puts \"----------------------------------------\""
    puts $fp "  set val$::bem::_curls $from"
    set strg [ format "  for { set i$::bem::_curls 0 } \
	    { \$i$::bem::_curls < %d } { incr i$::bem::_curls } \{ " \
	    $numLoops ]
    puts $fp $strg

    #---------------------------------------------------------
    # Change the value affected by the for loop.
    #---------------------------------------------------------
    if { [string length $nme] > 0 } {
	puts $fp "    $nme configure -$value \$val$::bem::_curls "
	puts $fp "    set ::bem::_changesArr\($nme-$value) \"$nme $value = \$val$::bem::_curls\""
    } else {
	if { [string compare $value "C-Seg"] == 0 } {
	    puts $fp "    set ::bem::Csegs  \$val$::bem::_curls"
	    puts $fp "    set ::bem::_changesArr\(CSEG) \"CSEG = \$val$::bem::_curls\""
	}
	if { [string compare $value "D-Seg"] == 0 } {
	    puts $fp "    set ::bem::Psegs \$val$::bem::_curls"
	    puts $fp "    set ::bem::_changesArr\(PSEG) \"DSEG = \$val$::bem::_curls\""
	}
	if { [string compare $value "Coupling-length"] == 0 } {
	    puts $fp "    set Stackup\:\:couplingLength \$val$::bem::_curls"
	    puts $fp "    set ::bem::_changesArr\(CL) \"Cplg-lgt =  \$val$::bem::_curls\""
	}
	if { [string compare $value "Risetime"] == 0 } {
	    puts $fp "    set Stackup\:\:riseTime \$val$::bem::_curls"
	    puts $fp "    set ::bem::_changesArr\(RT) \"Risetime = \$val$::bem::_curls\""
	}
    }
  
    puts $fp "    set val$::bem::_curls \[ expr \{ \$val$::bem::_curls + $incrt \}\]"
    return 0
}


proc ::bem::_finishSweepFile { fp } {
    global env

    puts $fp "    puts \$resultsFle \"\""
    puts $fp "    puts \$resultsFle \"_______________________________________________________\""
    puts $fp "    puts \$logFle \"_______________________________________________________\""
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
    puts $fp "    foreach { nme vle } \[ array get ::bem::_changesArr \] \{"
    puts $fp "      puts \$vle"
    puts $fp "    \}"
    puts $fp "    set done \[::bem::bemParseSweptResults \$results \]"
    puts $fp "    if \{ \$done > 1 \} \{"
    puts $fp "      close \$logFle"
    puts $fp "      close \$resultsFle"
    puts $fp "      return"
    puts $fp "     \}"
    
    puts $fp "    puts \$resultsFle \"\""
    puts $fp "    puts \$logFle \"\""
    puts $fp "    puts \"____________________________________________________\""
    puts $fp "    puts \$logFle \$result"

    for { set i 0 } { $i < $::bem::_curls } { incr i } {
	set strg "  \} "
	puts $fp $strg
    }
    puts $fp "  close \$logFle"
    puts $fp "  close \$resultsFle"
    set strg " \}"
    puts $fp $strg
}


proc ::bem::bemParseSweptResults { results} {


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

    set indx [string first Impedance $results]
    set newstrg [string range $results $indx  [string length $results]]
    while { 1 } {
	set indx [string first "Signal L" $newstrg]
	set stp [string first "Effective Dielectric" $newstrg]
	if { $indx > $stp } {
	    break
	}
    	set newstrg [string range $newstrg [expr { $indx + 12 }] \
		[string length $newstrg]]
	set indx [string first "=" $newstrg]
	set nme [string range $newstrg 0 [ expr {$indx - 1}] ]
	set newstrg [string range $newstrg $indx  [string length $newstrg]]
	if { [scan $newstrg {= %g} impedance] < 1 } {
	    puts stderr "**************************************************"
	    puts stderr "Cannot continue because the\
		    impedance value for $nme is set to \"nan\""
	    puts stderr "**************************************************"
	    return 1
	}
	puts "$nme   Impedance = $impedance"
	
    }
    if { $indx1 > $indx2 } {
	puts "** Asymmetry ratio for inductance matrix:" 
	puts "$maxx (max),  $average (average) **"
   }
   return -1
}









