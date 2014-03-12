#! /bin/sh
# restart using wish \
exec wish "$0" "$@"

#----------------------------------------------------------------
#
#  tnt.tcl
#
#  Main entry point for the TNT interface program
#  for Mayo SPPDG MMTL and other transmission line
#  simulators.
#
#  This module creates the BWidget mainframe framework
#  for the main window, and defines procedures for
#  accessing numerous other integrated simulation programs
#  and utilities.
#
#  The actual GUI is constructed by [createElements]
#
#
#  Copyright 2002-2004 Mayo Foundation.  All Rights Reserved.
#  $Id: tnt.tcl,v 1.27 2004/09/10 20:04:53 techenti Exp $
#
#----------------------------------------------------------------


set TNT_VERSION 1.2.2

#----------------------------------------------------------------
#  From whence is this script run?
#  For there shall we find the rest.
#----------------------------------------------------------------
set ::scriptDir [file dirname [info script]]

#----------------------------------------------------------------
#  TNT's application-specific libraries are usually found
#  in the ../lib directory, relative to this script's location.
#----------------------------------------------------------------
set ::auto_path [linsert $auto_path 0 [file join $scriptDir .. lib]]

#----------------------------------------------------------------
#  TNT executes binaries out of the same bin directory
#  as this script.  Add this directory to the head of our
#  path, so [auto_execok] can easily find executables.
#----------------------------------------------------------------
if { $::tcl_platform(platform) eq "windows" } {
    set sep ";"
} else {
    set sep ":"
}
set ::env(PATH) $::scriptDir$sep$::env(PATH)


#----------------------------------------------------------------
#  TNT's documentation is usually found in ../doc,
#  relative to this script's location.
#----------------------------------------------------------------
set ::docDirPath [file join $scriptDir .. doc]

#----------------------------------------------------------------
#  But for 'developer mode,' things are a little messy.
#  Application libraries are subdirectories under $scriptDir.
#  Executables (e.g., bem) are found in their respecitve
#  subdirectories, and docs are found under $scriptDir/doc.
#----------------------------------------------------------------
if { [file exists [file join $scriptDir bem]] } {
    lappend ::auto_path $scriptDir [file join $scriptDir bem]
    set ::env(PATH) [file join $::scriptDir bem]$sep$::env(PATH)
}
if { [file exists [file join $scriptDir calcRL]] } {
    lappend ::auto_path $scriptDir [file join $scriptDir calcRL]
    set ::env(PATH) [file join $::scriptDir calcRL]$sep$::env(PATH)
}
if { [file exists [file join $scriptDir calcCAP]] } {
    lappend ::auto_path $scriptDir [file join $scriptDir calcCAP]
    set ::env(PATH) [file join $::scriptDir calcCAP]$sep$::env(PATH)
}
if { [file exists [file join $scriptDir doc]] } {
    set ::docDirPath [file join $scriptDir doc]
}

#----------------------------------------------------------------
#  Splash Screen
#
#  Before anything else, find the GUI components, and throw
#  up a nice splash screen while we're doing the rest of the
#  initialization.
#
#  The splash screen is part of the GUI package, but we don't
#  want to do a package require.  That's too slow.  So we
#  create the "gui" namespace, locate the source directory,
#  and directly source the splash screen code.
#----------------------------------------------------------------
package require Tk
wm withdraw .
toplevel .splash

namespace eval gui {
    set LIBRARY [file join $::scriptDir .. lib]
    set flist [glob -nocomplain [file join $LIBRARY gui* splash.tcl]]
    if { [llength $flist] == 0 } {
	set flist [glob -nocomplain [file join $::scriptDir gui* splash.tcl]]
    }
    set splash [lindex $flist 0]
    set LIBRARY [file normalize [file dirname $splash]]
}
source $::gui::splash
::gui::splash .splash


#----------------------------------------------------------------
#  Load Required Packages
#----------------------------------------------------------------
package require Itcl
package require BWidget


#----------------------------------------------------------------
#  Load application-specific packages
#----------------------------------------------------------------
package require gui
package require csdl
package require units
package require bem
#  Optional packages
set femPkg     [catch { package require fem }]
set calcRLpkg  [catch { package require calcRL}]
set calcCAPpkg [catch { package require calcCAP}]



#----------------------------------------------------------------
# Create the main window.
#----------------------------------------------------------------
proc createMain { mmtlInitialize } {

    #----------------------------------------------------------------
    # BWidget Menu description
    #  Each menubar entry is
    #      menuname tags menuid tearoff menuentries
    #  where menuentries are one of
    #      separator
    #      command     menuname ?tags? ?desc? ?accel? ?option value?...
    #      checkbutton menuname ?tags? ?desc? ?accel? ?option value?...
    #      radiobutton menuname ?tags? ?desc? ?accel? ?option value?...
    #      radiobutton menuname ?tags? ?desc? ?accel? ?option value?...
    #      cascade     menuname tags menuID tearoff menuentries
    #----------------------------------------------------------------
    set descmenu {
        "&File" all file 0 {
	    {command "Ne&w" {} "Create a new file" {Ctrl n} \
		 -command ::gui::guiNew}
	    {command "&Open..." {} "Open existing file" {Ctrl o} \
		 -command ::gui::guiOpenExisting}
	    {command "&Save" {} "Save cross section" \
		 {Ctrl s} -command ::gui::guiSave}
	    {command "Sa&ve As..." {} \
		 "Save cross section to new file" {Ctrl v} \
		 -command ::gui::guiSaveAs}
	    {command "&Print..." {} "Print the cross section" {Ctrl p} \
		 -command ::gui::_canvas_print}
            {command "E&xit" {} "Exit" {Ctrl x} -command _exit}
	}
        "View" all view 0 {
	    {checkbutton "Console" {} "Show/hide command console" {} \
		 -variable ::gui::_showConsole}
	    {checkbutton "Annotation" {} \
		 "Show/hide dimensions and descriptions" {} \
		 -variable ::gui::_annotateFlag \
		 -command gui::_canvas_toggleAnnotation}
	    {command "Redraw" {} "Redraw cross section" \
		 {} -command gui::_canvas_redraw}
	}
        "&Setup" all setup 0 {
	    {command "Re-read conductivity list" {} \
		 "Read in a new conductivity list" {} \
		 -command ::gui::guiReadNewConductivityList}
	    {command "Re-read lossTangent list" {} \
		 "Read in a new lossTangent list" {} \
		 -command ::gui::guiReadNewLossTangentList}
	    {command "Re-read permittivity list" {} \
		 "Read in a new permittivity list" {} \
		 -command ::gui::guiReadNewPermittivityList}
        }
        "&BEM" all bem 0 {
	    {command "Run BEM MMTL S&imulation" {} \
		 "Save cross section file and run MMTL" \
		 {Ctrl i} -command ::gui::saveAndRunBem}
	    {command "View BEM MMTL Result" {} \
		 "Vie&w BEM MMTL results" {Ctrl w} \
		 -command {::gui::guiPopupResultsFile "result"}}
	    {command "Generate &HSPICE W" {} \
		 "Generate HSPICE W-element from MMTL results" \
		 {Ctrl h} -command ::gui::writeHspiceW}
	}
	"&Sweep" all sweep 0 {
	    {command "S&weep Simulation" {} \
		 "Run current node in sweep mode" {} \
		 -command ::gui::guiRunSweepMMTL }
	    {command "View \"swept_result\" file" {} \
		 "Display swept_result file" {} \
		 -command {::gui::guiPopupResultsFile "swept_result"} }
	    {command "Write csv file" {} \
		 "Write swept results into csv file" {} \
		 -command "::gui::writeSweptCsvFile swept_result" }
	}
	"&Iterate" all iterate 0 {
	    {command "I&terate Conductor Width" {} \
		 "Sweep conductor width or diameter to achieve impendance" {} \
		 -command ::gui::guiRunIterateMMTL }
	    {command "View \"iterate_result\" file" {} \
		 "Display iterate_result file" {} \
		 -command {::gui::guiPopupResultsFile "iterate_result"}}
	    {command "Write csv file" {} \
		 "Write iterate results into csv file" {} \
		 -command "::gui::writeSweptCsvFile iterate_result" }
        }
    }

    #----------------------------------------------------------------
    #  Optional experimental wavelet simulators
    #----------------------------------------------------------------
    if { ($::calcRLpkg==0) || ($::calcCAPpkg==0) } {
	append descmenu "\"Wavelet Simulators\" all calcrl 0 \{"
    }

    if { $::calcRLpkg == 0 } {
	append descmenu {
	    {command "Run RL Calculator" \
		 {} "Run full-wave wavelet-based RL calculator" {} \
		 -command ::gui::runCalcRL}
	    {command "View RL result file" {} \
		 "Display RL Calculator result file" {} \
		 -command {::gui::guiPopupResultsFile ri.out} }
	}
    }

    if { $::calcCAPpkg == 0 } {
	append descmenu {
	    {command "Run Capacitance Calculator" \
		 {} "Run full-wave wavelet-based Capacitance calculator" {} \
		 -command ::gui::runCalcCap}
	    {command "View Capacitance result file" {} \
		 "Display Capacitance Calculator result file" {} \
		 -command {::gui::guiPopupResultsFile cap.out} }
	}
    }

    if { ($::calcRLpkg==0) || ($::calcCAPpkg==0) } {
	append descmenu "\}"
    }


    append descmenu {
	"&Help" all help 0 {
	    {command "User Guide" {} "" {} \
		 -command {::gui::tnt_help userguide}}
	    {command "About TNT" {} "" {} \
		 -command {wm deiconify .splash}}
	}
    }

    set ::gui::mainframe [MainFrame .mainframe \
			      -menu $descmenu]
    set ::gui::f1  [$::gui::mainframe getframe]
    set ::gui::f2 [::gui::guiCreateElements $::gui::f1 $mmtlInitialize]

    pack $::gui::f2         -expand true -fill both
    pack $::gui::f1         -expand true -fill both
    pack $::gui::mainframe  -expand true -fill both

    wm protocol . WM_DELETE_WINDOW {_exit}

}



#----------------------------------------------------------------
#
#  _exit
#
#  Application exit point.
#  This would be a good place to catch model changes and
#  prompt for save before exiting.
#
#----------------------------------------------------------------
proc _exit {} {

    exit
}



#----------------------------------------------------------------
#
#  main
#
#  Controls splash screen, creates main window, and opens
#  a file, if specified on the command line.
#
#----------------------------------------------------------------
proc main { argv } {

    set mmtlInitialize 1

    wm withdraw .

    #  Create main window
    createMain $mmtlInitialize
    BWidget::place . 0 0 center

    #  Swap out splash screen with main window.
    wm withdraw .splash
    wm title . "TNT Cross Section Generator"
    wm deiconify .
    update

    #  If there was a filename supplied on the command line,
    #  then open that file
    if { [llength $argv] > 0 } {
	set filename [lindex $argv 0]
	::gui::guiOpenExisting $filename
    }

}

main $argv

