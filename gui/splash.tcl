#----------------------------------------------------------------
#
#  splash.tcl
#
#  TNT splash window
#
#  Create and display the requested toplevel splash window.
#  Closing the window just withdraws it, so it can be
#  deiconified later, as a sort of "help about" screen.
#
#  Bob Techentin
#  January 21, 2004
#  Copyright Mayo Foundation, 2004.  All Rights Reserved
#
#  $Id: splash.tcl,v 1.5 2004/07/22 13:16:21 techenti Exp $
#
#----------------------------------------------------------------



proc ::gui::splash { w } {

    #  Toplevel is already created.  Manage it here.
    wm withdraw $w
    wm protocol $w WM_DELETE_WINDOW [list wm withdraw $w]
    wm title $w "Mayo SPPDG Transmission Line Modeling Suite"
    $w configure -background white

    #  create components of the splash screen
    image create photo img1 -file [file join $::gui::LIBRARY MayoSPPDGlogo.gif]
    set lab0 [label $w.logo -image img1]
    set lab1 [label $w.label1 -justify center -font {Helvetica 36} \
		  -text "TNT $::TNT_VERSION"]
    set lab2 [label $w.label2 -justify center -font {Helvetica 20} -text \
		  "Transmission Line Electromagnetic Modeling Tool Suite"]
    set lab3 [label $w.label3 -justify left -font {Helvetica 12} -text \
		  "Copyright Mayo Foundation 2002-2004.  All Rights Reserved."]
    set lab4 [label $w.label4 -justify left -font {Helvetica 10} \
		  -wraplength 30c -text \
"TNT comes with ABSOLUTELY NO WARRANTY.  This is free software,
licensed under the GNU General Public License, and you are welcome to
redistribute it under certain conditions.  See the license for details."]

    #  Pack 'em up, and make it look nice
    foreach widget [list $lab0 $lab1 $lab2 $lab3 $lab4] {
	pack $widget -padx 20 -pady 20 -side top
	$widget configure -background white -borderwidth 0
    }
    pack $lab0 -anchor w

    #  Create 'close' and 'show license' buttons
    set bf [frame $w.buttonframe -background white]
    set closeb [button $bf.closeb -text "Close" -background white\
		    -command [list wm withdraw $w]]
    set licenseb [button $bf.licenseb -text "Show License" -background white \
		      -command [list ::gui::showLicense]]
    grid $closeb $licenseb -padx 20 -pady 20 -sticky news
    grid columnconfigure $bf {0 1} -weight 1 -uniform 1

    pack $bf -side top

    #  center the splash screen
    update idletasks
    set x [expr {[winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \
		     - [winfo vrootx [winfo parent $w]]}]
    set y [expr {[winfo screenheight $w]/2 - [winfo reqheight $w]/2 \
		     - [winfo vrooty [winfo parent $w]]}]
    wm geom $w +$x+$y
    wm deiconify $w
    update
}

proc ::gui::showLicense {} {
    # License file, COPYING, should be installed
    # just above the bin directory in $::scriptDir,
    # but in "developer mode," it is found in
    # $::scriptDir.

    if { $::tcl_platform(platform) eq "windows" } {
	set filename COPYING.txt
    } else {
	set filename COPYING
    }
    set licenseFile [file join $::scriptDir .. $filename]
    if { [file exists $licenseFile] } {
	::gui::guiPopupFile $licenseFile
    } else {
	set licenseFile [file join $::scriptDir $filename]
	if { [file exists $licenseFile] } {
	    ::gui::guiPopupFile $licenseFile
	} else {
	    tk_messageBox -message "License file 'COPYING' not found."
	}
    }
}

