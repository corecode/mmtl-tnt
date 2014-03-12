#----------------------------------------------------------------
#
#  mmtl_build.tcl
#
#  Build the TNT GUI main window
#
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved
#  $Id: gui_build.tcl,v 1.25 2004/07/20 14:51:55 techenti Exp $
#
#----------------------------------------------------------------
package require Tk
package require BWidget

package provide gui 2.0

##################################################################
#
#  createElements
#
#  The TNT gui can be created for a standalone application,
#  or as an integrated part of Xmission.  For standalone
#  mode, the main panels are arranged vertically, and
#  the integrated version arranges them horizontally with
#  some extra control buttons.
#
##################################################################
proc ::gui::guiCreateElements { nb {mmtlInterface 1} } {

    #------------------------------------------------------
    #  Create and populate the three main frames
    #  of the user interface:  title, control, and canvas.
    #------------------------------------------------------
    set f [frame $nb.tnt]
    set titleframe [_createTitleframeElements $f]
    set controlframe [_createControlframeElements $f]
    set canvasframe [_createCanvasframeElements  $f]


    #------------------------------------------------------
    # Check if this is the TNT gui interface.
    #------------------------------------------------------
    if { $mmtlInterface } {

	#------------------------------------------------------
	#  Manage the three main frames, from top to bottom
	#------------------------------------------------------
	grid $titleframe   -padx 4 -pady 4 -sticky news
	grid $controlframe -padx 4 -pady 4 -sticky news
	grid $canvasframe  -padx 4 -pady 4 -sticky news
	grid columnconfigure $f 0 -weight 1
	grid    rowconfigure $f 2 -weight 1

	#------------------------------------------------------
	#  Create the console, using sppdgTcllib's
	#  wrapper around TkCon.
	#------------------------------------------------------
	package require console
	console::create -title "TNT Console" \
	    -variable ::gui::_showConsole

    } else {

	#------------------------------------------------------
	#  This isn't TNT, so we manage the frames
	#  differently and add control buttons for BEM
	#  MMTL simulation
	#------------------------------------------------------
	set bb [ButtonBox $f.controlButtons -default -1 -homogeneous 1]

	$bb add -text "New" -command mmtlOpenNew -helptext "Create a new file"

	$bb add -text "New"  -command mmtlOpenNew \
	    -helptext "Create a new file"
	$bb add -text "Open"  -command mmtlOpenExisting \
		-helptext "Open an existing file"
	$bb add -text "Save"  -command mmtlSave \
	    -helptext "Save the file"
	$bb add -text "SaveAs"  -command mmtlSaveAs \
	    -helptext "Save the file under a different name"
	$bb add -text "Run"  -command saveAndRunBem \
	    -helptext "Save graphic file, run BEM sim, retrieve results."
	$bb add -text "View"  -command mmtlView \
	    -helptext "Send MMTL results to log window."

	grid $titleframe   $canvasframe   -padx 4 -pady 4 -sticky news
	grid $controlframe   ^            -padx 4 -pady 4 -sticky news
	grid $bb             -            -padx 4 -pady 4 -sticky news
	grid columnconfigure $f 2 -weight 1
	grid    rowconfigure $f 1 -weight 1

    }

    #----------------------------------------------------
    # Read in the material - value lists for conductivity,
    # loss-tangent and permittivity.
    #----------------------------------------------------
    ::gui::_readMaterialList "Read Conductivity List" ::gui::_cValList 0 \
	    [file join $::gui::scriptDir conductivity.list]
    ::gui::_readMaterialList "Read lossTangent List" ::gui::_ltValList 0 \
	    [file join $::gui::scriptDir loss_tangent.list]
    ::gui::_readMaterialList "Read Permittivity List" ::gui::_pValList 0 \
	    [file join $::gui::scriptDir permittivity.list]

    #  _pValList is actually an array.  Lets copy the values
    #  into a proper list.
    set  ::gui::_permittivityList [list]
    foreach { name value } [ array get ::gui::_pValList ] {
	lappend ::gui::_permittivityList "$name $value"
    }
    #  _ltValList is actually an array.  Lets copy the values
    #  into a proper list.
    set  ::gui::_losstangentList [list]
    foreach { name value } [ array get ::gui::_ltValList ] {
	lappend ::gui::_losstangentList "$name $value"
    }
    #  _cValList is actually an array.  Lets copy the values
    #  into a proper list.
    set  ::gui::_conductivityList [list]
    foreach { name value } [ array get ::gui::_cValList ] {
	lappend ::gui::_conductivityList "$name $value"
    }

    return $f
}


proc ::gui::_protectFromWindowsDoubleClickBug {} {
    puts protecting
    set newEvent [event info <<KeyOrButton>>]
    if { ($::tcl_platform(platform) eq "windows") && ($newEvent eq "")} {
	event add <<KeyOrButton>> <Button> <Key>
	bind Nascent <<KeyOrButton>> {break}
	bind all <Expose> {+ ::gui::_ignoreKeyOrButtonEvents %W}
    }
}
proc ::gui::_ignoreKeyOrButtonEvents { w } {
    bindtags $w [linsert [bindtags $w] 0 Nascent]
    after 300 [list ::gui::_restoreKeyOrButtonEvents $w]
}
proc ::gui::_restoreKeyOrButtonEvents { w } {
    if {[winfo exists $w]} {
	bindtags $w [lreplace [bindtags $w] 0 0]
    }
}


#-----------------------------------------------------------------
#  _createCanvas
#
#  Creates the cross section drawing canvas and supporting
#  widgets, which include zoom buttons and scrollbars.
#
#-----------------------------------------------------------------
proc ::gui::_createCanvasframeElements {parent} {

    set f [frame $parent.canvasframe]

    #----------------------------------------------------
    #  Create the canvas, and save the widget name
    #  in a global - as it is used by drawing objects.
    #----------------------------------------------------
    set ::gui::_canvas [canvas $f.canvas -width 500 \
			    -relief groove -borderwidth 3 \
			    -xscrollcommand [list $f.hscroll set] \
			    -yscrollcommand [list $f.vscroll set] ]


    #----------------------------------------------------
    #  Scrollbars are nice
    #----------------------------------------------------
    scrollbar $f.hscroll -command [list $f.canvas xview] \
	-orient horizontal
    scrollbar $f.vscroll -command [list $f.canvas yview]

    #----------------------------------------------------
    #  Create zoom buttons
    #----------------------------------------------------
    button $f.zoom_in \
	-command [list ::gui::_canvas_zoom $f.canvas 1.25] \
	-image [image create photo viewmag+-22 -data {
	    R0lGODlhFgAWAIYAAPwCBHSe1GyWzFyOxFSGvER6rDxyrHym3MTm/Nzy/Oz6
	    /OTy/Ex+rBxelMzq/Oz2/Pz+/PT+/PT6/OT2/NTu/ARKhLzi/Mzm/Lze/NTq
	    /Nzq/Mzi/GyezMTi/LTa/Nzu/LTW/Lza/LzW/LTS/KzO/KzS/KTK/KzK/KTG
	    /KzG/Dx6rLTe/JzC/KS+/Jy6/JS69CRmnJSy/JSu/Iyq/AROhKTC/JS2/Iyi
	    /Hye9LSuBISe/HyS9HSK9Pz+BKRiDPyWHMSOLKxmDKRaBMySXFwyBMSOVISq
	    3Fw2BER6tHyq3MySLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAA
	    LAAAAAAWABYAAAf/gACCggECAwQFBoOLjIIHAggICQoLCQwNjYwHCA4PEBAR
	    EhITFBWZjhYLn54PrQsXGKaZAhkSnxAKrQkaG7GZHBS2oq67HR4YHrKLAh/C
	    rQ8LGhcgISAiyoMECQ8Sz9IiIyMkJSTYggQbHR8Z3+IkJicoKeYAKhgrGCEi
	    JSUnKSwtXLh4QQ8AjGogSohIgUJgDBkzaJxqEI5EvBo2Ysy4gQNHhRynKpw4
	    UaOFDRk4dOzgUQFCDx8hK7h4iGNlhQo/ekAAEuQUgJsVaAQFIOTHECA7e/ps
	    RERIEaRAYA4yosjnER9Hkw46gGQpgKZPd/owkiSEAbJLifh4qsSHCgMqFIQW
	    XKS2iFpBRuYyJaIUwAG9SwMBACH+aENyZWF0ZWQgYnkgQk1QVG9HSUYgUHJv
	    IHZlcnNpb24gMi41DQqpIERldmVsQ29yIDE5OTcsMTk5OC4gQWxsIHJpZ2h0
	    cyByZXNlcnZlZC4NCmh0dHA6Ly93d3cuZGV2ZWxjb3IuY29tADs=
	}]

    button $f.zoom_out \
	-command [list ::gui::_canvas_zoom $f.canvas 0.80] \
	-image [image create photo viewmag--22 -data {
	    R0lGODlhFgAWAIYAAPwCBHSe1GyWzFyOxFSGvER6rDxyrHym3MTm/Nzy/Oz6
	    /OTy/Ex+rBxelMzq/Oz2/Pz+/PT+/PT6/OT2/NTu/ARKhLzi/Mzm/Lze/NTq
	    /Nzq/Mzi/GyezMTi/LTa/Nzu/LTW/Lza/LzW/LTS/KzO/KzS/KTK/KzK/KTG
	    /KzG/Dx6rLTe/JzC/KS+/Jy6/JS69CRmnJSy/JSu/Iyq/AROhKTC/JS2/Iyi
	    /Hye9LSuBISe/HyS9HSK9Pz+BKRiDPyWHMSOLKxmDKRaBMySXFwyBMSOVFw2
	    BISq3Hyq3ISq5MySLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAA
	    LAAAAAAWABYAAAf/gACCggECAwQFBoOLjIIHAggICQoLCQwNjYwHCA4PEBAR
	    EhITFBWZjhYLn54PrQsXGKaZAhkSnxAKrQkaG7GZHBS2oq67HR4YHrKLAh/C
	    rQ8LGhcgISAiyoMECQ8Sz9IiIyMkJSTYggQbHR8Z3+IkJicoKeYAKhgrGCEi
	    JSUnKSwtXLh4QQ8AjGogSohIgUJgDBkzaJxqEI5EvBo2Ysy4gQNHhRynKpw4
	    UaOFDRk4dOzgUQFCDx8hK7h4iGNlhQo/ekAAEuQUgJsVaAQFIOTHECA7e/ps
	    RERIEaRAYC5lZMTH0aRTGTV9utPHESRJjnxdSsTHUyU+VBhQAXRq2SJlCrMy
	    JaJU7ikMgQAAIf5oQ3JlYXRlZCBieSBCTVBUb0dJRiBQcm8gdmVyc2lvbiAy
	    LjUNCqkgRGV2ZWxDb3IgMTk5NywxOTk4LiBBbGwgcmlnaHRzIHJlc2VydmVk
	    Lg0KaHR0cDovL3d3dy5kZXZlbGNvci5jb20AOw==
	}]

    button $f.zoom_fit \
	-command [list ::gui::_canvas_zoom_fit $f.canvas] \
	-image [image create photo window_fullscreen-22 -data {
	    R0lGODlhFgAWAIUAAISq5ISq3Hyq3AR2xAT+BLTW/KTO/JTG/Hy6/Gyy/Eyi
	    /DSa/CSO/AyG/AR+/ARyvKzS/IS+/GSy/ASC/Hym3Nzm/NTi/NTm/Mzi/Mze
	    /MTe/MTa9Pz+/PQeLOweJPQeJPT6/Oz6/Oz2/OTy/OT2/Lza/Nzy/NTy/Mzu
	    /NTu/Mzq/LTa/KzW9LTW9AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAQALAAAAAAWABYAAAb/
	    QEAgIBASjcVhckBoBgqGAyKhWDAajqw2+2gSAAXIISKpMiaTrbbbpFQsF7jF
	    gsFkMJqMRrPRsL8cHR4eHxwgICEhIiEfgx4bTE4chB6IICIgIyAkJJQlfwEc
	    jYOHISSmnY6QXgEglY6bIpyDHxWPfwCHh5UkmSImIh4VJCarbbqYnCS/JiYj
	    JicoBbjIycTEzSnaBZEEFJeXIiPYIyMVJ9oqK9SYIh8k480nJx8mKtLd3x6p
	    Hs0mKSYcofAwzQuAYI7g+auQgtCHDyz+UEhVy1kFEyhQVLjgqGATAPw+jACo
	    LePDQSzybVjJssSKEgVWFGjBgiaEPwMe5NypsyfPEAc6vQgdSrSo0aNIkxI1
	    EAQAIf5oQ3JlYXRlZCBieSBCTVBUb0dJRiBQcm8gdmVyc2lvbiAyLjUNCqkg
	    RGV2ZWxDb3IgMTk5NywxOTk4LiBBbGwgcmlnaHRzIHJlc2VydmVkLg0KaHR0
	    cDovL3d3dy5kZXZlbGNvci5jb20AOw==
	}]

    button $f.zoom_reg \
	-command [list ::gui::_canvas_zooming_mode $f.canvas] \
	-image [image create photo viewmag-22 -data {
	    R0lGODlhFgAWAIYAAPwCBHSe1GyWzFyOxFSGvER6rDxyrHym3MTm/Nzy/Oz6
	    /OTy/Ex+rBxelMzq/Oz2/Pz+/PT+/PT6/OT2/NTu/ARKhLzi/Mzm/Lze/NTq
	    /Nzq/Mzi/GyezMTi/LTa/Nzu/LTW/Lza/LzW/LTS/KzO/KzS/KTK/KzK/KTG
	    /KzG/Dx6rLTe/JzC/KS+/Jy6/JS69CRmnJSy/JSu/Iyq/AROhKTC/JS2/Iyi
	    /Hye9LSuBISe/HyS9HSK9Pz+BKRiDPyWHMSOLKxmDKRaBMySXFwyBMSOVFw2
	    BMySLAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAAAA
	    LAAAAAAWABYAAAf/gACCggECAwQFBoOLjIIHAggICQoLCQwNjYwHCA4PEBAR
	    EhITFBWZjhYLn54PrQsXGKaZAhkSnxAKrQkaG7GZHBS2oq67HR4YHrKLAh/C
	    rQ8LGhcgISAiyoMECQ8Sz9IiIyMkJSTYggQbHR8Z3+IkJicoKeYAKhgrGCEi
	    JSUnKSwtXLh4QQ8AjGogSohIgUJgDBkzaJxqEI5EvBo2Ysy4gQNHhRynKpw4
	    UaOFDRk4dOzgUQFCDx8hK7h4iGNlhQo/ekAAEuQUgJsVaAQFIOTHECA7e/ps
	    RERIEaRAYC5lZMTH0aRTGTV9ulNqVkFEfDw94vUrgLBFwppdRISI0rWNA+wE
	    AgAh/mhDcmVhdGVkIGJ5IEJNUFRvR0lGIFBybyB2ZXJzaW9uIDIuNQ0KqSBE
	    ZXZlbENvciAxOTk3LDE5OTguIEFsbCByaWdodHMgcmVzZXJ2ZWQuDQpodHRw
	    Oi8vd3d3LmRldmVsY29yLmNvbQA7
	}]

    #----------------------------------------------------
    #  Grid buttons at the top, then the canvas and
    #  scrollbars.  Make the canvas and scrollbars
    #  resizable.
    #----------------------------------------------------
    grid $f.zoom_in $f.zoom_out $f.zoom_fit $f.zoom_reg
    grid $f.canvas -columnspan 5 -sticky news
    grid $f.hscroll -columnspan 5 -sticky ew
    grid $f.vscroll -row 1 -column 5 -sticky ns
    grid rowconfigure $f 1 -weight 1
    grid columnconfigure $f 4 -weight 1

    #----------------------------------------------------
    #  Bindings
    #  If the canvas resizes, do a redraw.
    #  Select on click
    #  Modify on double-click
    #----------------------------------------------------
    bind $::gui::_canvas <Configure> {	::gui::_canvas_redraw }

    bind $::gui::_canvas <ButtonRelease-1> {::gui::_canvas_select %W %x %y}
    bind $::gui::_canvas <Double-ButtonRelease-1> {
	::gui::updateStructure [$::gui::_tree selection get]
    }

    #----------------------------------------------------
    #  Create drawing visitor, which traverses the
    #  layer stackup, creating canvas elements.
    #----------------------------------------------------
    canvasDraw ::gui::_canvasDraw_visitor

    #----------------------------------------------------
    #  Set the initial scale factor
    #----------------------------------------------------
    set ::gui::canvasScaleFactor 1.0

    return $f
}




##################################################################
# Build the main user window.
##################################################################
proc ::gui::_createTitleframeElements { parent } {

    set f [frame $parent.titleframe]

    #--------------------------------------------------------------
    #  Create a project title (description) which spans the
    #  window.
    #--------------------------------------------------------------
    set ltitle [label $f.ltitle -text "Title"]
    set etitle [entry $f.etitle -textvariable ::gui::_title]
    set lunits [label $f.lunits -text "Default Units"]
    set cbunits [ComboBox $f.cbunits -width 10 \
		     -values {mils microns inches meters} \
		     -textvariable ::Stackup::defaultLengthUnits \
		     -modifycmd ::gui::_setDefaultUnits ]

    grid $ltitle $etitle $lunits $cbunits -sticky ew -padx 4 -pady 4
    grid columnconfigure $f 1 -weight 1



    #--------------------------------------------------------------
    #  Default parameters for this model
    #--------------------------------------------------------------
    set ::Stackup::defaultLengthUnits "meters"

#    set ::Stackup::frequency 1e9
#    guiBuildLabelEntry $parent ::Stackup::couplingLength "Coupling Length:"
#    guiBuildLabelEntry $parent ::Stackup::riseTime "Risetime (ps):"
#    guiBuildLabelEntry $parent ::Stackup::frequency "Frequency :"
    #    guiBuildLabelEntry $parent ::gui::_num_c_segs "ConductorSegments:"
#    guiBuildLabelEntry $parent ::gui::_num_p_segs "DielectricSegments:"


    return $f
}


#------------------------------------------------------
#  On global variables and GUI Dialogs
#
#  We want to create and modify CSDL objects from
#  the GUI, and be able to set the object's
#  attributes (public variables) from the GUI dialogs.
#
#  We define a namespace array (::gui::dialog())
#  which contains entries for all various
#  dialog attributes.  This array holds the
#  -textvariables for the data entry dialog entries,
#  comboboxes, spinboxes.
#
#  For creating and modifying CSDL structures,
#  we index the array by CSDL structure type
#  (e.g., RectangleConductors) and attribute name
#  (e.g., width).  So the textvariable for the
#  entry would be of the forms
#    $::gui::dialog(RectangleConductors,width)
#    $::gui::dialog($type,$attribute)
#
#  Each dialog also has special attributes for 'name',
#  which is the name of the corresponding CSDL object
#  (without the :: specifiers), and 'dialog', which is
#  the name of the dialog widget for this type.
#
#------------------------------------------------------



#------------------------------------------------------
#
#  Create Structure Dialogs
#
#  Data entry dialogs for the various cross section
#  structures are very similar.  We construct them as
#  a grid of labels and entries on a dialog.  Since
#  they are so similar, we can create them with a
#  common procedure, using switches to determine
#  which widgets to create and how to grid them.
#
#------------------------------------------------------

proc ::gui::_createStructureDialog { type } {
#------------------------------------------------------
# Rectangle Dielectrics Form
#------------------------------------------------------

    #  Create dialog, with title based on $type
    set ::gui::dialog($type,dialog) .dialog$type
    regsub -all {[A-Z]} $type { \0} dialogTitle
    set dlg [Dialog $::gui::dialog($type,dialog) -title $dialogTitle \
	    -side bottom -anchor e -default 0 -cancel 3]
    $dlg add -text "Add"
    $dlg add -text "Modify"
    $dlg add -text "Delete"
    $dlg add -text "Cancel"


    #------------------------------------------------------
    #  Different widgets for each type of form.
    #  Simple attributes are just a label and an entry.
    #  Complex attributes have special widgets, such as
    #  comboboxes or spinboxes.
    #------------------------------------------------------
    switch -exact $type {

	GroundPlane {
	    set simpleAttributes [list name thickness]
	}

	DielectricLayer {
	    set simpleAttributes [list name permeability thickness]
	    set complexAttributes(permittivity) 1
	    set complexAttributes(lossTangent) 1
	}

	RectangleDielectric {
	    set simpleAttributes \
		[list name permeability width height "x Offset" "y Offset"]
	    set complexAttributes(permittivity) 1
	    set complexAttributes(lossTangent) 1
	}

	RectangleConductors {
	    set simpleAttributes \
		[list name width height pitch "x Offset" "y Offset"]
	    set complexAttributes(conductivity) 1
	    set complexAttributes(number) 1
	}

	TrapezoidConductors {
	    set simpleAttributes [list name topWidth bottomWidth height \
				      pitch "x Offset" "y Offset"]
	    set complexAttributes(conductivity) 1
	    set complexAttributes(number) 1
	}

	CircleConductors {
	    set simpleAttributes \
		[list name diameter pitch "x Offset" "y Offset"]
	    set complexAttributes(conductivity) 1
	    set complexAttributes(number) 1
	}
    }

    #------------------------------------------------------
    #  Create widgets
    #------------------------------------------------------
    set w [$dlg getframe]

    foreach item $simpleAttributes {
	set varname [string map {" " ""} $item]
	set l$varname [label $w.l$varname -text [string totitle $item]]
	set e$varname [entry $w.e$varname -width 10 \
			   -textvariable ::gui::dialog($type,$varname)]
    }

    if { [info exists complexAttributes(permittivity)] } {
	set lpermitt [label $w.lpermitt -text "Permittivity"]
	set cbpermitt [ComboBox $w.cbpermitt -width 10 \
			     -values $::gui::_permittivityList \
			     -textvariable ::gui::dialog($type,permittivity)]
    }

    if { [info exists complexAttributes(lossTangent)] } {
	set llosstan [label $w.llosstan -text "Loss Tangent"]
	set cblosstan [ComboBox $w.cblosstan -width 10 \
			       -values $::gui::_losstangentList \
			       -textvariable ::gui::dialog($type,lossTangent)]
    }

    if { [info exists complexAttributes(conductivity)] } {
	set lconduct [label $w.lconduct -text "Conductivity"]
	set cbconduct [ComboBox $w.cbconduct -width 10 \
			   -values $::gui::_conductivityList \
			   -textvariable ::gui::dialog($type,conductivity)]
    }

    if { [info exists complexAttributes(number)] } {
	set lnumber [label $w.lnumber -text "Number"]
	set snumber [spinbox $w.snumber -width 5 -from 1 -to 100\
			 -textvariable ::gui::dialog($type,number)]
    }


    #------------------------------------------------------
    #  Manage widgets in a grid and set the stacking
    #  order for tab-traversal.
    #  (Different arrangement for each dialog)
    #------------------------------------------------------
    switch -exact $type {

	GroundPlane {
	    grid $lname      $ename      -sticky ew -padx 4 -pady 4
	    grid $lthickness $ethickness -sticky ew -padx 4 -pady 4
	    grid $lname $lthickness -sticky e
	    foreach e [list $ename $ethickness] {
		raise $e
	    }
	}

	DielectricLayer {
	    grid $lname      $ename - -      -sticky ew -padx 4 -pady 4
	    grid $lpermitt   $cbpermitt $lpermeability $epermeability \
		-sticky ew -padx 4 -pady 4
	    grid $llosstan   $cblosstan      -sticky ew -padx 4 -pady 4
	    grid $lthickness $ethickness     -sticky ew -padx 4 -pady 4

	    grid $lname $lpermitt $lpermeability \
		$llosstan $lthickness -sticky e
	    foreach e [list $ename $cbpermitt $epermeability \
			   $cblosstan $ethickness ] {
		raise $e
	    }
	}

	RectangleDielectric {
	    grid $lname     $ename - -       -sticky ew -padx 4 -pady 4
	    grid $lpermitt $cbpermitt $lpermeability $epermeability \
		                              -sticky ew -padx 4 -pady 4
	    grid $llosstan  $cblosstan        -sticky ew -padx 4 -pady 4
	    grid $lwidth    $ewidth           -sticky ew -padx 4 -pady 4
	    grid $lheight   $eheight          -sticky ew -padx 4 -pady 4
	    grid $lxOffset $exOffset $lyOffset $eyOffset \
		                              -sticky ew -padx 4 -pady 4
	    grid $lname $lpermitt $lpermeability $llosstan \
		$lwidth $lheight $lxOffset $lyOffset \
		-sticky e
	    foreach e [list $ename $cbpermitt $epermeability \
			   $cblosstan $ewidth $eheight $exOffset $eyOffset] {
		raise $e
	    }
	}

	RectangleConductors {
	    grid $lname    $ename - -             -sticky ew -padx 4 -pady 4
	    grid $lconduct $cbconduct - -         -sticky ew -padx 4 -pady 4
	    grid $lwidth   $ewidth    $lnumber  $snumber  \
		                                  -sticky ew -padx 4 -pady 4
	    grid $lheight  $eheight   $lpitch   $epitch \
		                                  -sticky ew -padx 4 -pady 4
	    grid $lxOffset $exOffset  $lyOffset $eyOffset \
		                                  -sticky ew -padx 4 -pady 4
	    grid $lname $lconduct $lwidth $lheight $lnumber $lpitch \
		$lxOffset $lyOffset -sticky e
	    foreach e [list $ename $cbconduct $ewidth $snumber \
			   $eheight $epitch $exOffset $eyOffset] {
		raise $e
	    }
	}

	TrapezoidConductors {
	    grid $lname        $ename - -         -sticky ew -padx 4 -pady 4
	    grid $lconduct     $cbconduct - -     -sticky ew -padx 4 -pady 4
	    grid $ltopWidth    $etopWidth         -sticky ew -padx 4 -pady 4
	    grid $lbottomWidth $ebottomWidth    $lnumber  $snumber  \
		                                  -sticky ew -padx 4 -pady 4
	    grid $lheight      $eheight   $lpitch   $epitch \
		                                  -sticky ew -padx 4 -pady 4
	    grid $lxOffset     $exOffset  $lyOffset $eyOffset \
		                                  -sticky ew -padx 4 -pady 4
	    grid $lname $lconduct $ltopWidth $lbottomWidth $lheight \
		$lnumber $lpitch $lxOffset $lyOffset -sticky e
	    foreach e [list $ename $cbconduct $etopWidth $ebottomWidth \
			   $snumber $eheight $epitch $exOffset $eyOffset] {
		raise $e
	    }
	}

	CircleConductors {
	    grid $lname     $ename - -            -sticky ew -padx 4 -pady 4
	    grid $lconduct  $cbconduct - -        -sticky ew -padx 4 -pady 4
	    grid $ldiameter $ediameter $lnumber  $snumber  \
		                                  -sticky ew -padx 4 -pady 4
	    grid    x           x      $lpitch   $epitch \
		                                  -sticky ew -padx 4 -pady 4
	    grid $lxOffset $exOffset  $lyOffset $eyOffset \
		                                  -sticky ew -padx 4 -pady 4
	    grid $lname $lconduct $ldiameter $lnumber $lpitch \
		$lxOffset $lyOffset -sticky e
	    foreach e [list $ename $cbconduct $ediameter $snumber \
			   $epitch $exOffset $eyOffset] {
		raise $e
	    }
	}
    }

    #  Enable resizing (of columns 1 and 3)
    grid columnconfigure $w {1 3} -weight 1

    pack $w -expand yes -fill both

    return $dlg
}


#------------------------------------------------------
#
#  ::gui::_createBemRunDialog
#
#  Simulation controls and "run" button
#
#------------------------------------------------------
proc ::gui::_createBemRunDialog { } {

    set dlg [Dialog .bemRun \
		 -title "Run Boundary Element Method (BEM) MMTL" \
		-homogeneous 1 -side bottom -anchor e -default 0 -cancel 1]
    $dlg add -text Run
    $dlg add -text Cancel
    set dlgf [$dlg getframe]

	#  Control the simulation with these values
    set cframe [TitleFrame $dlgf.controls -text "Simulation Controls"]
    set f [$cframe getframe]
    set lclen [label $f.lclen -text "Coupling Length"]
    set eclen [entry $f.eclen -width 10 \
		   -textvariable ::Stackup::couplingLength]
    set lrise [label $f.lrise -text "Risetime (ps)"]
    set erise [entry $f.erise -width 10 -textvariable ::Stackup::riseTime]
    set lcseg [label $f.lcseg -text "Conductor Segments"]
    set ecseg [entry $f.ecseg -width 10 -textvariable ::gui::_num_c_segs]
    set ldseg [label $f.ldseg -text "Dielectric Segments"]
    set edseg [entry $f.edseg -width 10 -textvariable ::gui::_num_p_segs]
    grid $lclen $eclen $lcseg $ecseg -sticky news -padx 4 -pady 4
    grid $lrise $erise $ldseg $edseg -sticky news -padx 4 -pady 4

    pack $cframe -side top -padx 4 -pady 4

    # save for later
    set ::gui::dialog(bemRun,dialog) $dlg
}

#------------------------------------------------------
#
#  ::gui::_createBemLogDialog
#
#  Simple log window and close button
#
#------------------------------------------------------
proc ::gui::_createBemLogDialog {} {

    set dlg [Dialog .bemLog \
		 -title "Boundary Element Method (BEM) MMTL Run Log" \
		-homogeneous 1 -side bottom -anchor e -default 0 -cancel 1]
    $dlg add -text "Show Results"
    $dlg add -text "Close"

    set sw [ScrolledWindow [$dlg getframe].sw]
    set txt [text $sw.text]
    $sw setwidget $txt
    pack $sw -padx 4 -pady 4 -expand true -fill both

    # save for later
    set ::gui::dialog(bemLog,dialog) $dlg
    set ::gui::dialog(bemLog,text) $txt
}


#------------------------------------------------------
#
#  ::gui::_createCalcRLRunDialog
#
#  Simulation controls and "run" button
#
#------------------------------------------------------
proc ::gui::_createCalcRLRunDialog { } {

    set dlg [Dialog .calcRLRun \
		 -title "Run Wavelet-based RL Calculator" \
		 -homogeneous 1 -side bottom -anchor e -default 0 -cancel 1]
    $dlg add -text Run
    $dlg add -text Cancel
    set dlgf [$dlg getframe]

    #  Control the simulation with these values
    set cframe [TitleFrame $dlgf.controls -text "Simulation Controls"]
    set f [$cframe getframe]
    set lfreq [label $f.lfreq -text "Frequencies"]
    set efreq [entry $f.efreq -width 30 \
		   -textvariable ::gui::calcRLfrequencies]

    grid $lfreq $efreq  -sticky news -padx 4 -pady 4

    pack $cframe -side top -padx 4 -pady 4

    # save for later
    set ::gui::dialog(calcRLRun,dialog) $dlg
}


#------------------------------------------------------
#
#  ::gui::_createCalcRLLogDialog
#
#  Simple log window and close button
#
#------------------------------------------------------
proc ::gui::_createCalcRLLogDialog {} {

    set dlg [Dialog .calcRLLog \
		 -title "Wavelet-based RL Calculator Run Log" \
		-homogeneous 1 -side bottom -anchor e -default 0 -cancel 1]
    $dlg add -text "Show Results"
    $dlg add -text "Close"

    set sw [ScrolledWindow [$dlg getframe].sw]
    set txt [text $sw.text]
    $sw setwidget $txt
    pack $sw -padx 4 -pady 4 -expand true -fill both

    # save for later
    set ::gui::dialog(calcRLLog,dialog) $dlg
    set ::gui::dialog(calcRLLog,text) $txt
}

#------------------------------------------------------
#
#  ::gui::_createCalcCapLogDialog
#
#  Simple log window and close button
#
#------------------------------------------------------
proc ::gui::_createCalcCapLogDialog {} {

    set dlg [Dialog .calcCapLog \
		 -title "Wavelet-based Capacitance Calculator Run Log" \
		-homogeneous 1 -side bottom -anchor e -default 0 -cancel 1]
    $dlg add -text "Show Results"
    $dlg add -text "Close"

    set sw [ScrolledWindow [$dlg getframe].sw]
    set txt [text $sw.text]
    $sw setwidget $txt
    pack $sw -padx 4 -pady 4 -expand true -fill both

    # save for later
    set ::gui::dialog(calcCapLog,dialog) $dlg
    set ::gui::dialog(calcCapLog,text) $txt
}


##################################################################
# Popup the various data entry forms
##################################################################
proc ::gui::_activateClearForm {pane varName} {
    $::gui::_pageManager raise $pane
    set $varName ""
}


##################################################################
# Display file name on main window title bar
##################################################################
proc ::gui::_setNodeName {} {

    wm title . "TNT:  $::gui::_nodename"
}

#----------------------------------------------------------------
#
#  _controlframeElements
#
#  Build the control frame elements, including the "new"
#  structure buttons and the tree view of the stackup.
#----------------------------------------------------------------
proc ::gui::_createControlframeElements { parent } {

    set f [frame $parent.controlframe]

    #--------------------------------------------------------------
    #  Create "New Structure" buttons
    #
    #  Each button opens a different dialog and creates a new
    #  structure object for the layer stackup.
    #--------------------------------------------------------------
    set newbuttons [TitleFrame $f.newbuttons -text "Create New Structure"]
    set bb [ButtonBox [$newbuttons getframe].buttons -orient vertical]

    $bb add -text "New Ground Plane" \
	-command [list ::gui::newStructure GroundPlane]
    $bb add -text "New Dielectric Layer" \
	-command [list ::gui::newStructure DielectricLayer]
    $bb add -text "New Dielectric Block" \
	-command [list ::gui::newStructure RectangleDielectric]
    $bb add -text "New Rectangle Conductors" \
	-command [list ::gui::newStructure RectangleConductors]
    $bb add -text "New Trapezoid Conductors" \
	-command [list ::gui::newStructure TrapezoidConductors]
    $bb add -text "New Circular Conductors"  \
	-command [list ::gui::newStructure CircleConductors]

    #--------------------------------------------------------------
    #  We'll create the corresponding dialogs later, on demand,
    #  because if we try to create them while the main window is
    #  withdrawn, some window managers (from Microsoft) won't
    #  correctly handle transient dialogs.  But we must set
    #  the dialog name variable here, so that 'winfo exists' works.
    #--------------------------------------------------------------
    set ::gui::dialog(GroundPlane,dialog) ""
    set ::gui::dialog(DielectricLayer,dialog) ""
    set ::gui::dialog(RectangleDielectric,dialog) ""
    set ::gui::dialog(RectangleConductors,dialog) ""
    set ::gui::dialog(TrapezoidConductors,dialog) ""
    set ::gui::dialog(CircleConductors,dialog) ""
    set ::gui::dialog(canvasPrint,dialog) ""

    set ::gui::dialog(bemRun,dialog) ""
    set ::gui::dialog(bemLog,dialog) ""

    set ::gui::dialog(calcRLRun,dialog) ""
    set ::gui::dialog(calcRLLog,dialog) ""

    set ::gui::dialog(calcCapLog,dialog) ""

    set ::gui::dialog(result,dialog) ""


    #------------------------------------------------------
    #  Create tree view of stackup
    #------------------------------------------------------
    set treeFrame [TitleFrame $f.treeframe -text "Layer Stackup"]

    set sw    [ScrolledWindow [$treeFrame getframe].sw \
		   -relief sunken -borderwidth 3]
    set ::gui::_tree [Tree $sw.tree -relief flat \
			  -borderwidth 0 -width 15 \
			  -highlightthickness 0 -redraw 1  \
			  -dropenabled 1 -dragenabled 1 \
			  -selectbackground red \
			  -showlines false \
			  -dragevent 1 -droptypes {
			      TREE_NODE    {copy {} move {} link {}}
			      LISTBOX_ITEM {copy {} move {} link {}}
			  } \
			  -dropcmd "::gui::_drpendComp" \
			  -dropovermode p ]

    $sw setwidget $::gui::_tree

    #  We have to pack widgets into the TitleFrame frame
    pack $bb           -expand true -fill both
    pack $sw           -expand true -fill both

    #  But we grid the TitleFrames for nice resizing
    grid $newbuttons $treeFrame -sticky news -padx 4 -pady 4
    grid columnconfigure $f 1 -weight 1

    #-------------------------------------------
    # Bind MB1 click on a tree node to be a select of
    # that item.  Double-click opens a modify dialog.
    #-------------------------------------------
    $::gui::_tree bindText  <ButtonPress-1> ::gui::highlightItem
    $::gui::_tree bindText  <Double-ButtonPress-1> ::gui::updateStructure


    return $f

}


##################################################################
# Popup the form for the selected component.
##################################################################
proc ::gui::_popupForm { objnme } {

    set class [$objnme info class]
    #  Raise the proper page, based on class name
    set pageName [string trim $class ":"]
    $::gui::_pageManager raise $pageName
}


