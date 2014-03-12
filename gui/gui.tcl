#----------------------------------------------*-TCL-*------------
#
#  gui.tcl
#
#  Support functions for TNT GUI construction.
#
#    ::gui::guiBuildComboBox
#    ::gui::guiBuildButton
#    ::gui::guiPopupWarning
#    ::gui::guiBuildLabelEntry
#    ::gui::guiCheckbox_create
#    ::gui::guiCheckbox_add
#    ::gui::guiCheckbox_destroy
#    ::gui::guiPopdownBusy
#    ::gui::guiPopupBusy
#    ::gui::guiPopupFile
#    ::gui::guiCloseResults
#    ::gui::guiSetCursorBusy
#    ::gui::guiSetCursorNormal
#    ::gui::guiBuildNmeValList
#    ::gui::drainEventQueue
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: gui.tcl,v 1.11 2004/07/20 14:51:55 techenti Exp $
#
#-----------------------------------------------------------------

package provide gui 2.0


namespace eval ::gui:: {
    
    variable busyLabel
    variable rbInfo
    variable ButtonCount 0
    variable CheckboxCount 0
    variable LabelCount 0
    variable ComboCount 0
    
    variable _annotateFlag 1

    variable f1
    variable tf1
    variable tf3
    variable _groundFrame
    variable _activeFrame

    variable rectCondCB
    variable circCondCB
    variable trapCondCB
    variable permittivityCB
    variable ltangentCB

    variable _tree
    variable _treect 0
    variable _title ""

    variable _canvas
    variable _canvasDraw_visitor

    variable _xSectionObjectList
    variable _xSectionIndexList

######################################
# Groundplane variables
######################################
    variable _gname
    variable _gthickness


######################################
# Dielectric variables
######################################
    variable _dname
    variable _thickness    0
    variable _permittivity 1.0
    variable _permeability 1.0
    variable _lossTangent  0.0

######################################
# Rectangular dielectric variables
######################################
    variable _dRname
    variable _dpermittivity 1.0
    variable _dpermeability 1.0
    variable _dlossTangent  0.0
    variable _dwidth        1
    variable _dheight       1
    variable _dnumber       1
    variable _dpitch        0
    variable _dxoffset      0
    variable _dyoffset      0

######################################
# Rectangular conductor variables
######################################
    variable _rname
    variable _rconductivity copper
    variable _rwidth        1
    variable _rheight       1
    variable _rnumber       1
    variable _rpitch        0
    variable _rxoffset      0
    variable _ryoffset      0

######################################
# Circular conductor variables
######################################
    variable _cname
    variable _cconductivity copper
    variable _cdiameter     1
    variable _cnumber       1
    variable _cpitch        0
    variable _cxoffset      0
    variable _cyoffset      0

######################################
# Trapezoidal conductor variables
######################################
    variable _tname
    variable _tconductivity copper
    variable _ttopwidth     1
    variable _tbotwidth     1
    variable _theight       1
    variable _tnumber       1
    variable _tpitch        0
    variable _txoffset      0
    variable _tyoffset      0

######################################
# Counter for ground-plane
######################################
    variable _grndCount 0

######################################
# Counter for dielectrics
######################################
    variable _dielCount 0

######################################
# Counter for ground-plane
######################################
    variable _condCount 0

    variable _nodename ""
    variable _nodedirectory ""
    variable _currentNode ""

    variable scaleFactor
    variable xrange
    variable yrange

    variable selectedObject
    variable _tagSelected ""
    variable _previousColor black

    variable _cValList

    variable _pValList

    variable _ltValList

    variable _printerName princess
    variable _colorMode color
    variable _savePrinterName
    variable _saveColorMode

    variable _num_c_segs 10
    variable _num_p_segs 10

    namespace export _xSectionObjectList
    namespace export _xSectionIndexList
    namespace export f1
    namespace export tf1
    namespace export tf3
    namespace export _nodename
    
    namespace export guiBuildButton
    namespace export guiOpenExisting
    namespace export guiOpenNew
    namespace export guiBuildComboBox
    namespace export guiBuildLabelEntry
    namespace export guiBuildNmeValList
    namespace export guiCheckbox_create
    namespace export guiCheckbox_add
    namespace export guiCheckbox_destroy
    namespace export guiCreateElements
    namespace export guiPopdownBusy
    namespace export guiPopupBusy
    namespace export guiPopupFile
    namespace export guiPopupResultsFile
    namespace export guiPopupWarning
    namespace export guiSetCursorBusy
    namespace export guiSetCursorNormal
    namespace export guiSetupPrinter
    namespace export guiGenRLinputFiles
    namespace export guiRunRLbatch
    namespace export guiRunRLgraphical
    namespace export guiGenCAPinputFiles
    namespace export guiRunCAPbatch
    namespace export guiRunCAPgraphical
    namespace export guiPrintPS
    namespace export guiReadNewConductivityList
    namespace export guiReadNewLossTangentList
    namespace export guiReadNewPermittivityList
    namespace export guiRunFee2dDisplay
    namespace export guiRunFee2dRun
    namespace export guiRunFee2dPlotTranParameter
    namespace export guiSaveFee2d
    namespace export guiRunSweepMMTL
    namespace export guiRunIterateMMTL
    namespace export guiSave
    namespace export guiSaveAndRun
    namespace export guiSaveAs
    namespace export guiSaveGraphicAndRunOld
    namespace export guiSim2hspicew
    namespace export gui_toggleAnnotation
    namespace export guiViewPlotP
    namespace export guiViewTheGraph

    #  Canvas functions
    namespace export _canvas_zoom
    namespace export _canvas_zoom_fit
    namespace export _canvas_zooming_mode
    namespace export _canvas_zoomStart
    namespace export _canvas_zoomMove
    namespace export _canvas_zoomEnd
    namespace export _canvas_redraw
    namespace export _canvas_toggleAnnotation
    namespace export _canvas_select
    namespace export _canvas_print

    namespace export _helpModCom
    namespace export _helpDelComp
    namespace export _helpMovComp
    namespace export _helpAddComp 
    namespace export _helpSweep
    namespace export _helpIterate
    namespace export _helpRunSimulation
    namespace export _helpSaveRunSimulation
    namespace export _helpMaterialLists
    namespace export _helpFiles
    namespace export _helpFEE2D

    #-------------------------------------------------------------
    # Routines called by xmission.
    #-------------------------------------------------------------
    namespace export _mmtlCreateMain
    namespace export _runAndSend
}


##################################################################
# Bulid a ComboBox widget. 
#   parent   :  parent widget
#   varg     :  variable that contains the value
#   labelStrg:  label
##################################################################
proc ::gui::guiBuildComboBox { parent varg labelStrg } {

    set lb [Label $parent.l$::gui::ComboCount -text $labelStrg]
    set sb [ComboBox $parent.s$::gui::ComboCount \
	    -textvariable $varg \
	    -values {1 2 3 4 5 6 7 8 9 10} \
	    -width 10]

    incr ::gui::ComboCount
    grid $lb $sb -sticky news -padx 4 -pady 4
    grid $lb -sticky e
}


##################################################################
# Build a button.
##################################################################
proc ::gui::guiBuildButton { parent txt cmmnd helptxt} {

    set but [Button $parent.but$::gui::ButtonCount \
            -text $txt  -borderwidth 8 \
            -helptext $helptxt \
            -command $cmmnd \
            -relief link]
    incr ::gui::ButtonCount
    return $but
}


proc ::gui::guiPopupWarning { mssge } {

    tk_dialog .tkw "Warning" $mssge warning {} "OK"
}

##################################################################
# Build a LabelEntry widget. 
#   parent   :  parent widget
#   varg     :  variable that contains the value
#   labelStrg:  label
##################################################################
proc ::gui::guiBuildLabelEntry { parent varg labelStrg } {

    set lab [Label $parent.lab$::gui::LabelCount -text $labelStrg]
    set ent [Entry $parent.ent$::gui::LabelCount -textvariable $varg -width 10]
    incr ::gui::LabelCount

    grid $lab $ent -sticky new -padx 4 -pady 4
    grid $lab -sticky e
}


proc ::gui::guiCheckbox_create {win {title ""}} {
    
    set ::gui::rbInfo($win-current) ""
    set ::gui::rbInfo($win-count) 0

    frame $win -class Checkbox
    
    grid rowconfigure $win 0 -weight 1
    grid columnconfigure $win 0 -weight 1

    if { $title != "" } {
        label $win.title -text $title
        pack $win.title -side top
    }
    frame $win.border -borderwidth 2 -relief groove
    pack $win.border -side top -expand true -fill both 
    
    bind $win <Destroy> [ list  ::gui::guiCheckbox_destroy $win ]
    return $win
}


proc ::gui::guiCheckbox_add { win choice varble {command ""}} {

    set cmnd [format {set %s 0} $varble]
    eval $cmnd
    set name "$win.border.rb[incr ::gui::rbInfo($win-count)]"

    checkbutton $name -text $choice -command $command \
            -variable $varble
    
    grid $name -row $::gui::CheckboxCount -sticky w
    incr ::gui::CheckboxCount

}

proc ::gui::guiCheckbox_destroy { win } {
    

    unset ::gui::rbInfo($win-count)
    unset ::gui::rbInfo($win-current)
}

##################################################################
##################################################################
proc ::gui::guiPopdownBusy {} {
    
    destroy $::gui::busyLbl
}

##################################################################
##################################################################
proc ::gui::guiPopupBusy { wdgt txt } {

    set ::gui::busyLbl [toplevel .busyLbl]
    set lbl [Label $::gui::busyLbl.busyLbl -width 100 -height 50 \
	    -fg red -bg black \
	    -justify center -text $txt]
    pack $lbl -side top -expand false
    grab $::gui::busyLbl
    update
}


#-----------------------------------------------------------------
#
#  ::gui::guiPopupFile
#
#  Pop up a non-modal dialog showing the contents of a file.
#  Make it non-modal, so that users can have more than one up
#  at the same time.  This implies that it chooses a unique name
#  and destroys itself when done.
#
#  Handy for displaying results from simulation runs.
#
#-----------------------------------------------------------------
proc ::gui::guiPopupFile { filename } {

    #  Check that filename exists
    if { ! [file exists $filename] } {
	::gui::guiPopupWarning "$filename doesn't exist!"
	return
    }

    #  Create dialog with unique name
    set i 1
    while { [winfo exists .result$i] } {
	incr i
    }
    set dlg .result$i
    Dialog $dlg -title $filename -modal none -transient 0 \
	-homogeneous 1 -side bottom -anchor e -default 0 -cancel 0
    $dlg add -text Close -command [list destroy $dlg]

    set sw [ScrolledWindow [$dlg getframe].sw]
    set txt [text $sw.text]
    $sw setwidget $txt
    pack $sw -padx 4 -pady 4 -expand true -fill both



    #  Read data
    if { [catch {set fp [open $filename "r"]} result] } {
	set data "<empty>"
    } else {
	set data [read $fp]
	close $fp
    }

    #  Display data
    $txt delete 1.0 end
    $txt insert end $data
    $txt see 1.0
    $dlg draw
}



##################################################################
##################################################################
proc ::gui::guiSetCursorBusy {f1} {

    eval $f1 configure -cursor watch
    update
}


##################################################################
##################################################################
proc ::gui::guiSetCursorNormal {f1} {

    eval $f1 configure -cursor left_ptr
    update
}


##################################################################
# Build the material/value list.
##################################################################
proc ::gui::guiBuildNmeValList { parent valList labelString varble} {
    global $valList

    set lst ""
    foreach { name value } [ eval array get $valList ] {
	set val [format "\{%s %s\}" $name $value]
	append lst "$val "
    }
    set lb [label $parent.lb$::gui::ComboCount -text $labelString]
    set sb [ComboBox $parent.sb$::gui::ComboCount \
	    -textvariable $varble -width 10 \
	    -values $lst ]
    incr ::gui::ComboCount

    grid $lb $sb -sticky news -padx 4 -pady 4
    return $sb
}


#-----------------------------------------------------------------
#
#  ::gui::_drainEventQueue
#
#  Windows has a rather odd bug with the native file-open
#  dialog, which Tk uses for tk_getOpenFile and tk_getSaveFile.
#  If you double-click on a filename, the Windows dialog only
#  consumes 1.5 clicks (<down><up><down>), then closes the
#  dialog.  The remaining half click (<up> event) propagates
#  down to the window below.  Since this application has a
#  number of buttons on the main window, it is common to have
#  these buttons fire their command event after double-click
#  selecting a file.
#
#  Several workaround hacks are described in comp.lang.tcl,
#  including draining the event queue, revectoring bindings
#  for a fraction of a second, and even some C code patches
#  to Tk.
#
#  This code uses [grab] to direct all events to an
#  inoccuous widget (a label), drains the event
#  queue by calling [update], then releases the grab.
#  The draining widget must be mapped, so we search
#  [winfo children .].
#
#-----------------------------------------------------------------
proc ::gui::drainEventQueue {} {
    #  Search for a mapped Label widget in children of "."
    set wlist [winfo children .]
    while { [llength $wlist] > 0 } {
	set w [lindex $wlist 0]
	set wlist [lrange $wlist 1 end]
	#  If we've got a mapped Label Widget, drain the queue
	if { [winfo ismapped $w] } {
	    if { [winfo class $w] eq "Label" } {
		grab $w
		update
		grab release $w
		return
	    }
	    #  Not a label, but ismapped, so add chldren to search
	    set wlist [concat $wlist [winfo children $w]]
	}
    }
    #  if we fall through, then there wan't a suitable widget.
    #  Tough luck.
}

